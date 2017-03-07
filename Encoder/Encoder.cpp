// Authors:		Juan Fuentes
//				Irfan Khan (khanirf1) 999207665
//				David Chakkuthara 995435266
// Date:		March 4th, 2016
// Description: Given a Y (LUMA) only File, it will be encoded 
//				Outputs an encoded MDiff File
//				Outputs an encoded QTC Coeff. File

#include <common.h>
#include <residual.h>
#include <reconstructed.h>
#include <quantization.h>
#include <entropy.h>
#include <InterFramePrediction.h>
#include <IntraFramePrediction.h>
#include <discrete_cosine_transform.h>

int main(int argCnt, char **args)
{

	char curfile_name[500] = "";
	char mvfile_name[500] = "";
	char resfile_name[500] = "";
	char recfile_name[500] = "";
	char matchfile_name[500] = "";
	char gmvx_name[500] = "";
	char gmvy_name[500] = "";

	int width = -1;
	int height = -1;
	int frames = -1;
	int range = -1;
	int block = -1;
	int padRight = -1;
	int padBottom = -1;
	int round = -1;
	int i_period = -1;
	int FrameType = -1;
	int QP = -1;

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

	//TODO Convert these 1D Frames to 2D Frames
	signed char* CUR_FRAME = new signed char[FRAME_SIZE];
	unsigned char* REC_FRAME = new unsigned char[FRAME_SIZE];
	signed char* RES_FRAME = new   signed char[FRAME_SIZE];

	signed char* COEFF_REORDER = new signed char[FRAME_SIZE];
	signed char* RLE = new signed char[FRAME_SIZE];

	unsigned char** MATCH_FRAME = new unsigned char*[height];
	signed int** TC_FRAME = new signed int*[height];
	signed int** QTC_FRAME = new signed int*[height];
	signed char** CUR_FRAME_2D = new signed char*[height];
	signed char** REC_FRAME_2D = new signed char*[height];

	for (unsigned int row = 0; row < height; row++) {
		MATCH_FRAME[row] = new unsigned char[width];
		TC_FRAME[row] = new signed int[width];
		QTC_FRAME[row] = new signed int[width];
		CUR_FRAME_2D[row] = new signed char[width];
		REC_FRAME_2D[row] = new signed char[width];
	}

	// This 1D Buffer will Contain MDIFF data for each block in raster row order
	// TODO Convert to 2D array
	struct MDIFF* MDIFF_VECTOR = new struct MDIFF[(width / block)*(height / block)];

	//TEST JUAN
/*	int size = 4;
	int index = 0;
	int8_t * out = new int8_t[size *size];
	int8_t * RLE = new int8_t[size*size + size*size];
	int8_t ** in = new int8_t*[size];
	for (int i = 0; i < size; i++)
		in[i] = new int8_t[size];
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++) {
			in[i][j] = index;
			index++;
		}
	}
	in[0][0] = -31;
	in[0][1] = 9;
	in[0][2] = 8;
	in[0][3] = 4;
	in[1][0] = -4;
	in[1][1] = 1;
	in[1][2] = 4;
	in[1][3] = 0;
	in[2][0] = -3;
	in[2][1] = 2;
	in[2][2] = 4;
	in[2][3] = 0;
	in[3][0] = 4;
	in[3][1] = 0;
	in[3][2] = -4;
	in[3][3] = 2;

	int total_counter=entropy(in, out, size, RLE);
	FILE* test = fopen("test.txt", "w");
	fprint_coeef(in, out, size, test, RLE, total_counter);
	fclose(test);
	*/
	// Encode Each Frame
	// =========================================
	for (int frame = 0; frame < frames; frame++) {
		/*ENCODING TEST BEGIn
		// Print MDIFF File
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

					// TODO Remove this when we do 2D Arrays
					for (unsigned int copy_row = 0; copy_row < width; copy_row++) {
						for (unsigned int copy_col = 0; copy_col < height; copy_col++) {
							CUR_FRAME_2D[copy_row][copy_col] = CUR_FRAME[copy_row*width + copy_col];
						}
					}

					MDIFF_VECTOR[((row*width / block) / block) + (col / block)] = IntraFramePrediction(row, col, block, CUR_FRAME_2D); // TODO Change when all arrays are 2D
				
					int MODE = MDIFF_VECTOR[((row*width / block) / block) + (col / block)].MODE;

					// Fill the Match Frame with the best matching block
					if (MODE == HORIZONTAL) {
						for (int i = 0; i < block; i++) {
							for (int j = 0; j < block; j++) {
								if (col == 0) {
									MATCH_FRAME[row + i][col + j] = 128;
								}
								else {
									MATCH_FRAME[row + i][col + j] = CUR_FRAME[(row + i) * width + (col - 1)];
								}
							}
						}
					}
					if (MODE == VERTICAL){
						for (int i = 0; i < block; i++) {
							for (int j = 0; j < block; j++) {
								if (col == 0) {
									MATCH_FRAME[row + i][col + j] = 128;
								}
								else {
									MATCH_FRAME[row + i][col + j] = CUR_FRAME[(row - 1) * width + (col + j)];
								}
							}
						}
					}
				}
				
				if (FrameType == PFRAME) {
					
					MDIFF_VECTOR[((row*width / block) / block) + (col / block)] = InterFramePrediction(row, col, width, height, block, range, CUR_FRAME, REC_FRAME);

					int GMV_X = MDIFF_VECTOR[((row*width / block) / block) + (col / block)].X;
					int GMV_Y = MDIFF_VECTOR[((row*width / block) / block) + (col / block)].Y;

					// Fill the Match Frame with the best matching block
					for (int i = 0; i < block; i++) {
						for (int j = 0; j < block; j++) {
							MATCH_FRAME[row + i][col + j] = REC_FRAME[(row + GMV_Y + i) * width + (col + GMV_X + j)];
						}
					}
				}

				int DATA_1 = MDIFF_VECTOR[((row*width / block) / block) + (col / block)].X;
				int DATA_2 = MDIFF_VECTOR[((row*width / block) / block) + (col / block)].Y;

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
		ENCODING TEST END*/

		fseek(curfile, frame*FRAME_SIZE, SEEK_SET);
		fread(CUR_FRAME, sizeof(signed char), FRAME_SIZE, curfile);
		for (int row = 0; row < height; row += block) {
			for (int col = 0; col < width; col += block) {
				CUR_FRAME_2D[row][col] = CUR_FRAME[row*width + col];
			}
		}

		// TRANFORM FRAME
		// =========================================================================
		dct(TC_FRAME, CUR_FRAME_2D, height, width);

		// QUANTIZE FRAME
		// =========================================================================
		Quantize(TC_FRAME, QTC_FRAME, QP, block, width, height);

		// ENTROPY ENCODE FRAME
		// =========================================================================
	/*	int8_t ** QTC_BLOCK = new int8_t*[block];
		for (int i = 0; i < block; i++)
			QTC_BLOCK[i] = new int8_t[block];

		for (int row = 0; row < height; row += block) {
			for (int col = 0; col < width; col += block) {
				for (int j = 0; j < block; j++)
					for (int i = 0; i < block; i++)
						QTC_BLOCK[j][i] = QTC_FRAME[row + j][col + i];
				entropy(QTC_BLOCK, block, RLE);
			}
		}
		delete QTC_BLOCK;*/
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
		idct(REC_FRAME_2D, TC_FRAME,width, height);

		for (unsigned int row = 0; row < height; row++) {
			fwrite(REC_FRAME_2D[row], sizeof(unsigned char), width, recfile);
		}

		// Should Have REC_FRAME for next iteration of the loop now. REC is only used in P frames though

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


