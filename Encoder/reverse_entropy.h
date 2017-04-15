#include <common.h>

int reverse_signed_golomb_value(int *RLE, FILE * golomb_file, int block, int row, int col);
int16_t decoder_signed_golomb_value(uint32_t input, uint8_t *count);
void reverse_entropy(int ** QTC_FRAME, int block, int height, int width, int frame);
void rle_decode(int *COEFF_REORDER, int *RLE, int block);
void diag_to_raster(int ** QTC_BLOCK, int * COEFF_REORDER, int block);


int reverse_signed_golomb_value(int *RLE, FILE * golomb_file, int block, int row, int col) {
	int counter = 0;
	int encoded_value = 0;
	uint8_t count = 0;
	int index = 0;
	while (counter != block*block) {
		fread(&encoded_value, sizeof(int), 1, golomb_file);
		int32_t result = decode_signed_golomb_value(encoded_value, &count);
		if (result < 0) {
			RLE[index] = result;
			index++;
			int run = abs(result);
			for (int i = 0; i < run;i++) {
				fread(&encoded_value, sizeof(int), 1, golomb_file);
				int32_t result = decode_signed_golomb_value(encoded_value, &count);
				counter++;
				RLE[index] = result;
				index++;
			}
		}
		else if (result > 0) {
			RLE[index] = result;
			index++;
			for(int i =0; i< result; i++)
				counter++;
		}
		else {
			for (int i = 0; i < (block*block) - counter; i++) {
				RLE[index] = 0;
				counter = block*block;
				index++;
			}
		}
	}
	return index;
}

int16_t decoder_signed_golomb_value(uint32_t input, uint8_t *count) {
	int16_t result = 0;
	uint8_t zero_count = 0;
	uint8_t bit_count = 0;
	int16_t sign = 0;

	while (!(input & 0x1)) {
		zero_count++;
		input >>= 1;
	}

	bit_count = zero_count + 1;
	for (uint8_t i = 0; i < bit_count; i++) {
		result <<= 1;
		result |= input & 0x1;
		input >>= 1;
	}

	/*Remove the lowest bit as our sign bit.*/
	sign = 1 - 2 * (result & 0x1);
	result = sign * ((result >> 1) & 0x7FFF);

	/*Defend against overflow on min int16.*/
	bit_count += zero_count;
	if (bit_count > 0x20) {
		result |= 0x8000;
	}

	if (count) {
		*count = bit_count + zero_count;
	}

	return result;
}

void reverse_entropy(int ** QTC_FRAME, int block, int height, int width, int frame) {
#ifdef TRACE_ON
	FILE* file_re_qtc;
	FILE* file_re_reorder;
	FILE* file_re_rle;
	char buf[0x100];
	snprintf(buf, sizeof(buf), "QTC_BLOCK_DEC_%d.txt", frame);
	file_re_qtc = fopen(buf, "w");
	snprintf(buf, sizeof(buf), "REORDER_BLOCK_DEC_%d.txt", frame);
	file_re_reorder = fopen(buf, "w");
	snprintf(buf, sizeof(buf), "RLE_BLOCK_DEC_%d.txt", frame);
	file_re_rle = fopen(buf, "w");
#endif // TRACE_ON
	int row = 0;
	int col = 0;
	FILE* golomb_file;
	char mdiff_golomb_name[0x100];
	snprintf(mdiff_golomb_name, sizeof(mdiff_golomb_name), "%s\\COEFF_GOLOMB_CODING_%d", filepath, frame);
	golomb_file = fopen(mdiff_golomb_name, "rb");

	int ** QTC_BLOCK = new int*[block];
	int * RLE = new int[block*block + block*block];
	int * COEFF_REORDER = new int[block*block];
	for (int i = 0; i < block; i++)
		QTC_BLOCK[i] = new int[block];

	while (!feof(golomb_file)) {
		int total_counter=reverse_signed_golomb_value(RLE, golomb_file, block, row, col);
		rle_decode(COEFF_REORDER, RLE, block);
		diag_to_raster(QTC_BLOCK, COEFF_REORDER, block);
		for (int j = 0; j < block; j++)//Copy block into QTC_FRAME
			for (int i = 0; i < block; i++)
				QTC_FRAME[row + j][col + i]= QTC_BLOCK[j][i];
#ifdef TRACE_ON
		fprintf_RLE(file_re_rle, RLE, total_counter, row, col);
		fprintf_REORDER_BLOCK(file_re_reorder, COEFF_REORDER, block, row, col);
		fprintf_QTC_BLOCK(file_re_qtc, QTC_BLOCK, block, row, col);
#endif
		col = col + block;
		if (col == width) {
			row = row + block;
			if (row == height && col == width)//All blocks decoded
				break;
			col = 0;
		}
	}
}

