#include <common.h>

void DCTBlock(int32_t** TC_FRAME, int32_t** RES_FRAME, int row, int col, int block_size);
void IDCTBlock(int32_t **RES_FRAME, int32_t ** TC_FRAME, int row, int col, int block_size);

void DCTBlock(int32_t** TC_FRAME, int32_t** RES_FRAME, int row, int col, int block_size) {

	int i, j, u, v;
	float alpha_u, alpha_v, temp_float;
	for (u = 0; u < block_size; ++u) {
		for (v = 0; v < block_size; ++v) {
			temp_float = 0.0;
			alpha_u = (u == 0) ? (1. / sqrt((float)block_size)) : (sqrt(2. / (float)block_size));
			alpha_v = (v == 0) ? (1. / sqrt((float)block_size)) : (sqrt(2. / (float)block_size));
			for (i = 0; i < block_size; i++) {
				for (j = 0; j < block_size; j++) {
					temp_float += ((float)RES_FRAME[row + i][col + j]) * cos(M_PI / ((float)block_size)*((2.*i + 1.) / 2.)*u)*cos(M_PI / ((float)block_size)*((2.*j + 1.) / 2.)*v);
				}
			}
			TC_FRAME[row + u][col + v] = alpha_u * alpha_v * temp_float;

		}
	}
}

void IDCTBlock(int32_t **RES_FRAME, int32_t ** TC_FRAME,int row, int col, int block_size) {
	int i, j, u, v;
	float alpha_u, alpha_v, temp_float;

	for (i = 0; i < block_size; ++i) {
		for (j = 0; j < block_size; ++j) {
			temp_float = 0.0;
			for (u = 0; u < block_size; ++u) {
				for (v = 0; v < block_size; ++v) {
					alpha_u = (u == 0) ? (1. / sqrt((float)block_size)) : (sqrt(2. / (float)block_size));
					alpha_v = (v == 0) ? (1. / sqrt((float)block_size)) : (sqrt(2. / (float)block_size));
					temp_float += alpha_u * alpha_v * ((float)TC_FRAME[row + u][col + v]) * cos(M_PI / ((float)block_size)*((2.*i + 1.) / 2.)*u)*cos(M_PI / ((float)block_size)*((2.*j + 1.) / 2.)*v);
				}
			}
			RES_FRAME[row + i][col + j] =  temp_float;
		}
	}
}

