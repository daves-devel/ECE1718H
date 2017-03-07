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

void free_mat(int **m) {
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

void dct(int **DCTMatrix, int **Matrix, int N, int M) {

	int i, j, u, v;
	float alpha_u, alpha_v, temp_float;
	for (u = 0; u < N; ++u) {
		for (v = 0; v < M; ++v) {
			temp_float = 0.0;
			alpha_u = (u == 0) ? (1. / sqrt((float)N)):(sqrt(2. / (float)N));
			alpha_v = (v == 0) ? (1. / sqrt((float)M)):(sqrt(2. / (float)M));
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
