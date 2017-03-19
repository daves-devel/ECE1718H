#include <common.h>
#define EVX_MAX_INT64           (0x7FFFFFFFFFFFFFFF)
#define EVX_MAX_INT32           (0x7FFFFFFF)
#define EVX_MAX_INT16           (0x7FFF)
#define EVX_MAX_INT8            (0x7F)

#define EVX_MAX_UINT64          (0xFFFFFFFFFFFFFFFF)
#define EVX_MAX_UINT32          (0xFFFFFFFF)
#define EVX_MAX_UINT16          (0xFFFF)
#define EVX_MAX_UINT8           (0xFF)

#define EVX_MIN_INT64           (-EVX_MAX_INT64 - 1)
#define EVX_MIN_INT32           (-EVX_MAX_INT32 - 1)
#define EVX_MIN_INT16           (-EVX_MAX_INT16 - 1)
#define EVX_MIN_INT8            (-EVX_MAX_INT8 - 1)

#define EVX_REQUIRED_BITS(n) (evx_log2((n)) + 1)

//H
inline int8_t evx_abs(int8_t value) {
	if (value == EVX_MIN_INT8) {
		return EVX_MAX_INT8;
	}
	return (value < 0 ? -value : value);
}

inline int16_t evx_abs(int16_t value) {
	if (value == EVX_MIN_INT16) {
		return EVX_MAX_INT16;
	}
	return (value < 0 ? -value : value);
}

inline int32_t evx_abs(int32_t value) {
	if (value == EVX_MIN_INT32) {
		return EVX_MAX_INT32;
	}
	return (value < 0 ? -value : value);
}

inline uint8_t evx_log2(uint8_t value) {
	return EVX_LOG2_BYTE_LUT[value];
}

inline uint8_t evx_log2(uint16_t value) {
	if (value <= 0xFF) {
		return evx_log2((uint8_t)value);
	}
	return 8 + evx_log2((uint8_t)(value >> 8));
}

inline uint8_t evx_log2(uint32_t value) {
	if (value <= 0xFFFF) {
		return evx_log2((uint16_t)value);
	}

	return 16 + evx_log2((uint16_t)(value >> 16));
}

int entropy(int ** QTC_FRAME, int block, int * RLE, int* COEFF_REORDER, FILE* golomb_file, int &bitcount);
void raster_to_diag(int ** QTC_FRAME, int * COEFF_REORDER, int block);
int rle_encode(int *COEFF_REORDER, int *RLE, int block);
uint32_t encode_signed_golomb_value(int32_t input, uint8_t *count);
uint32_t encode_signed_golomb_value(int8_t input, uint8_t *count);

void convert_signed_golomb_value(int *RLE, int total_counter, FILE* golomb_file, int &bitcount);
int16_t decode_signed_golomb_value(uint32_t input, uint8_t *count);
int entropy_wrapper(int ** QTC_FRAME, int block, int height, int width, int frame);
void fprintf_QTC_BLOCK(FILE* file, int ** in, int block, int row, int col);
void fprintf_REORDER_BLOCK(FILE* file, int * in, int block, int row, int col);
void fprintf_RLE(FILE* file, int * in, int total_counter, int row, int col);
void encode_mdiff(MDIFF** MDIFF_VECTOR_DIFF, int row, int col, int Frametype, uint32_t *bitcount, FILE* mdiff_golomb);
int encode_mdiff_wrapper(MDIFF** MDIFF_VECTOR_DIFF, int height, int width, int block, int frame, int Frametype, int row, int col);


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

void convert_signed_golomb_value(int *RLE, int total_counter, FILE * golomb_file, int &bitcount) {
	for (int i = 0; i < total_counter; i++) {
		uint8_t count = 0;
		uint32_t encoded_value = encode_signed_golomb_value(RLE[i], &count);
		fwrite(&encoded_value, sizeof(uint32_t), 1, golomb_file);
		bitcount = bitcount + count;
		int32_t result = decode_signed_golomb_value(encoded_value, &count);
	}
}

int encode_mdiff_wrapper(MDIFF** MDIFF_VECTOR_DIFF, int height, int width, int block, int frame, int Frametype, int row, int col) {
	uint32_t bitcount = 0;
	encode_mdiff(MDIFF_VECTOR_DIFF, row / block, col / block, Frametype, &bitcount, mdiff_golomb);
	return bitcount;
}

