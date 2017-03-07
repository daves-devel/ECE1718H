#include <common.h>
int entropy(int ** QTC_FRAME, int block, int * RLE);
void raster_to_diag(int ** QTC_FRAME, int * COEFF_REORDER, int block);
int rle_encode(int *COEFF_REORDER, int *RLE, int block);
void fprint_coeef(int ** in, int * out, int block, FILE* file, int *RLE, int total_counter);
uint32_t encode_signed_golomb_value(int input, uint8_t *count);
void convert_signed_golomb_value(int *RLE, int total_counter);
int16_t decode_signed_golomb_value(uint32_t input, uint8_t *count);


int rle_encode(int *COEFF_REORDER, int *RLE, int block){
	int index = 0;
	int zero_flag = 0;
	int change_flag = 1;
	int counter = 0;
	int total_counter = 0;
	for (int i = 0; i < block*block; i++) {
		if (change_flag == 1) {
			change_flag = 0;
			if (COEFF_REORDER[i] != 0)
				zero_flag = 0;
			else
				zero_flag = 1;
		}
		counter = counter + 1;
		if (zero_flag == 1) {
			if (COEFF_REORDER[i + 1] != 0 || i == (block*block) -1) {
				total_counter = total_counter + 1;//Only one increment for 0's
				if (i == (block*block) - 1)
					RLE[index] = 0;
				else
					RLE[index] = counter;
				counter = 0;
				index = total_counter;
				change_flag = 1;
			}
		}
		else {
			RLE[index + counter] = COEFF_REORDER[i];
			total_counter = total_counter + 1;//Increment for every value
			if (COEFF_REORDER[i + 1] == 0 || i == (block*block)-1) {
				RLE[index] = counter * -1;
				counter = 0;
				total_counter = total_counter + 1;//Increment for the value infront of the non-zero coefficient
				index = total_counter;
				change_flag = 1;
			}
		}
	}
	return total_counter;
}

void convert_signed_golomb_value(int *RLE, int total_counter) {
	for (int i = 0; i <= total_counter; i++) {
		uint8_t count = 0;
		uint32_t encoded_value = encode_signed_golomb_value(RLE[i], &count);
		int32_t result = decode_signed_golomb_value(encoded_value, &count);
	}
}

int entropy(int ** QTC_FRAME, int block, int * RLE) {
	int total_counter;
	int * COEFF_REORDER = new int [block*block];
	raster_to_diag(QTC_FRAME, COEFF_REORDER, block);
	total_counter = rle_encode(COEFF_REORDER, RLE, block);
	convert_signed_golomb_value(RLE, total_counter);
	return total_counter;
}

void fprint_coeef(int ** in, int * out, int block, FILE* file, int *RLE, int total_counter) {
	for (int i = 0; i < block; i++) {
		for (int j = 0; j < block; j++) {
			fprintf(file, "%02d ", in[i][j]);
		}
		fprintf(file, "\n");
	}
	for (int i = 0; i < block*block; i++)
		fprintf(file, "%02d ", out[i]);
	fprintf(file, "\n");

	for (int i = 0; i < total_counter; i++)
		fprintf(file, "%02d ", RLE[i]);
	fprintf(file, "\n");

}

void raster_to_diag(int ** QTC_FRAME, int * COEFF_REORDER,int block) {
	int x = 0;
	int index = 0;
	for (int diag = 0; diag < (block * 2) - 1; diag++) {
		if (diag < block) {
			x = diag;
			for (int y = 0; y < diag + 1; y++) {
				COEFF_REORDER[index] = QTC_FRAME[y][x];
				printf("%x ", COEFF_REORDER[index]);
				index++;
				x = x - 1;
			}
		}
		else {
			x = block-1;
			for (int y = diag-block+1; y < block; y++) {
				COEFF_REORDER[index] = QTC_FRAME[y][x];
				printf("%x ", COEFF_REORDER[index]);
				index++;
				x = x - 1;
			}
		}
	}
}

uint32_t encode_signed_golomb_value(int input, uint8_t *count) {
	uint8_t index = input;
	uint32_t result = EVX_SEXP_GOLOMB_CODES[index];

	if (count) {
		*count = EVX_SEXP_GOLOMB_SIZE_LUT[index];
	}
	return result;
}

int16_t decode_signed_golomb_value(uint32_t input, uint8_t *count) {
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
