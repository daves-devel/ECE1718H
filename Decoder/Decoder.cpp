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
	char mvfile_name[500];
	char resfile_name[500];
	char recfile_name[500];

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
		else if (!strcmp((*args) + 1, "mvfile")) {
			args++;
			sscanf(*args, "%s", mvfile_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "resfile")) {
			args++;
			sscanf(*args, "%s", resfile_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "recfile")) {
			args++;
			sscanf(*args, "%s", recfile_name);
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
	FILE* mvfile = fopen(mvfile_name, "rb");
	FILE* resfile = fopen(resfile_name, "rb");

	if (decodedfile == NULL) {
		printf("Cannot open input file <%s>\n", decodedfile_name);
		exit(-1);
	}
	if (mvfile == NULL) {
		printf("Cannot open output file <%s>\n", mvfile_name);
		exit(-1);
	}
	if (resfile == NULL) {
		printf("Cannot open output file <%s>\n", resfile_name);
		exit(-1);
	}
	if (recfile == NULL) {
		printf("Cannot open output file <%s>\n", recfile_name);
		exit(-1);
	}
	if (block == 0) {
		printf("Invalid Block Dimension <%d>", block);
	}

	unsigned int  FRAME_SIZE = width*height;
	unsigned char* CUR_FRAME = new unsigned char[width * height];
	unsigned char* INTERMEDIATE_FRAME = new unsigned char[width * height];
	unsigned char* REC_FRAME_OUT = new unsigned char[width * height];
	unsigned char* RES_FRAME = new unsigned char[width * height];


	// This 2D Buffer Will containe the best blocks for 
	// estimation in their corresponding block locations
	unsigned char** MOTION_FRAME = new unsigned char*[height];
	for (unsigned int row = 0; row < height; row++) {
		MOTION_FRAME[row] = new unsigned char[width];
	}

	// Decode Each Frame
	for (unsigned int frame = 0; frame < frames; frame++) {

		if (frame == 0) {
			for (unsigned int i = 0; i < FRAME_SIZE; i++)
				INTERMEDIATE_FRAME[i] = 128; // Prefill with GREY
		}
		else {
			// Create a intermediate frame from previous 
			fseek(recfile, (frame - 1)*FRAME_SIZE, SEEK_SET);
			fread(REC_FRAME, sizeof(unsigned char), FRAME_SIZE, recfile);
		}

		// Go to the beginning of the current frame and copy it to buffer
		fseek(curfile, frame*FRAME_SIZE, SEEK_SET);
		fread(CUR_FRAME, sizeof(unsigned char), FRAME_SIZE, curfile);
		for (unsigned int row = 0; row < height; row += block) {
			for (unsigned int col = 0; col < width; col += block) {


			}

		}

		// MV FILE GENERATION
		// ===================
		// Dump GMV 1D array to mvfile					-> Keep 1D array buffer alive in mem still

		// RESIDUAL FILE GENERATION
		// =========================
		// Use motion frame to create residual frame	-> Keep Motion Frame buffer alive in mem still 
		residual(RES_FRAME, CUR_FRAME, block, width, height, 2);//TODO substract motion estimation value and add N
																// Dump residual frame to file resfile			-> Keep Resisdual Frame buffer alive in mem still
		fwrite(RES_FRAME, sizeof(unsigned char), FRAME_SIZE, resfile);
		// RECONSTRUCTED FILE GENERATION
		// ==============================
		// Use motion frame								-> Decoder will do this too so it should be valid
		// and the residual frame and the GMVs			-> all this stuff should still be alive in mem.
		recon(RES_FRAME, REC_FRAME, block, width, height); //TODO add motion estimation value
		fclose(recfile);
		recfile = fopen(recfile_name, "a+b");
		fwrite(REC_FRAME, sizeof(unsigned char), FRAME_SIZE, recfile);
		// Dump Reconstructed 

	}

	delete CUR_FRAME;
	delete REC_FRAME;
	fclose(decodedfile);
	fclose(mvfile);
	fclose(resfile);
	fclose(recfile);

	return 0;

}



