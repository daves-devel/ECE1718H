#define _USE_MATH_DEFINES
#include <cmath> 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <discrete_cosine_transform.h>

float **calloc_mat(int dimX, int dimY) {
	float **m = (float **)calloc(dimX, sizeof(float*));
	float *p = (float *)calloc(dimX*dimY, sizeof(float));
	int i;
	for (i = 0; i <dimX; i++) {
		m[i] = &p[i*dimY];

	}
	return m;
}

void free_mat(float **m) {
	free(m[0]);
	free(m);
}

void write_mat(FILE *fp, float **m, int N, int M) {

	int i, j;
	for (i = 0; i< N; i++) {
		fprintf(fp, "%f", m[i][0]);
		for (j = 1; j < M; j++) {
			fprintf(fp, ",%f", m[i][j]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
}

void dct(float **DCTMatrix, float **Matrix, int N, int M) {

	int i, j, u, v;
	float alpha_u, alpha_v;
	for (u = 0; u < N; ++u) {
		for (v = 0; v < M; ++v) {
			DCTMatrix[u][v] = 0;
			alpha_u = (u == 0) ? (1. / sqrt((float)N)):(sqrt(2. / N));
			alpha_v = (v == 0) ? (1. / sqrt((float)M)):(sqrt(2. / M));
			for (i = 0; i < N; i++) {
				for (j = 0; j < M; j++) {
					DCTMatrix[u][v] += Matrix[i][j] * cos(M_PI / ((float)N)*((2.*i + 1.) / 2.)*u)*cos(M_PI / ((float)M)*((2.*j + 1.) / 2.)*v);
				}
			}
			DCTMatrix[u][v] = alpha_u * alpha_v * DCTMatrix[u][v];
		}
	}
}

void idct(float **Matrix, float **DCTMatrix, int N, int M) {
	int i, j, u, v;
	float alpha_u, alpha_v;
	
	for (i = 0; i < N; ++i) {
		for (j = 0; j < M; ++j) {
			Matrix[i][j] = 0;
			for (u = 0; u < N; ++u) {
				for (v = 0; v < M; ++v) {
					alpha_u = (u == 0) ? (1. / sqrt((float)N)) : (sqrt(2. / N));
					alpha_v = (v == 0) ? (1. / sqrt((float)M)) : (sqrt(2. / M));
					Matrix[i][j] += alpha_u * alpha_v * DCTMatrix[u][v] * cos(M_PI / ((float)N)*((2.*i + 1.) / 2.)*u)*cos(M_PI / ((float)M)*((2.*j + 1.) / 2.)*v);
				}
			}
		}
	}
}
