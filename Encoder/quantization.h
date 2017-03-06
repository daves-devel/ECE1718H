#include <common.h>

bool ValidQP(unsigned int QP, unsigned int block);
void GenerateQ_Matrix(unsigned char ** Q_Matrix, unsigned int QP, unsigned int block);
int Quantize(signed char** TC_Frame, signed char ** QTC_Frame, unsigned int QP, unsigned int block, unsigned int width, unsigned int height);
int Rescale(signed char** QTC_Frame, signed char ** TC_Frame, unsigned int QP, unsigned int block, unsigned int width, unsigned int height);

bool ValidQP(unsigned int QP, unsigned int block) {

	unsigned int minQP = 0;
	unsigned int logI = (unsigned int)log2((double)block);
	unsigned int maxQP = logI + 7;

	if ((QP >= minQP) && (QP <= maxQP)) {
		return true;
	}

	else {
		printf("Error, QP %d is not within Range [%d,%d]\n", QP, minQP, maxQP);
		return false;
	}

}

void GenerateQ_Matrix(unsigned char ** Q_Matrix, unsigned int QP, unsigned int block) {

	for (unsigned int row = 0; row < block; row++) {
		for (unsigned int col = 0; col < block; col++) {

			if (row + col < (block - 1)) {
				Q_Matrix[row][col] = 1 << QP;
			}
			else if (row + col == (block - 1)) {
				Q_Matrix[row][col] = 1 << (QP + 1);
			}
			else {
				Q_Matrix[row][col] = 1 << (QP + 2);
			}
		}
	}
}

int Quantize(signed char** TC_Frame, signed char ** QTC_Frame, unsigned int QP, unsigned int block, unsigned int width, unsigned int height) {

	if (!ValidQP(QP, block)) {
		return 1;
	}

	unsigned char** Q_Matrix = new unsigned char*[height];
	for (unsigned int row = 0; row < height; row++) {
		Q_Matrix[row] = new unsigned char[width];
	}

	GenerateQ_Matrix(Q_Matrix, QP, block);

	for (unsigned int row = 0; row < block; row++) {
		for (unsigned int col = 0; col < block; col++) {

			QTC_Frame[row][col] = round(TC_Frame[row][col] / Q_Matrix[row % block][col % block]);

		}
	}

}

int Rescale(signed char** QTC_Frame, signed char ** TC_Frame, unsigned int QP, unsigned int block, unsigned int width, unsigned int height) {

	if (!ValidQP(QP, block)) {
		return 1;
	}

	unsigned char** Q_Matrix = new unsigned char*[height];
	for (unsigned int row = 0; row < height; row++) {
		Q_Matrix[row] = new unsigned char[width];
	}

	GenerateQ_Matrix(Q_Matrix, QP, block);

	for (unsigned int row = 0; row < block; row++) {
		for (unsigned int col = 0; col < block; col++) {

			TC_Frame[row][col] = QTC_Frame[row][col] * Q_Matrix[row % block][col % block];

		}
	}
}