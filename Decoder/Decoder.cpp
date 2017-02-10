// Decoder.cpp : Generates a decoded video file from the mv and residual files created during the encoding process

#include <residual.h>
#include <reconstructed.h>
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

	char decodedfile_name[500];
	char mvxfile_name[500];
	char mvyfile_name[500];
	char resfile_name[500];

	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int frames = 0;
	unsigned int range = 0;
	unsigned int block = 0;

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
		else if (!strcmp((*args) + 1, "decodedfile")) {
			args++;
			sscanf(*args, "%s", decodedfile_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "mvxfile")) {
			args++;
			sscanf(*args, "%s", mvxfile_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "mvyfile")) {
			args++;
			sscanf(*args, "%s", mvyfile_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "resfile")) {
			args++;
			sscanf(*args, "%s", resfile_name);
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
		else if (!strcmp((*args) + 1, "range")) {
			args++;
			range = atoi(*args);
			args++;
			tmpArgCnt += 2;
		}

		else {
			printf("Huh? I don't know %s (option #%d) \n", *args, tmpArgCnt);
			exit(-1);
		}
	}

	FILE* decodedfile = fopen(decodedfile_name, "wb");
	FILE* mvxfile = fopen(mvxfile_name, "rb");
	FILE* mvyfile = fopen(mvyfile_name, "rb");
	FILE* resfile = fopen(resfile_name, "rb");

	if (decodedfile == NULL) {
		printf("Cannot open input file <%s>\n", decodedfile_name);
		exit(-1);
	}
	if (mvxfile == NULL) {
		printf("Cannot open output file <%s>\n", mvxfile_name);
		exit(-1);
	}
	if (mvyfile == NULL) {
		printf("Cannot open output file <%s>\n", mvxfile_name);
		exit(-1);
	}
	if (resfile == NULL) {
		printf("Cannot open output file <%s>\n", resfile_name);
		exit(-1);
	}

	unsigned int  FRAME_SIZE = width*height;
	unsigned int index = 0;
	int MVX = 0;
	int MVY = 0;
	unsigned char* INTER_FRAME = new unsigned char[width * height];
	unsigned char*  DEC_FRAME = new unsigned char[width * height];
	signed char* RES_FRAME = new signed char[width * height];

	// This 2D Buffer Will containe the best blocks for 
	// estimation in their corresponding block locations
	/*unsigned char** INTER_FRAME = new unsigned char*[height];
	for (unsigned int row = 0; row < height; row++) {
		INTER_FRAME[row] = new unsigned char[width];
	}*/

	// Decode Each Frame
	for (unsigned int frame = 0; frame < frames; frame++) {

		if (frame == 0) {
			for (unsigned int i = 0; i < FRAME_SIZE; i++)
				DEC_FRAME[i] = 128; // Prefill with GREY
		}
		else {
			// Create a intermediate frame from the previous decoded frame and the motion vectors for the frame
			for (unsigned int i = 0; i < height; i++) {
				for (unsigned int j = 0; j < width; j++) {
					fread(&MVX, sizeof(int), 1, mvxfile);
					fread(&MVY, sizeof(int), 1, mvyfile);
					index = MVX + MVY + i + j;//current plus delta 
					INTER_FRAME[i + j] = DEC_FRAME[index];
				}
			}

			//Get residual frame
			fseek(resfile, frame*FRAME_SIZE, SEEK_SET);
			fread(RES_FRAME, sizeof(unsigned char), FRAME_SIZE, resfile);

			//Decoded frame = intermediate + residual 
			for (unsigned int i = 0; i < height; i++) {
				for (unsigned int j = 0; j < width; j++) {
					DEC_FRAME[i + j] = RES_FRAME[i + j];//INTER_FRAME[i + j] + RES_FRAME[i + j];
				}
			}
		}
		// Dump decoded frame
		fwrite(DEC_FRAME, sizeof(unsigned char), FRAME_SIZE, decodedfile);

	}

	delete INTER_FRAME;
	delete DEC_FRAME;
	delete RES_FRAME;
	fclose(decodedfile);
	fclose(mvxfile);
	fclose(mvyfile);
	fclose(resfile);

	return 0;

}



