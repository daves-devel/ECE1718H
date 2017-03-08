#include <common.h>

int reverse_signed_golomb_value(int *RLE, FILE * golomb_file, int block, int row, int col);
int16_t decoder_signed_golomb_value(uint32_t input, uint8_t *count);
void reverse_entropy(int block, FILE* golomb_file, int height, int width, int frame);


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

void reverse_entropy(int block, FILE* golomb_file, int height, int width, int frame) {
#ifdef TRACE_ON
	FILE* file_re_qtc;
	FILE* file_re_reorder;
	FILE* file_re_rle;
	char buf[0x100];
	snprintf(buf, sizeof(buf), "RE_QTC_BLOCK_%d.txt", frame);
	file_re_qtc = fopen(buf, "w");
	snprintf(buf, sizeof(buf), "RE_REORDER_BLOCK_%d.txt", frame);
	file_re_reorder = fopen(buf, "w");
	snprintf(buf, sizeof(buf), "RE_RLE_BLOCK_%d.txt", frame);
	file_re_rle = fopen(buf, "w");
#endif // TRACE_ON
	int * RLE = new int[block*block + block*block];
	int row = 0;
	int col = 0;
	while (!feof(golomb_file)) {
		int total_counter=reverse_signed_golomb_value(RLE, golomb_file, block, row, col);
#ifdef TRACE_ON
		fprintf_RLE(file_re_rle, RLE, total_counter, row, col);
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