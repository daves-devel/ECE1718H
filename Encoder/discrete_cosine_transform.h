#include <common.h>

void dct(int **DCTMatrix, int **Matrix, int N, int M);
void idct(int **Matrix, int **DCTMatrix, int N, int M);

void dct(int **DCTMatrix, int **Matrix, int N, int M) {

	int i, j, u, v;
	float alpha_u, alpha_v, temp_float;
	for (u = 0; u < N; ++u) {
		for (v = 0; v < M; ++v) {
			temp_float = 0.0;
			alpha_u = (u == 0) ? (1. / sqrt((float)N)) : (sqrt(2. / (float)N));
			alpha_v = (v == 0) ? (1. / sqrt((float)M)) : (sqrt(2. / (float)M));
			for (i = 0; i < N; i++) {
				for (j = 0; j < M; j++) {
					temp_float += ((float)Matrix[i][j]) * cos(M_PI / ((float)N)*((2.*i + 1.) / 2.)*u)*cos(M_PI / ((float)M)*((2.*j + 1.) / 2.)*v);
				}
			}
			DCTMatrix[u][v] = alpha_u * alpha_v * temp_float;
		}
	}
}

void idct(int **Matrix, int **DCTMatrix, int N, int M) {
	int i, j, u, v;
	float alpha_u, alpha_v, temp_float;

	for (i = 0; i < N; ++i) {
		for (j = 0; j < M; ++j) {
			temp_float = 0;
			for (u = 0; u < N; ++u) {
				for (v = 0; v < M; ++v) {
					alpha_u = (u == 0) ? (1. / sqrt((float)N)) : (sqrt(2. / (float)N));
					alpha_v = (v == 0) ? (1. / sqrt((float)M)) : (sqrt(2. / (float)M));
					temp_float += alpha_u * alpha_v * ((float)DCTMatrix[u][v]) * cos(M_PI / ((float)N)*((2.*i + 1.) / 2.)*u)*cos(M_PI / ((float)M)*((2.*j + 1.) / 2.)*v);
				}
			}
			Matrix[i][j] = temp_float;
		}
	}
}

