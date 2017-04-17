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

	char recfile_name[500];
	char decfile_name[500];

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
		else if (!strcmp((*args) + 1, "recfile")) {
			args++;
			sscanf(*args, "%s", recfile_name);
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

	FILE* reffile = fopen(recfile_name, "rb");
	FILE* decfile = fopen(decfile_name, "rb");

	if (reffile == NULL) {
		printf("Cannot open input file <%s>\n", recfile_name);
		exit(-1);
	}
	if (decfile == NULL) {
		printf("Cannot open output file <%s>\n", decfile_name);
		exit(-1);
	}

	unsigned int  FRAME_SIZE = width*height;
	unsigned char* REF_FRAME = new unsigned char[FRAME_SIZE];
	unsigned char* DEC_FRAME = new unsigned char[FRAME_SIZE];

	for (int frame = 0; frame < frames; frame++) {

		fread(REF_FRAME, sizeof(unsigned char), FRAME_SIZE, reffile);
		fread(DEC_FRAME, sizeof(unsigned char), FRAME_SIZE, decfile);

		for (int pixel = 0; pixel < FRAME_SIZE; pixel++) {
			if (REF_FRAME[pixel] != DEC_FRAME[pixel]) {
				printf("\n========================================\nERROR:Frame %d does not match on pixel %d\n========================================\n", frame + 1, pixel);
				exit(-1);
			}
		}
		
	}

	delete REF_FRAME;
	delete DEC_FRAME;

	fclose(reffile);
	fclose(decfile);

	return 0;

}