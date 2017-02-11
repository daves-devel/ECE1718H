// Author:		Irfan Khan (khanirf1) 999207665
// Date:		February 5th, 2016
// Description: Given a Y only File, this program will pad it with grey based on block size used for encoding

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

unsigned int width		= 0;
unsigned int height		= 0;
unsigned int frames		= 0;
unsigned int range		= 0;
unsigned int block		= 0;
unsigned int padRight	= 0;
unsigned int padBottom	= 0;

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
	else if (!strcmp((*args) + 1, "block")) {
		args++;
		block = atoi(*args);
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
	if (block == 0) {
		printf("Invalid Block Dimension <%d>", block);
	}

	// Determine How Many Padded Pixels needed
	padRight	= ( width	% block ) ? ( block - ( width	% block ) ) : 0;
	padBottom	= ( height	% block ) ? ( block - ( height	% block ) ) : 0;

	unsigned char* Y_FRAME		= new unsigned char[ width * height ];
	unsigned char* PAD_RIGHT	= new unsigned char[ padRight ];
	unsigned char* PAD_BOTTOM	= new unsigned char[ width + padRight ];

	for (int i = 0; i < padRight; i++)
		PAD_RIGHT[i] = 128; // Prefill with GREY
	for (int i = 0; i < (width + padRight); i++)
		PAD_BOTTOM[i] = 128 ; // Prefill with GREY

	for (int frame = 0; frame < frames; frame++) {
			
		// Go to the beginning of an original frame
		fseek(infile, frame*width*height, SEEK_SET);

		for (int row = 0; row < height; row++) {
				
			fread(	Y_FRAME, sizeof(unsigned char), width, infile);
			fwrite(	Y_FRAME, sizeof(unsigned char), width, outfile);

			if (padRight > 0) { // Pad Right side of the Image with GREY=128
				fwrite(PAD_RIGHT, sizeof(unsigned char), padRight, outfile);
			}
		}

		for (int row = 0; row < padBottom; row++) { // Pad Bottom Rows of the Image with GREY=128
			fwrite(PAD_BOTTOM, sizeof(unsigned char), width + padRight, outfile);
		}
	}

	delete Y_FRAME;
	delete PAD_RIGHT;
	delete PAD_BOTTOM;
	fclose(infile);
	fclose(outfile);

	return 0;

}