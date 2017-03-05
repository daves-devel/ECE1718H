#ifndef __COMMON_H__
#define __COMMON_H__
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>   
#include <stdlib.h> 
#include <stdint.h>
#include <algorithm>
#include <string.h>
#include <math.h>
#define _USE_MATH_DEFINES

enum FRAME {
	IFRAME = 0,
	PFRAME = 1
};

enum INTRAMODE {
	HORIZONTAL = 0,
	VERTICAL = 1
};

struct MDIFF {
	int X; // For Inter GMV
	int Y; // For Inter GMV
	int MODE; // Intra Mode
	unsigned int SAD;
	unsigned int NORM;
};
#endif