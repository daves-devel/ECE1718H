#include <quantization.h>

bool ValidQP(unsigned int QP,unsigned int block) {

	unsigned int minQP = 0;
	unsigned int logI = (unsigned int) log2 ((double)block);
	unsigned int maxQP = logI + 7.0;

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
			else if (row + col == (block -1 )) {
				Q_Matrix[row][col] = 1 << (QP + 1);
			}
			else {
				Q_Matrix[row][col] = 1 << (QP + 2);
			}
		}
	}
}

void GenerateQTC_Matrix(signed char** TC_Matrix, unsigned char ** Q_Matrix, unsigned char ** QTC_Matrix, unsigned int block) {

	for (unsigned int row = 0; row < block; row++) {
		for (unsigned int col = 0; col < block; col++) {

			QTC_Matrix[row][col] = round(TC_Matrix[row][col] / Q_Matrix[row][col]);

		}
	}
}