#include <common.h>
int entropy(int8_t ** QTC_FRAME, int8_t * COEFF_REORDER, int block, int8_t * RLE);
void raster_to_diag(int8_t ** QTC_FRAME, int8_t * COEFF_REORDER, int block);
int rle_encode(int8_t *COEFF_REORDER, int8_t *RLE, int block);
void fprint_coeef(int8_t ** in, int8_t * out, int block, FILE* file, int8_t *RLE, int total_counter);

int rle_encode(int8_t *COEFF_REORDER, int8_t *RLE, int block){
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
int entropy(int8_t ** QTC_FRAME, int8_t * COEFF_REORDER, int block, int8_t * RLE) {
	int total_counter;
	raster_to_diag(QTC_FRAME, COEFF_REORDER, block);
	total_counter = rle_encode(COEFF_REORDER, RLE, block);
	return total_counter;
}

void fprint_coeef(int8_t ** in, int8_t * out, int block, FILE* file, int8_t *RLE, int total_counter) {
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

void raster_to_diag(int8_t ** QTC_FRAME, int8_t * COEFF_REORDER,int block) {
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