void decode_mdiff(MDIFF** MDIFF_VECTOR_DIFF, int row, int col, int Frametype, FILE* mdiff_golomb) {
	int encoded_value = 0;
	uint8_t count;
	int32_t result;
	if (Frametype == IFRAME) {
		fread(&encoded_value, sizeof(int), 1, mdiff_golomb);
		result = decode_signed_golomb_value(encoded_value, &count);
		MDIFF_VECTOR_DIFF[row][col].split = result;
		fread(&encoded_value, sizeof(int), 1, mdiff_golomb);
		result = decode_signed_golomb_value(encoded_value, &count);
		MDIFF_VECTOR_DIFF[row][col].MODE = result;
	}
	else{
		fread(&encoded_value, sizeof(int), 1, mdiff_golomb);
		result = decode_signed_golomb_value(encoded_value, &count);
		MDIFF_VECTOR_DIFF[row][col].split = result;
		fread(&encoded_value, sizeof(int), 1, mdiff_golomb);
		result = decode_signed_golomb_value(encoded_value, &count);
		MDIFF_VECTOR_DIFF[row][col].X = result;
		fread(&encoded_value, sizeof(int), 1, mdiff_golomb);
		result = decode_signed_golomb_value(encoded_value, &count);
		MDIFF_VECTOR_DIFF[row][col].Y = result;
		fread(&encoded_value, sizeof(int), 1, mdiff_golomb);
		result = decode_signed_golomb_value(encoded_value, &count);
		MDIFF_VECTOR_DIFF[row][col].ref = result;
	}

}


void decode_mdiff_wrapper(MDIFF** MDIFF_VECTOR_DIFF, int height, int width, int block, int frame, int Frametype) {
	FILE* file_mdiff_golomb;
	char mdiff_golomb_name[0x100];
	snprintf(mdiff_golomb_name, sizeof(mdiff_golomb_name), "%s\\MDIFF_GOLOMB_%d", filepath, frame);
	file_mdiff_golomb = fopen(mdiff_golomb_name, "rb");
	while (!feof(file_mdiff_golomb)) {
		for (int row = 0; row < height; row = row + block) {
			for (int col = 0; col < width; col = col + block) {
				decode_mdiff(MDIFF_VECTOR_DIFF, row/block, col/block, Frametype, file_mdiff_golomb);
			}
		}
		break;
	}
	fclose(file_mdiff_golomb);
}

void rle_decode(int *COEFF_REORDER, int *RLE, int block) {
	int rle_index = 0;
	int coeff_index = 0;
	int run = 0;
	while (coeff_index<block*block) {
		if (RLE[rle_index] < 0) {
			run = abs(RLE[rle_index]);
			rle_index++;
			int counter = 0;
			while (counter < run) {
				COEFF_REORDER[coeff_index] = RLE[rle_index];
				coeff_index++;
				counter++;
				rle_index++;
			}
		}
		else if (RLE[rle_index] > 0) {
			run = RLE[rle_index];
			rle_index++;
			int counter = 0;
			while (counter < run) {
				COEFF_REORDER[coeff_index] = 0;
				coeff_index++;
				counter++;
			}
		}
		else {//RLE is 0 fill remaining with 0's
			while (coeff_index < block*block) {
				COEFF_REORDER[coeff_index] = 0;
				coeff_index++;
			}
		}
	}
}

void diag_to_raster(int ** QTC_BLOCK, int * COEFF_REORDER, int block) {
	int x = 0;
	int index = 0;
	for (int diag = 0; diag < (block * 2) - 1; diag++) {
		if (diag < block) {
			x = diag;
			for (int y = 0; y < diag + 1; y++) {
				QTC_BLOCK[y][x]= COEFF_REORDER[index];
				index++;
				x = x - 1;
			}
		}
		else {
			x = block - 1;
			for (int y = diag - block + 1; y < block; y++) {
				QTC_BLOCK[y][x]= COEFF_REORDER[index];
				index++;
				x = x - 1;
			}
		}
	}
}

