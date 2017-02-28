#include <stdint.h>
#include <cmath>
#include <stdio.h> 
bool ValidQP(unsigned int QP, unsigned int block);

void GenerateQ_Matrix(unsigned char ** Q_Matrix, unsigned int block);

void GenerateQTC_Matrix(signed char** TC_Matrix, unsigned char ** Q_Matrix, unsigned int QP,unsigned char ** QTC_Matrix);