void encode_mdiff(MDIFF** MDIFF_VECTOR_DIFF, int row, int col, int Frametype, uint32_t *bitcount, FILE* mdiff_golomb ) {
	uint8_t count=0;
	uint32_t result;
	if (Frametype == IFRAME) {
		result=encode_signed_golomb_value(MDIFF_VECTOR_DIFF[row][col].MODE, &count);
		fwrite(&result, sizeof(uint32_t), 1, mdiff_golomb);
		*bitcount = count + *bitcount;
	}
	else {
		result = encode_signed_golomb_value(MDIFF_VECTOR_DIFF[row][col].X, &count);
		fwrite(&result, sizeof(uint32_t), 1, mdiff_golomb);
		*bitcount = count + *bitcount;
		result = encode_signed_golomb_value(MDIFF_VECTOR_DIFF[row][col].Y, &count);
		fwrite(&result, sizeof(uint32_t), 1, mdiff_golomb);
		*bitcount = count + *bitcount;
		result = encode_signed_golomb_value(MDIFF_VECTOR_DIFF[row][col].ref, &count);
		fwrite(&result, sizeof(uint32_t), 1, mdiff_golomb);
		*bitcount = count + *bitcount;
	}
}
int entropy_wrapper(int ** QTC_FRAME, int block, int height, int width, int frame, int row, int col) {
#ifdef TRACE_ON
	FILE* file_qtc;
	FILE* file_reorder;
	FILE* file_rle;
	char buf[0x100];
	snprintf(buf, sizeof(buf), "testdata\\QTC_BLOCK_ENC_%d.txt", frame);
	file_qtc = fopen(buf, "w");
	snprintf(buf, sizeof(buf), "testdata\\REORDER_BLOCK_ENC_%d.txt", frame);
	file_reorder = fopen(buf, "w");
	snprintf(buf, sizeof(buf), "testdata\\RLE_BLOCK_ENC_%d.txt", frame);
	file_rle = fopen(buf, "w");
#endif // TRACE_ON
	int total_counter=0;
	int bitcount=0;
	int ** QTC_BLOCK = new int*[block];
	int * RLE = new int[block*block + block*block];
	int * COEFF_REORDER = new int[block*block];
	for (int i = 0; i < block; i++)
		QTC_BLOCK[i] = new int[block];

	for (int j = 0; j < block; j++)
		for (int i = 0; i < block; i++)
			QTC_BLOCK[j][i] = QTC_FRAME[row + j][col + i];//Copy block into QTC_BLOCK
	total_counter = entropy(QTC_BLOCK, block, RLE, COEFF_REORDER, golomb_file, bitcount);
#ifdef TRACE_ON
			fprintf_QTC_BLOCK(file_qtc, QTC_BLOCK, block, row, col);
			fprintf_REORDER_BLOCK(file_reorder, COEFF_REORDER, block, row, col);
			fprintf_RLE(file_rle, RLE, total_counter, row, col);
#endif //TRACE_ON 
	delete QTC_BLOCK;
	delete RLE;
	delete COEFF_REORDER;
#ifdef TRACE_ON
	fclose(file_qtc);
	fclose(file_reorder);
	fclose(file_rle);
#endif
	return bitcount;
}

int entropy(int ** QTC_BLOCK, int block, int * RLE, int * COEFF_REORDER, FILE* golomb_file, int &bitcount) {
	int total_counter;
	raster_to_diag(QTC_BLOCK, COEFF_REORDER, block);
	total_counter = rle_encode(COEFF_REORDER, RLE, block);
	convert_signed_golomb_value(RLE, total_counter, golomb_file, bitcount);
	return total_counter;
}

void raster_to_diag(int ** QTC_BLOCK, int * COEFF_REORDER,int block) {
	int x = 0;
	int index = 0;
	for (int diag = 0; diag < (block * 2) - 1; diag++) {
		if (diag < block) {
			x = diag;
			for (int y = 0; y < diag + 1; y++) {
				COEFF_REORDER[index] = QTC_BLOCK[y][x];
				index++;
				x = x - 1;
			}
		}
		else {
			x = block-1;
			for (int y = diag-block+1; y < block; y++) {
				COEFF_REORDER[index] = QTC_BLOCK[y][x];
				index++;
				x = x - 1;
			}
		}
	}
}



uint32_t encode_signed_golomb_value(int32_t input, uint8_t *count) {
	if (input >= -128 && input < 128) {
		return encode_signed_golomb_value((int8_t)input, count);
	}

	uint32_t result = 0;
	uint32_t value = (!input ? 1 : (evx_abs((int32_t)input) << 1) | ((input >> 15) & 0x1));
	uint8_t bit_count = EVX_REQUIRED_BITS(value);

	while (value) {
		result <<= 1;
		result |= value & 0x1;
		value >>= 1;
	}

	result <<= bit_count - 1;
	bit_count <<= 1;
	bit_count -= 1;

	if (count) {
		*count = bit_count;
	}

	return result;
}

uint32_t encode_signed_golomb_value(int8_t input, uint8_t *count) {
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

void fprintf_QTC_BLOCK(FILE* file, int ** in, int block, int row, int col) {
	fprintf(file, "Block Y=%d X=%d\n", row, col);
	for (int row = 0; row < block; row++) {
		for (int col = 0; col < block; col++) {
			fprintf(file, "%04d ", in[row][col]);
		}
		fprintf(file, "\n");
	}
}
void fprintf_REORDER_BLOCK(FILE* file, int * in, int block, int row, int col) {
	fprintf(file, "Block Y=%d X=%d\n", row, col);
	for (int i=0; i<block*block; i++)
			fprintf(file, "%04d ", in[i]);
	fprintf(file, "\n");
}

void fprintf_RLE(FILE* file, int * in, int total_counter, int row, int col) {
	fprintf(file, "Block Y=%d X=%d\n", row, col);
	for (int i=0; i<total_counter; i++)
			fprintf(file, "%04d ", in[i]);
	fprintf(file, "\n");
}
