// Author:		Irfan Khan (khanirf1) 999207665
// Date:		February 6th, 2016
// Description: Given a Y only File, this will Encode the Video.
//It will produce a Motion vector file and a Residual Video

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


struct GMV {
	unsigned char x;
	unsigned char y;
};

struct GMV MotionEstimate(unsigned int row,	unsigned int col, unsigned int width, unsigned int height, unsigned int block, unsigned int range, unsigned char* CUR_FRAME, unsigned char* REC_FRAME);
void FILL_MOTION_BLOCK(struct GMV BEST_GMV, unsigned char** MOTION_FRAME);
void GMV_COMPARE(unsigned int X, unsigned int Y, unsigned int SADD);

int main(int argCnt, char **args)
{

	char curfile_name[500];
	char mvfile_name[500];
	char resfile_name[500];
	char recfile_name[500];

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
		else if (!strcmp((*args) + 1, "curfile")) {
			args++;
			sscanf(*args, "%s", curfile_name);
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

	FILE* curfile = fopen(curfile_name, "rb");
	FILE* mvfile = fopen(mvfile_name, "wb");
	FILE* resfile = fopen(resfile_name, "wb");
	FILE* recfile = fopen(recfile_name, "w+b");

	if (curfile == NULL) {
		printf("Cannot open input file <%s>\n", curfile_name);
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
	unsigned char* REC_FRAME = new unsigned char[width * height];
	unsigned char* REC_FRAME_OUT = new unsigned char[width * height];
	unsigned char* RES_FRAME = new unsigned char[width * height];

	// This 2D Buffer Will containe the best blocks for 
	// estimation in their corresponding block locations
	unsigned char** MOTION_FRAME = new unsigned char*[height];
	for (unsigned int row = 0; row < height; row++) {
		MOTION_FRAME[row] = new unsigned char[width];
	}
	
	// This 1D Buffer Will Contain the GMV for each block
	// in raster row order
	struct GMV* GMV_VECTOR = new struct GMV[width/block*height/block];


	// Encode Each Frame
	for (unsigned int frame = 0; frame < frames; frame++) {

		if (frame == 0) {
			for (unsigned int i = 0; i < FRAME_SIZE; i++)
				REC_FRAME[i] = 128; // Prefill with GREY
		}
		else {
			// Go to the beginning of the previous reconstructed frame and copy it to buffer
			fseek(recfile, (frame - 1)*FRAME_SIZE, SEEK_SET);
			fread(REC_FRAME, sizeof(unsigned char), FRAME_SIZE, recfile);
		}

		// Go to the beginning of the current frame and copy it to buffer
		fseek(curfile, frame*FRAME_SIZE, SEEK_SET);
		fread(CUR_FRAME, sizeof(unsigned char), FRAME_SIZE, curfile);
		for (unsigned int row = 0; row < height; row+=block) {
			for (unsigned int col = 0; col < width; col+=block){

				// IDEALLY THREAD EVERYTHING IN THIS FOR LOOP
				
				// Motion estimate for block x = row/block, y = col/block					
				// Store the result GMV in its spot in 1D array 
				//GMV_VECTOR[row*width/block + col/block] = MotionEstimate(row,col, width, height, block, range, CUR_FRAME, REC_FRAME);

				// Fill the Motion Frame with the best matching block
				//FILL_MOTION_BLOCK(GMV_VECTOR[row*width / block + col / block], MOTION_FRAME);

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
	fclose(curfile);
	fclose(mvfile);
	fclose(resfile);
	fclose(recfile);

	return 0;

}

struct GMV MotionEstimate(	unsigned int row, // Pixel Row in Current Frame
							unsigned int col, // Pixel Col in Current Frame
							unsigned int width, 
							unsigned int height, 
							unsigned int block, 
							unsigned int range, 
							unsigned char* CUR_FRAME, 
							unsigned char* REC_FRAME) {

	struct GMV BEST_GMV;

	/*for (int x_range = -range; x_range <= range; range++) {
		for (int y_range = -range; y_range <= range; range++) {
			if (((x_range + row) < 0) || ((x_range + row) > width)){
				continue; //Block outisde search space
			}
			if ((y_range + col))
		}
	}*/
	/*
	Search 2D array space

	Nested for loop {

	Calculate SAD and norm

	IF: New GMV is better than current GMV
	Overwrite GMV
	Else:
	Go to Next block
	}
	Return GVM
	}*/
	return BEST_GMV;

}

void FILL_MOTION_BLOCK(struct GMV BEST_GMV, unsigned char** MOTION_FRAME) {


}

void GMV_COMPARE(unsigned int X, unsigned int Y, unsigned int SADD) {

}
