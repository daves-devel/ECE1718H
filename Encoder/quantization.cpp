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

char** GenerateQMatrix(unsigned int QP, unsigned int block) {


}