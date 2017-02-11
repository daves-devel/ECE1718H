// Author:		Irfan Khan (khanirf1) 999207665
// Date:		February 5th, 2016
// Description: Given a YUV file, this program will extract the Luma Components and produce a Y only file.

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>   
#include <stdlib.h> 
#include <algorithm>
#include <string.h>
#include "math.h"


int main(int argCnt, char **args)
{

	char filein_name[500];
	char fileout_name[500];

	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int frames = 0;

	args++;
	int tmpArgCnt = 1;

	//  Parse Input Arguments
	// =======================
	while (tmpArgCnt < argCnt && (*args)[0] == '-') {

		if (!strcmp((*args) + 1, "width")) {
			args++;
			width = atoi(*args);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "height")) {
			args++;
			height = atoi(*args);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "filein")) {
			args++;
			sscanf(*args, "%s", filein_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "fileout")) {
			args++;
			sscanf(*args, "%s", fileout_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "frames")) {
			args++;
			frames = atoi(*args);
			args++;
			tmpArgCnt += 2;
		}
		else {
			printf("Huh? I don't know %s (option #%d) \n", *args, tmpArgCnt);
			exit(-1);
		}

	}

	FILE* infile = fopen(filein_name, "rb");
	FILE* outfile = fopen(fileout_name, "wb");

	if (infile == NULL) {
		printf("Cannot open input file <%s>\n", filein_name);
		exit(-1);
	}
	if (outfile == NULL) {
		printf("Cannot open output file <%s>\n", fileout_name);
		exit(-1);
	}

	unsigned char* Y_FRAME = new unsigned char[width*height];

	for (int frame = 0; frame < frames; frame++) {
		fseek(infile,frame*width*height * 3/2, SEEK_SET);
		fread(Y_FRAME, sizeof(unsigned char), width*height, infile);
		fwrite(Y_FRAME, sizeof(unsigned char), width*height, outfile);
	}

	delete Y_FRAME;
	fclose(infile);
	fclose(outfile);
		
	return 0;
}

