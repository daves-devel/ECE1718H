// Author:		Irfan Khan (khanirf1) 999207665
// Date:		February 6th, 2016
// Description: Given a Y only File, this will Encode the Video.
//It will produce a Motion vector file and a Residual Video

#include <common.h>
#include <residual.h>
#include <reconstructed.h>
#include <quantization.h>
#include <InterFramePrediction.h>
#include <IntraFramePrediction.h>

int main(int argCnt, char **args)
{

	char curfile_name[500];
	char mvfile_name[500];
	char resfile_name[500];
	char recfile_name[500];
	char matchfile_name[500];
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
	int i_period	= -1;
	int FrameType	= -1;
	int QP			= -1;

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
		else if (!strcmp((*args) + 1, "matchfile")) {
			args++;
			sscanf(*args, "%s", matchfile_name);
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
		else if (!strcmp((*args) + 1, "i_period")) {
			args++;
			i_period = atoi(*args);
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
		else if (!strcmp((*args) + 1, "qp")) {
			args++;
			QP = atoi(*args);
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
	FILE* matchfile = fopen(matchfile_name, "wb");
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
	  signed char* RES_FRAME	    = new   signed char[FRAME_SIZE];

	// This 2D Buffer Will containe the best blocks for 
	// estimation in their corresponding block locations
	unsigned char** MATCH_FRAME	= new unsigned char*[height];
	  signed char** TC_FRAME	= new   signed char*[height];
	unsigned char** QTC_FRAME	= new unsigned char*[height];

	for (unsigned int row = 0; row < height; row++) {
		MATCH_FRAME[row] = new unsigned char[width];
		TC_FRAME[row]	 = new   signed char[width];
		QTC_FRAME[row]   = new unsigned char[width];
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

		if ((frame%i_period) == 0) { 
			FrameType = IFRAME;
		}
		else {
			FrameType = PFRAME;
		}

		if (FrameType == PFRAME){
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

				if (FrameType == IFRAME) {

					GMV_VECTOR[((row*width / block) / block) + (col / block)] = IntraFramePrediction(row, col, width, height, block, CUR_FRAME);
				
					int MODE = GMV_VECTOR[((row*width / block) / block) + (col / block)].X;
					int RUN  = GMV_VECTOR[((row*width / block) / block) + (col / block)].Y;

					// Fill the Match Frame with the best matching block
					if (MODE == HORIZONTAL) {
						for (int i = 0; i < block; i++) {
							for (int j = 0; j < block; j++) {
								// TODO / Need an Intra mode algoritm
								MATCH_FRAME[row + i][col + j] = REC_FRAME[(row + MODE) * width + (col + MODE + j)];
							}
						}
					}
					if (MODE == VERTICAL){
						for (int i = 0; i < block; i++) {
							for (int j = 0; j < block; j++) {
								// TODO / Need an Intra mode algoritm
								MATCH_FRAME[row + i][col + j] = REC_FRAME[(row + MODE) * width + (col + MODE + j)];
							}
						}
					}
				}
				
				if (FrameType == PFRAME) {
					
					GMV_VECTOR[((row*width / block) / block) + (col / block)] = InterFramePrediction(row, col, width, height, block, range, CUR_FRAME, REC_FRAME);

					int GMV_X = GMV_VECTOR[((row*width / block) / block) + (col / block)].X;
					int GMV_Y = GMV_VECTOR[((row*width / block) / block) + (col / block)].Y;

					// Fill the Match Frame with the best matching block
					for (int i = 0; i < block; i++) {
						for (int j = 0; j < block; j++) {
							MATCH_FRAME[row + i][col + j] = REC_FRAME[(row + GMV_Y + i) * width + (col + GMV_X + j)];
						}
					}
				}

				int DATA_1 = GMV_VECTOR[((row*width / block) / block) + (col / block)].X;
				int DATA_2 = GMV_VECTOR[((row*width / block) / block) + (col / block)].Y;

				// MV FILE GENERATION (VECTOR DUMP)
				// =======================================
				fwrite(&DATA_1, sizeof(int), 1, gmvXfile);
				fwrite(&DATA_2, sizeof(int), 1, gmvYfile);
				if (FrameType == IFRAME) {
					fprintf(mvfile, "B(%d,%d)_M(%d,%d)\n", row / block, col / block, DATA_1, DATA_2);
				}
				if (FrameType == PFRAME) {
					fprintf(mvfile, "B(%d,%d)_V(%d,%d)\n", row / block, col / block, DATA_1, DATA_2);
				}
				
			}
		}

		
		for (unsigned int row = 0; row < height; row++) {
			fwrite(MATCH_FRAME[row], sizeof(unsigned char), width, matchfile);
		}

		// RESIDUAL FILE GENERATION
		// =========================================================================
		residual(RES_FRAME, CUR_FRAME, block, width, height, round, MATCH_FRAME);
		fwrite(RES_FRAME, sizeof(unsigned char), FRAME_SIZE, resfile);

		// TRANFORM FRAME
		// =========================================================================
		// TODO
		// DCT(TC_FRAME, RES_FRAME, width, height);

		// QUANTIZE FRAME
		// =========================================================================
		Quantize(TC_FRAME, QTC_FRAME, QP, block, width, height);

		// ENTROPY ENCODE FRAME
		// =========================================================================
		// TODO
		
		// DUMP ENCODED DATA
		// =========================================================================
		// TODO

		// ENTROPY DECODE FRAME
		// =========================================================================
		// TODO

		// RESCALING
		// =========================================================================
		Rescale(QTC_FRAME, TC_FRAME, QP, block, width, height);

		// INV DCT
		// =========================================================================
		// IDCT (TC_FRAME,REC_FRAME,width,height);

		// Should Have REC_FRAME for next iteration of the loop now. REC is only used in P frames though

	}

	for (unsigned int row = 0; row < height; row++) {
		delete MATCH_FRAME[row];
	}
	delete MATCH_FRAME;
	delete CUR_FRAME;
	delete REC_FRAME;
	fclose(curfile);
	fclose(mvfile);
	fclose(gmvXfile);
	fclose(gmvYfile);
	fclose(resfile);
	fclose(recfile);
	fclose(matchfile);
	return 0;

}


