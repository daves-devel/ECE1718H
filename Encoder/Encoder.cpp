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
	int X;
	int Y;
	unsigned int SAD;
	unsigned int NORM;
};

struct GMV MotionEstimate(unsigned int row, unsigned int col, unsigned int width, unsigned int height, 
							unsigned int block, int range, unsigned char* CUR_FRAME, unsigned char* REC_FRAME);

int main(int argCnt, char **args)
{

	char curfile_name[500];
	char mvfile_name[500];
	char resfile_name[500];
	char recfile_name[500];
	char gmvx_name[500];
	char gmvy_name[500];

	unsigned int width		= 0;
	unsigned int height		= 0;
	unsigned int frames		= 0;
	int range		= 0;
	unsigned int block		= 0;
	unsigned int padRight	= 0;
	unsigned int padBottom	= 0;
	unsigned int round		= 0;

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
		else if (!strcmp((*args) + 1, "round")) {
			args++;
			round = atoi(*args);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "round")) {
			args++;
			round = atoi(*args);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "gmvx")) {
			args++;
			sscanf(*args, "%s", gmvx_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "gmvy")) {
			args++;
			sscanf(*args, "%s", gmvy_name);
			args++;
			tmpArgCnt += 2;
		}

		else {
			printf("Huh? I don't know %s (option #%d) \n", *args, tmpArgCnt);
			exit(-1);
		}
	}

	FILE* curfile = fopen(curfile_name, "rb");
	FILE* mvfile = fopen(mvfile_name, "w");
	FILE* resfile = fopen(resfile_name, "wb");
	FILE* recfile = fopen(recfile_name, "w+b");
	FILE* gmvXfile = fopen(gmvx_name, "wb");
	FILE* gmvYfile = fopen(gmvy_name, "wb");

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
	signed char* RES_FRAME = new signed char[width * height];

	// This 2D Buffer Will containe the best blocks for 
	// estimation in their corresponding block locations
	unsigned char** MOTION_FRAME = new unsigned char*[height];
	for (unsigned int row = 0; row < height; row++) {
		MOTION_FRAME[row] = new unsigned char[width];
	}
	
	// This 1D Buffer Will Contain the GMV for each block
	// in raster row order
	struct GMV* GMV_VECTOR = new struct GMV[(width/block)*(height/block)];


	// Encode Each Frame
	for (unsigned int frame = 0; frame < frames; frame++) {
		fprintf(mvfile, "Frame %d\n", frame);
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
		for (unsigned int row = 0; row < height; row += block) {
			for (unsigned int col = 0; col < width; col += block) {

				// IDEALLY THREAD EVERYTHING IN THIS FOR LOOP

				// Motion estimate for block x = row/block, y = col/block					
				// Store the result GMV in its spot in 1D array 
				GMV_VECTOR[((row*width/block) / block) + (col / block)] = MotionEstimate(row, col, width, height, block, range, CUR_FRAME, REC_FRAME);

				int GMV_X = GMV_VECTOR[((row*width/block) / block) + (col / block)].X;
				int GMV_Y = GMV_VECTOR[((row*width/block) / block) + (col / block)].Y;

				fwrite(&GMV_X, sizeof(int), 1, gmvXfile);
				fwrite(&GMV_Y, sizeof(int), 1, gmvYfile);
				fprintf(mvfile, "Block_size %d Block_X %d Block_Y %d GVM_X %d GMV_Y %d\n", block, col / block, row / block, GMV_X, GMV_Y);
				// Fill the Motion Frame with the best matching block
				for (unsigned int i = 0; i < block; i++) {
					for (unsigned int j = 0; j < block; j++) {
						MOTION_FRAME[row + i][col + j] = REC_FRAME[(row + GMV_Y + i) * width + (col + GMV_X + j)];
					}
				}
			}
		}

		// MV FILE GENERATION
		// ===================
		// Dump GMV 1D array to mvfile					-> Keep 1D array buffer alive in mem still

		// RESIDUAL FILE GENERATION
		// =========================
		// Use motion frame to create residual frame	-> Keep Motion Frame buffer alive in mem still 
		residual(RES_FRAME, CUR_FRAME, block, width, height, round, MOTION_FRAME);
		// Dump residual frame to file resfile			-> Keep Resisdual Frame buffer alive in mem still
		fwrite(RES_FRAME, sizeof(unsigned char), FRAME_SIZE, resfile);
		// RECONSTRUCTED FILE GENERATION
		// ==============================
		// Use motion frame								-> Decoder will do this too so it should be valid
		// and the residual frame and the GMVs			-> all this stuff should still be alive in mem.
		recon(RES_FRAME, REC_FRAME, block, width, height, MOTION_FRAME); //TODO add motion estimation value
		fclose(recfile);
		recfile = fopen(recfile_name, "a+b");
		fwrite(REC_FRAME, sizeof(unsigned char), FRAME_SIZE, recfile);
		// Dump Reconstructed 
		fprintf(mvfile, "End of frame %d\n", frame);
	}

	delete CUR_FRAME;
	delete REC_FRAME;
	fclose(curfile);
	fclose(mvfile);
	fclose(resfile);
	fclose(recfile);

	return 0;

}

