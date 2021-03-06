#define _USE_MATH_DEFINES
#include <cmath> 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <discrete_cosine_transform.h>

int **calloc_mat(int dimX, int dimY) {
	int **m = (int **)calloc(dimX, sizeof(int*));
	int *p = (int *)calloc(dimX*dimY, sizeof(int));
	int i;
	for (i = 0; i <dimX; i++) {
		m[i] = &p[i*dimY];

	}
	return m;
}

signed char **calloc_mat2(int dimX, int dimY) {
	signed char  **m = (signed char **)calloc(dimX, sizeof(signed char*));
	signed char *p = (signed char *)calloc(dimX*dimY, sizeof(signed char));
	int i;
	for (i = 0; i <dimX; i++) {
		m[i] = &p[i*dimY];

	}
	return m;
}

void free_mat(int **m) {
	free(m[0]);
	free(m);
}

void free_mat2(signed char **m) {
	free(m[0]);
	free(m);
}

void write_mat(FILE *fp, int **m, int N, int M) {

	int i, j;
	for (i = 0; i< N; i++) {
		fprintf(fp, "%d", m[i][0]);
		for (j = 1; j < M; j++) {
			fprintf(fp, ",%d", m[i][j]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
}

void write_mat2(FILE *fp, signed char **m, int N, int M) {

	int i, j;
	for (i = 0; i< N; i++) {
		fprintf(fp, "%d", m[i][0]);
		for (j = 1; j < M; j++) {
			fprintf(fp, ",%d", m[i][j]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
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
					CUR_BLOCK[i][j] = Matrix_FRAME[row + i][col + j];
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