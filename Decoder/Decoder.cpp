// Authors:		Juan Fuentes
//				Irfan Khan (khanirf1) 999207665
//				David Chakkuthara 995435266
// Date:		March 4th, 2016
// Description: Given MDiff File and a QTC Coeff. File
//				create a decoded video file

#include <common.h>
#include <residual.h>
#include <reconstructed.h>
#include <quantization.h>
#include <entropy.h>
#include <InterFramePrediction.h>
#include <IntraFramePrediction.h>
#include <discrete_cosine_transform.h>
#include <DiffEnc.h>

int main(int argCnt, char **args)
{

	char qtcfile_name[500] = "";
	char mdiff_file_name[500] = "";
	char decfile_name[500] = "";


	int width = -1;
	int height = -1;
	int frames = -1;
	int range = -1;
	int block = -1;
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
		else if (!strcmp((*args) + 1, "qtcfile")) {
			args++;
			sscanf(*args, "%s", qtcfile_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "mdiff_file")) {
			args++;
			sscanf(*args, "%s", mdiff_file_name);
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

	FILE* qtcfile = fopen(qtcfile_name, "rb");
	FILE* mdiff_file = fopen(mdiff_file_name, "rb");
	FILE* decfile = fopen(decfile_name, "w+b");

	/*
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
	*/
	unsigned int  FRAME_SIZE = width*height;
	signed char* COEFF_REORDER = new signed char[FRAME_SIZE];
	signed char* RLE = new signed char[FRAME_SIZE];

	// Allocate Memory
	uint8_t** DEC_FRAME_2D = new uint8_t*[height];
	int8_t** ENC_RES_FRAME_2D = new  int8_t*[height];
	int32_t** ENC_TC_FRAME_2D = new int32_t*[height];
	int8_t** DEC_RES_FRAME_2D = new  int8_t*[height];
	int32_t** DEC_TC_FRAME_2D = new int32_t*[height];
	int32_t** QTC_FRAME_2D = new int32_t*[height];
	uint8_t** QP_FRAME_2D = new uint8_t*[height];

	for (unsigned int row = 0; row < height; row++) {
		CUR_FRAME_2D[row] = new uint8_t[width];
		DEC_FRAME_2D[row] = new uint8_t[width];
		REF_FRAME_2D[row] = new uint8_t[width];
		ENC_RES_FRAME_2D[row] = new  int8_t[width];
		ENC_TC_FRAME_2D[row] = new int32_t[width];
		DEC_RES_FRAME_2D[row] = new  int8_t[width];
		DEC_TC_FRAME_2D[row] = new int32_t[width];
		QTC_FRAME_2D[row] = new int32_t[width];
		QP_FRAME_2D[row] = new uint8_t[width];
	}


	// This 2D Buffer will Contain MDIFF data for each block 
	struct MDIFF** MDIFF_VECTOR = new struct MDIFF*[(height / block)];
	struct MDIFF** MDIFF_VECTOR_DIFF = new struct MDIFF*[(height / block)];

	for (int row = 0; row < height; row = row + block) {
		MDIFF_VECTOR[row / block] = new struct MDIFF[width / block];
		MDIFF_VECTOR_DIFF[row / block] = new struct MDIFF[width / block];
	}



	// Decode Each Frame
	// =========================================
	for (int frame = 0; frame < frames; frame++) {

		if ((frame%i_period) == 0) {
			FrameType = IFRAME;
		}
		else {
			FrameType = PFRAME;
		}

		if (FrameType == PFRAME) {
			// Go to the beginning of the previous reconstructed frame and copy it to buffer
			fseek(recfile, (frame - 1)*FRAME_SIZE, SEEK_SET);
			for (unsigned int row = 0; row++; row < height) {
				fread(REC_FRAME_2D[row], sizeof(uint8_t), width, recfile);
			}
		}

		// Go to the beginning of the current frame and copy it to buffer

		fseek(curfile, frame*FRAME_SIZE, SEEK_SET);
		for (unsigned int row = 0; row++; row < height) {
			fread(CUR_FRAME_2D[row], sizeof(uint8_t), width, curfile);
		}

		// Apply Decode Operations on Each Block
		for (int row = 0; row < height; row += block) {
			for (int col = 0; col < width; col += block) {

				// IDEALLY THREAD EVERYTHING IN THIS FOR LOOP FOR PFRAMES

				// PREDICTOR DATA GENERATION
				if (FrameType == IFRAME) {
					MDIFF_VECTOR[row / block][col / block] = IntraFramePrediction(CUR_FRAME_2D, REC_FRAME_2D, REF_FRAME_2D, row, col, block);
				}

				if (FrameType == PFRAME) {
					MDIFF_VECTOR[row / block][col / block] = InterFramePrediction(CUR_FRAME_2D, REC_FRAME_2D, REF_FRAME_2D, row, col, width, height, block, range);
				}

				// RESIDUAL 
				GenerateResidualBlock(ENC_RES_FRAME_2D, CUR_FRAME_2D, REF_FRAME_2D, row, col, block);

				// DCT 
				DCTBlock(ENC_TC_FRAME_2D, ENC_RES_FRAME_2D, row, col, block);

				// QUANTIZE
				QuantizeBlock(QTC_FRAME_2D, ENC_TC_FRAME_2D, QP_FRAME_2D, row, col, width, height, QP, block);

				// SCALE
				ScaleBlock(DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D, row, col, width, height, QP, block);

				// IDCT
				IDCTBlock(DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, row, col, block);

				// RECONSTRUCT 
				ReconstructBlock(REC_FRAME_2D, DEC_RES_FRAME_2D, REF_FRAME_2D, row, col, block);

			}
		}

		entropy_wrapper(QTC_FRAME_2D, block, height, width, frame);
		diff_enc_wrapper(MDIFF_VECTOR, MDIFF_VECTOR_DIFF, 0, height, width, block, frame);
		encode_mdiff_wrapper(MDIFF_VECTOR_DIFF, height, width, block, frame, 0);

		// =====================================================================================================
		// TODOOOOO
		// Any File Dumps can be added on any 2D array here for verification purpose
		// =====================================================================================================

	}



	// Deallocate Memory
	for (unsigned int row = 0; row < height; row++) {
		delete		CUR_FRAME_2D[row];
		delete		REC_FRAME_2D[row];
		delete		REF_FRAME_2D[row];
		delete	ENC_RES_FRAME_2D[row];
		delete   ENC_TC_FRAME_2D[row];
		delete	DEC_RES_FRAME_2D[row];
		delete   DEC_TC_FRAME_2D[row];
		delete		QTC_FRAME_2D[row];
		delete		 QP_FRAME_2D[row];
	}

	delete CUR_FRAME_2D;
	delete REC_FRAME_2D;
	delete REF_FRAME_2D;
	delete ENC_RES_FRAME_2D;
	delete  ENC_TC_FRAME_2D;
	delete DEC_RES_FRAME_2D;
	delete  DEC_TC_FRAME_2D;
	delete QTC_FRAME_2D;
	delete  QP_FRAME_2D;

	for (unsigned int row = 0; row < (height / block); row++) {
		delete MDIFF_VECTOR[row];
	}

	delete MDIFF_VECTOR;

	// Close Files
	//fclose(curfile);
	//fclose(mvfile);
	//fclose(gmvXfile);
	//fclose(gmvYfile);
	//fclose(resfile);
	//fclose(recfile);
	//fclose(matchfile);
	return 0;

}