#include <common.h>

void dct_frame_wrapper(int **DCTMatrix_frame, signed char **Matrix_FRAME, int width, int height, int block_size);
void dct(int **DCTMatrix, signed char **Matrix, int block_size);
void idct_frame_wrapper(signed char **Matrix_FRAME, int **DCTMatrix_frame, int width, int height, int block_size);
void idct(signed char **Matrix, int **DCTMatrix, int block_size);

void DCTBlock(int32_t** TC_FRAME, int8_t** RES_FRAME, int row, int col, int block_size);
void IDCTBlock(int8_t **RES_FRAME, int32_t ** TC_FRAME, int row, int col, int block_size);

void dct_frame_wrapper(int **DCTMatrix_frame, signed char **Matrix_FRAME, int width, int height, int block_size) {
	//create block matrices 
	int** TC_BLOCK = new int*[block_size];
	signed char** CUR_BLOCK = new signed char*[block_size];

	for (int row = 0; row < block_size; row++) {
		TC_BLOCK[row] = new signed int[block_size];
		CUR_BLOCK[row] = new signed char[block_size];
	}

	//Iterate block by block over the frame
	for (int row = 0; row < height; row += block_size) {
		for (int col = 0; col < width; col += block_size) {
			for (int i = 0; i < block_size; i++) {
				for (int j = 0; j < block_size; j++) {
					CUR_BLOCK[i][j] = Matrix_FRAME[row + i][col +j];
				}
			}
			//call DCT on block matrix
			dct(TC_BLOCK, CUR_BLOCK, block_size);
			//place 
			for (int i = 0; i < block_size; i++) {
				for (int j = 0; j < block_size; j++) {
					DCTMatrix_frame[row + i][col + j] = TC_BLOCK[i][j];
				}
			}
		}
	}
}

void idct_frame_wrapper(signed char **Matrix_FRAME, int **DCTMatrix_frame, int width, int height, int block_size) {
	//create block matrices 
	int** TC_BLOCK = new int*[block_size];
	signed char** CUR_BLOCK = new signed char*[block_size];

	for (int row = 0; row < block_size; row++) {
		TC_BLOCK[row] = new signed int[block_size];
		CUR_BLOCK[row] = new signed char[block_size];
	}

	//Iterate block by block over the frame
	for (int row = 0; row < height; row += block_size) {
		for (int col = 0; col < width; col += block_size) {
			for (int i = 0; i < block_size; i++) {
				for (int j = 0; j < block_size; j++) {
					TC_BLOCK[i][j] = DCTMatrix_frame[row + i][col + j];
				}
			}
			//call DCT on block matrix
			idct(CUR_BLOCK, TC_BLOCK, block_size);
			//place 
			for (int i = 0; i < block_size; i++) {
				for (int j = 0; j < block_size; j++) {
					Matrix_FRAME[row + i][col + j] = CUR_BLOCK[i][j];
				}
			}
		}
	}
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
			temp_float = 0.0;
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

void DCTBlock(int32_t** TC_FRAME, int8_t** RES_FRAME,int row, int col, int block_size) {

	int i, j, u, v;
	float alpha_u, alpha_v, temp_float, rounding_num;
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
			temp_float = alpha_u * alpha_v * temp_float;
			rounding_num = (temp_float > 0) ? (0.5) : (-0.5);
			TC_FRAME[row + u][col + v] = temp_float + rounding_num;

		}
	}
}

void IDCTBlock(int8_t **RES_FRAME, int32_t ** TC_FRAME,int row, int col, int block_size) {
	int i, j, u, v;
	float alpha_u, alpha_v, temp_float, rounding_num;

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
			rounding_num = (temp_float > 0.0) ? (0.5) : (-0.5);
			RES_FRAME[row + i][col + j] = temp_float + rounding_num;
		}
	}
}