struct GMV MotionEstimate(unsigned int row, // Pixel Row in Current Frame
	unsigned int col, // Pixel Col in Current Frame
	unsigned int width,
	unsigned int height,
	unsigned int block,
	int range,
	unsigned char* CUR_FRAME,
	unsigned char* REC_FRAME) {

	struct GMV BEST_GMV;
	bool firstGMV = true;

	for (int GMV_X = 0 - range; GMV_X <= range; GMV_X++) {
		for (int GMV_Y = 0 - range; GMV_Y <= range; GMV_Y++) {
			if (((GMV_X + col) < 0) || ((GMV_X + col) >= width)) {
				continue; //Block outisde search space so don't compute
			}
			if (((GMV_Y + row) < 0) || ((GMV_Y + row) >= height)) {
				continue; //Block outisde search space so don't compute
			}

			int X = GMV_X;
			int Y = GMV_Y;
			unsigned int SAD = 0;
			unsigned int NORM = abs(X) + abs(Y);

			//Calculate SAD
			for (int i = 0; i < block; i++) {
				for (int j = 0; j < block; j++) {
					SAD += abs(CUR_FRAME[(X + row + i)*width + Y + col + j] - REC_FRAME[(X + row + i)*width + Y + col + j]);
				}
			}

			// Pick Best Global Motion Vector 
			if (firstGMV) {
				firstGMV = false;
				BEST_GMV.X = X;
				BEST_GMV.Y = Y;
				BEST_GMV.SAD = SAD;
				BEST_GMV.NORM = NORM;
				continue;
			}

			if (BEST_GMV.SAD > SAD) {
				BEST_GMV.X = X;
				BEST_GMV.Y = Y;
				BEST_GMV.SAD = SAD;
				BEST_GMV.NORM = NORM;
			}
			else if (BEST_GMV.SAD == SAD) {
				if (BEST_GMV.NORM > NORM) {
					BEST_GMV.X = X;
					BEST_GMV.Y = Y;
					BEST_GMV.SAD = SAD;
					BEST_GMV.NORM = NORM;
				}
				else if (BEST_GMV.NORM == NORM) {
					if (abs(BEST_GMV.Y) > abs(Y)) {
						BEST_GMV.X = X;
						BEST_GMV.Y = Y;
						BEST_GMV.SAD = SAD;
						BEST_GMV.NORM = NORM;
					}
					else if (abs(BEST_GMV.Y) == abs(Y)) {
						if (abs(BEST_GMV.X) > abs(X)) {
							BEST_GMV.X = X;
							BEST_GMV.Y = Y;
							BEST_GMV.SAD = SAD;
							BEST_GMV.NORM = NORM;
						}
						else if (abs(BEST_GMV.X) == abs(X)) {
							// Either Do Nothing
							// Or we could Overwrite
						}
					}
				}
			}
		}
	}
	return BEST_GMV;
}