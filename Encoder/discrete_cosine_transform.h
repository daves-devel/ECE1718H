#include <common.h>

void dct_frame_wrapper(int **DCTMatrix_frame, signed char **Matrix_FRAME, int width, int height, int block_size);
void dct(int **DCTMatrix, signed char **Matrix, int block_size);
void idct_frame_wrapper(signed char **Matrix_FRAME, int **DCTMatrix_frame, int width, int height, int block_size);
void idct(signed char **Matrix, int **DCTMatrix, int block_size);

void dct_frame_wrapper(int **DCTMatrix_frame, signed char **Matrix_FRAME, int width, int height, int block_size) {

}

void idct_frame_wrapper(signed char **Matrix_FRAME, int **DCTMatrix_frame, int width, int height, int block_size) {

}


void dct(int **DCTMatrix, signed char **Matrix, int block_size) {

	int i, j, u, v;
	float alpha_u, alpha_v, temp_float, rounding_num;
	for (u = 0; u < block_size; ++u) {
		for (v = 0; v < block_size; ++v) {
			temp_float = 0.0;
			alpha_u = (u == 0) ? (1. / sqrt((float)block_size)) : (sqrt(2. / (float)block_size));
			alpha_v = (v == 0) ? (1. / sqrt((float)block_size)) : (sqrt(2. / (float)block_size));
			for (i = 0; i < block_size; i++) {
				for (j = 0; j < block_size; j++) {
					temp_float += ((float)Matrix[i][j]) * cos(M_PI / ((float)block_size)*((2.*i + 1.) / 2.)*u)*cos(M_PI / ((float)block_size)*((2.*j + 1.) / 2.)*v);
				}
			}
			temp_float = alpha_u * alpha_v * temp_float;
			rounding_num = (temp_float > 0) ? (0.5) : (-0.5);
			DCTMatrix[u][v] = temp_float + rounding_num;

		}
	}
}

void idct(signed char **Matrix, int **DCTMatrix, int block_size) {
	int i, j, u, v;
	float alpha_u, alpha_v, temp_float, rounding_num;

	for (i = 0; i < block_size; ++i) {
		for (j = 0; j < block_size; ++j) {
			temp_float = 0;
			for (u = 0; u < block_size; ++u) {
				for (v = 0; v < block_size; ++v) {
					alpha_u = (u == 0) ? (1. / sqrt((float)block_size)) : (sqrt(2. / (float)block_size));
					alpha_v = (v == 0) ? (1. / sqrt((float)block_size)) : (sqrt(2. / (float)block_size));
					temp_float += alpha_u * alpha_v * ((float)DCTMatrix[u][v]) * cos(M_PI / ((float)block_size)*((2.*i + 1.) / 2.)*u)*cos(M_PI / ((float)block_size)*((2.*j + 1.) / 2.)*v);
				}
			}
			rounding_num = (temp_float > 0.0) ? (0.5) : (-0.5);
			Matrix[i][j] = temp_float + rounding_num;
		}
	}
}

