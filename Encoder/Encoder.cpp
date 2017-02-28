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

struct GMV MotionEstimate(int row, int col, int width, int height, 
						 int block, int range, unsigned char* CUR_FRAME, unsigned char* REC_FRAME);

int main(int argCnt, char **args)
{

	char curfile_name[500];
	char mvfile_name[500];
	char resfile_name[500];
	char recfile_name[500];
	char motionfile_name[500];
	char gmvx_name[500];
	char gmvy_name[500];
	
	int width		= 0;
	int height		= 0;
	int frames		= 0;
	int range		= 0;
	int block		= 0;
	int padRight	= 0;
	int padBottom	= 0;
	int round		= 0;

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
		else if (!strcmp((*args) + 1, "motionfile")) {
			args++;
			sscanf(*args, "%s", motionfile_name);
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
	FILE* motionfile = fopen(motionfile_name, "wb");
	FILE* gmvXfile = fopen(gmvx_name, "wb");
	FILE* gmvYfile = fopen(gmvy_name, "wb");
	//FILE* debug_inter_frame = fopen("C:\\Users\\JuanFuentes\\Desktop\\test\\inter_frame_enc.txt", "w");DEBUG


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

	unsigned int  FRAME_SIZE		= width*height;
	unsigned char* CUR_FRAME		= new unsigned char[FRAME_SIZE];
	unsigned char* REC_FRAME		= new unsigned char[FRAME_SIZE];
	unsigned char* REC_FRAME_OUT	= new unsigned char[FRAME_SIZE];
	unsigned char* RES_FRAME	    = new unsigned char[FRAME_SIZE];

	// This 2D Buffer Will containe the best blocks for 
	// estimation in their corresponding block locations
	unsigned char** MOTION_FRAME = new unsigned char*[height];
	for (unsigned int row = 0; row < height; row++) {
		MOTION_FRAME[row] = new unsigned char[width];
	}
	
	// This 1D Buffer will Contain the GMV for each block
	// in raster row order
	struct GMV* GMV_VECTOR = new struct GMV[(width/block)*(height/block)];

	// Encode Each Frame
	for (int frame = 0; frame < frames; frame++) {
		// MV FILE GENERATION (BEGINNING of FRAME)
		// =========================================
		//fprintf(debug_inter_frame, "Frame %d\n",  frame);DEBUG
		fprintf(mvfile, "Frame %d Block_size %d \n", frame + 1, block);
		if (frame == 0) {
			for (int i = 0; i < FRAME_SIZE; i++)
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
		for (int row = 0; row < height; row += block) {
			for (int col = 0; col < width; col += block) {
				// IDEALLY THREAD EVERYTHING IN THIS FOR LOOP
				// Motion estimate for block x = row/block, y = col/block					
				// Store the result GMV in its spot in 1D array 
				GMV_VECTOR[((row*width/block) / block) + (col / block)] = MotionEstimate(row, col, width, height, block, range, CUR_FRAME, REC_FRAME);

				int GMV_X = GMV_VECTOR[((row*width/block) / block) + (col / block)].X;
				int GMV_Y = GMV_VECTOR[((row*width/block) / block) + (col / block)].Y;

				// MV FILE GENERATION (VECTOR DUMP)
				// =======================================
				fwrite(&GMV_X, sizeof(int), 1, gmvXfile);
				fwrite(&GMV_Y, sizeof(int), 1, gmvYfile);
				fprintf(mvfile, "B(%d,%d)_V(%d,%d)\n", row / block, col / block, GMV_X, GMV_Y);

				// Fill the Motion Frame with the best matching block
				for (int i = 0; i < block; i++) {
					for (int j = 0; j < block; j++) {
						MOTION_FRAME[row + i][col + j] = REC_FRAME[(row + GMV_Y + i) * width + (col + GMV_X + j)];
						unsigned int temp = REC_FRAME[(row + GMV_Y + i) * width + (col + GMV_X + j)];
					}
				}
			}
		}

		
		for (unsigned int row = 0; row < height; row++) {
			fwrite(MOTION_FRAME[row], sizeof(unsigned char), width, motionfile);
		}
		fclose(motionfile);
		motionfile = fopen(motionfile_name, "a+b");
		//fwrite(MOTION_FRAME, sizeof(unsigned char), width, motionfile);
		// RESIDUAL FILE GENERATION
		// =========================================================================
		residual(RES_FRAME, CUR_FRAME, block, width, height, round, MOTION_FRAME);
		fwrite(RES_FRAME, sizeof(unsigned char), FRAME_SIZE, resfile);

		// RECONSTRUCTED FILE GENERATION
		// ==============================
		recon(RES_FRAME, REC_FRAME, block, width, height, MOTION_FRAME);
		fclose(recfile);
		recfile = fopen(recfile_name, "a+b");
		fwrite(REC_FRAME, sizeof(unsigned char), FRAME_SIZE, recfile);

		// MV FILE GENERATION (END of FRAME)
		// =======================================
		fprintf(mvfile, "End of frame %d\n\n", frame+1);
	}

	for (unsigned int row = 0; row < height; row++) {
		delete MOTION_FRAME[row];
	}
	delete MOTION_FRAME;
	delete CUR_FRAME;
	delete REC_FRAME;
	fclose(curfile);
	fclose(mvfile);
	fclose(gmvXfile);
	fclose(gmvYfile);
	fclose(resfile);
	fclose(recfile);
	fclose(motionfile);
	return 0;

}

struct GMV MotionEstimate(int row, // Pixel Row in Current Frame
	int col, // Pixel Col in Current Frame
	int width,
	int height,
	int block,
	int range,
	unsigned char* CUR_FRAME,
	unsigned char* REC_FRAME) {

	struct GMV BEST_GMV;
	bool firstGMV = true;

	for (int GMV_X = 0 - range; GMV_X <= range; GMV_X++) {
		for (int GMV_Y = 0 - range; GMV_Y <= range; GMV_Y++) {
			if (((GMV_X + col) < 0) || ((GMV_X + col + block) > width)) {
				continue; //Block outisde search space so don't compute
			}
			if (((GMV_Y + row) < 0) || ((GMV_Y + row + block) > height)) {
				continue; //Block outisde search space so don't compute
			}

			int X = GMV_X;
			int Y = GMV_Y;
			unsigned int SAD = 0;
			unsigned int NORM = abs(X) + abs(Y);

			//Calculate SAD
			for (int i = 0; i < block; i++) {
				for (int j = 0; j < block; j++) {
					SAD += abs(CUR_FRAME[(row + i)*width + col + j] - REC_FRAME[(GMV_Y + row + i)*width + GMV_X + col + j]);
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