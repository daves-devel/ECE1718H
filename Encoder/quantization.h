#include <common.h>

bool ValidQP(unsigned int QP, unsigned int block);
void GenerateQ_Matrix(unsigned char ** Q_Matrix, unsigned int QP, unsigned int block);
int Quantize(signed char** TC_Frame, unsigned char ** QTC_Frame, unsigned int QP, unsigned int block, unsigned int width, unsigned int height);
int Rescale(unsigned char** QTC_Frame, signed char ** TC_Frame, unsigned int QP, unsigned int block, unsigned int width, unsigned int height) {
