// Authors:		Juan Fuentes
//				Irfan Khan (khanirf1) 999207665
//				David Chakkuthara 995435266
// Date:		March 4th, 2016
// Description: Given MDiff File and a QTC Coeff. File
//				create a decoded video file

#include <common.h>
#include <residual.h>
#include <entropy.h>
#include <reconstructed.h>
#include <quantization.h>
#include <reverse_entropy.h>
#include <InterFramePrediction.h>
#include <IntraFramePrediction.h>
#include <discrete_cosine_transform.h>
#include <DiffEnc.h>

int main(int argCnt, char **args)
{

	char coeff_bitcount_name[500] = "";
	char mdiff_bitcount_name[500] = "";
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
		else if (!strcmp((*args) + 1, "coeff_bitcount_name")) {
			args++;
			sscanf(*args, "%s", coeff_bitcount_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "mdiff_bitcount_name")) {
			args++;
			sscanf(*args, "%s", mdiff_bitcount_name);
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
		else {
			printf("Huh? I don't know %s (option #%d) \n", *args, tmpArgCnt);
			exit(-1);
		}
	}

	FILE* decfile = fopen(decfile_name, "wb");
	coeff_bitcount_file = fopen(coeff_bitcount_name, "r");
	mdiff_bitcount_file = fopen(mdiff_bitcount_name, "r");

	unsigned int  FRAME_SIZE = width*height;

	// Allocate Memory
	uint8_t** DEC_FRAME_2D = new uint8_t*[height];
	uint8_t** REF_FRAME_2D = new uint8_t*[height];
	int8_t** ENC_RES_FRAME_2D = new  int8_t*[height];
	int32_t** ENC_TC_FRAME_2D = new int32_t*[height];
	int8_t** DEC_RES_FRAME_2D = new  int8_t*[height];
	int32_t** DEC_TC_FRAME_2D = new int32_t*[height];
	int32_t** QTC_FRAME_2D = new int32_t*[height];
	uint8_t** QP_FRAME_2D = new uint8_t*[height];

	for (unsigned int row = 0; row < height; row++) {
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
			fseek(decfile, (frame - 1)*FRAME_SIZE, SEEK_SET);
			for (unsigned int row = 0; row++; row < height) {
				fread(DEC_FRAME_2D[row], sizeof(uint8_t), width, decfile);
			}
		}

		// Go to the beginning of the current frame and copy it to buffer
		reverse_entropy(QTC_FRAME_2D, block, height, width, frame);
		decode_mdiff_wrapper(MDIFF_VECTOR_DIFF, height, width, block, frame, FrameType);

		// Apply Decode Operations on Each Block
		for (int row = 0; row < height; row += block) {
			for (int col = 0; col < width; col += block) {

				// IDEALLY THREAD EVERYTHING IN THIS FOR LOOP FOR PFRAMES

				// SCALE
				ScaleBlock(DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D, row, col, width, height, QP, block);

				// IDCT
				IDCTBlock(DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, row, col, block);

				// RECONSTRUCT 
				ReconstructBlock(DEC_FRAME_2D, DEC_RES_FRAME_2D, REF_FRAME_2D, row, col, block);

			}
		}

	}

	// Deallocate Memory
	for (unsigned int row = 0; row < height; row++) {
		delete		DEC_FRAME_2D[row];
		delete		REF_FRAME_2D[row];
		delete	ENC_RES_FRAME_2D[row];
		delete   ENC_TC_FRAME_2D[row];
		delete	DEC_RES_FRAME_2D[row];
		delete   DEC_TC_FRAME_2D[row];
		delete		QTC_FRAME_2D[row];
		delete		 QP_FRAME_2D[row];
	}

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
	fclose(decfile);
	fclose(mdiff_bitcount_file);
	fclose(coeff_bitcount_file);
	return 0;

}