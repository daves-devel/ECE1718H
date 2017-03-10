#include <common.h>

bool ValidQP(uint32_t  QP, uint32_t  block);
void GenerateQ_Matrix(uint32_t  ** Q_Matrix, uint32_t  QP, uint32_t  block);
int QuantizeBlock(int32_t  ** QTC_FRAME, int32_t ** TC_FRAME, uint8_t** QP_FRAME,uint32_t row, uint32_t col, uint32_t  width, uint32_t  height, uint32_t  QP, uint32_t  block);
int ScaleBlock(int32_t  ** TC_FRAME, int32_t ** QTC_FRAME, uint8_t** QP_FRAME, uint32_t row, uint32_t col, uint32_t  width, uint32_t  height, uint32_t  QP, uint32_t  block);

bool ValidQP(uint32_t  QP, uint32_t  block) {

	uint32_t  minQP = 0;
	uint32_t  logI = (uint32_t )log2((double)block);
	uint32_t  maxQP = logI + 7;

	if ((QP >= minQP) && (QP <= maxQP)) {
		return true;
	}

	else {
		printf("Error, QP %d is not within Range [%d,%d]\n", QP, minQP, maxQP);
		return false;
	}

}

void GenerateQ_Matrix(uint32_t  ** Q_Matrix, uint32_t  QP, uint32_t  block) {

	for (uint32_t  i = 0; i < block; i++) {
		for (uint32_t  j = 0; j < block; j++) {

			if (i + j < (block - 1)) {
				Q_Matrix[i][j] = 1 << QP;
			}
			else if (i + j == (block - 1)) {
				Q_Matrix[i][j] = 1 << (QP + 1);
			}
			else {
				Q_Matrix[i][j] = 1 << (QP + 2);
			}
		}
	}
}

int QuantizeBlock(int32_t  ** QTC_FRAME, int32_t ** TC_FRAME, uint8_t ** QP_FRAME, uint32_t row, uint32_t col, uint32_t  width, uint32_t  height, uint32_t  QP, uint32_t  block) {

	if (!ValidQP(QP, block)) {
		return 1;
	}

	for (uint32_t i = 0; i < block; i++) {
		for (uint32_t j = 0; j < block; j++) {

			if (i + j < (block - 1)) {
				QP_FRAME[row + i][col + j] = 1 << QP;
			}
			else if (i + j == (block - 1)) {
				QP_FRAME[row + i][col + j] = 1 << (QP + 1);
			}
			else {
				QP_FRAME[row + i][col + j] = 1 << (QP + 2);
			}
		}
	}

	for (uint32_t  i = 0; i < block; i++) {
		for (uint32_t  j = 0; j < block; j++) {

			QTC_FRAME[row + i][col + j] = round(TC_FRAME[row + i][col + j] / QP_FRAME[row + i][col + j]);

		}
	}

}

int ScaleBlock(int32_t  ** TC_FRAME, int32_t ** QTC_FRAME, uint8_t** QP_FRAME,uint32_t row, uint32_t col,uint32_t  width, uint32_t  height, uint32_t  QP, uint32_t  block) {

	if (!ValidQP(QP, block)) {
		return 1;
	}

	for (uint32_t  i = 0; i < block; i++) {
		for (uint32_t  j = 0; j < block; j++) {

			TC_FRAME[row + i][col + j] = QTC_FRAME[row + i][col + j] * QP_FRAME[row + i][col + j];

		}
	}
}