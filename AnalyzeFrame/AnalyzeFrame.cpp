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

	char reffile_name[500];
	char decfile_name[500];
	char SADfile_name[500];
	char PSNRfile_name[500];

	int width = 0;
	int height = 0;
	int frames = 0;

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
		else if (!strcmp((*args) + 1, "reffile")) {
			args++;
			sscanf(*args, "%s", reffile_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "SAD")) {
			args++;
			sscanf(*args, "%s", SADfile_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "PSNR")) {
			args++;
			sscanf(*args, "%s", PSNRfile_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "decfile")) {
			args++;
			sscanf(*args, "%s", decfile_name);
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

	FILE* reffile = fopen(reffile_name, "rb");
	FILE* decfile = fopen(decfile_name, "rb");
	FILE* SADfile = fopen(SADfile_name, "w");
	FILE* PSNRfile = fopen(PSNRfile_name, "w");

	if (reffile == NULL) {
		printf("Cannot open input file <%s>\n", reffile_name);
		exit(-1);
	}
	if (decfile == NULL) {
		printf("Cannot open decoded file <%s>\n", decfile_name);
		exit(-1);
	}
	if (SADfile == NULL) {
		printf("Cannot open output file <%s>\n", SADfile_name);
		exit(-1);
	}

	unsigned int  FRAME_SIZE = width*height;
	unsigned char* REF_FRAME = new unsigned char[FRAME_SIZE];
	unsigned char* DEC_FRAME = new unsigned char[FRAME_SIZE];

	for (int frame = 0; frame < frames; frame++) {

		unsigned int SAD = 0;
		float MSE_SUM = 0.0;
		float MSE = 0.0;
		float PSNR = 0.0;
		float MAX_I = 255.0;

		fread(REF_FRAME, sizeof(unsigned char), FRAME_SIZE, reffile);
		fread(DEC_FRAME, sizeof(unsigned char), FRAME_SIZE, decfile);

		for (int pixel = 0; pixel < FRAME_SIZE; pixel++) {

			// SAD MATH
			SAD += abs(REF_FRAME[pixel] - DEC_FRAME[pixel]);

			// PSNR MATH
			MSE_SUM += pow(((float)REF_FRAME[pixel] - (float)DEC_FRAME[pixel]),2);
		}

		// PSNR FINAL TOUCHES
		MSE = MSE_SUM/ width/ height;
		PSNR = 20*log10(MAX_I) - 10 * log10(MSE);

		fprintf(SADfile,"%d,%d\n", frame+1, SAD); 
		fprintf(PSNRfile, "%d,%f\n", frame + 1, PSNR); 
	}

	delete REF_FRAME;
	delete DEC_FRAME;

	fclose(reffile);
	fclose(decfile);
	fclose(SADfile);
	fclose(PSNRfile);

	return 0;

}