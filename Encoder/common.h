#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>   
#include <stdlib.h> 
#include <stdint.h>
#include <algorithm>
#include <string.h>
#include "math.h"

enum FRAME {
	IFRAME = 0,
	PFRAME = 1
};

enum INTRAMODE {
	HORIZONTAL = 0,
	VERTICAL = 1
};
struct GMV {
	int X;
	int Y;
	unsigned int SAD;
	unsigned int NORM;
};
