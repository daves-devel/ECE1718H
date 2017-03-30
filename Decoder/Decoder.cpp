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
#include <DiffDec.h>


int main(int argCnt, char **args)
{

	//char filepath[500] = "";
	char decfile_name[500] = "";
	char frame_header_name[500] = "";


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

		if (!strcmp((*args) + 1, "decfile")) {
			args++;
			sscanf(*args, "%s", decfile_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "filepath")) {
			args++;
			sscanf(*args, "%s", filepath);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "frame_header")) {
			args++;
			sscanf(*args, "%s", frame_header_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "frames")) {
			args++;
			frames = atoi(*args);
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

	FILE* decfile = fopen(decfile_name, "w+b");
	FILE* reffile = fopen("ref.csv", "w");
	FILE* dectcfile = fopen("dec_tc.csv", "w");
	FILE* decresfile = fopen("dec_res.csv", "w");

	frame_header_file = fopen(frame_header_name, "rb");
	//coeff_bitcount_file = fopen(coeff_bitcount_name, "r");
	//mdiff_bitcount_file = fopen(mdiff_bitcount_name, "r");
	uint32_t sz;

	//Read the frame header once to get frame paramters
	fread(&FrameType, sizeof(int32_t), 1, frame_header_file);
	fread(&block, sizeof(int32_t), 1, frame_header_file);
	fread(&width, sizeof(int32_t), 1, frame_header_file);
	fread(&height, sizeof(int32_t), 1, frame_header_file);

	unsigned int  FRAME_SIZE = width*height;
	//reset to begining of file
	fseek(frame_header_file, 0, SEEK_SET);

	// Allocate Memory
	uint8_t** DEC_FRAME_2D = new uint8_t*[height];
	uint8_t** REF_FRAME_2D = new uint8_t*[height];
	uint8_t** PREV_DEC_FRAME = new uint8_t*[height];
	int32_t** ENC_RES_FRAME_2D = new  int32_t*[height];
	int32_t** ENC_TC_FRAME_2D = new int32_t*[height];
	int32_t** DEC_RES_FRAME_2D = new  int32_t*[height];
	int32_t** DEC_TC_FRAME_2D = new int32_t*[height];
	int32_t** QTC_FRAME_2D = new int32_t*[height];
	int32_t** QP_FRAME_2D = new int32_t*[height];

	for (unsigned int row = 0; row < height; row++) {
		DEC_FRAME_2D[row] = new uint8_t[width];
		REF_FRAME_2D[row] = new uint8_t[width];
		PREV_DEC_FRAME[row] = new uint8_t[width];
		ENC_RES_FRAME_2D[row] = new  int32_t[width];
		ENC_TC_FRAME_2D[row] = new int32_t[width];
		DEC_RES_FRAME_2D[row] = new  int32_t[width];
		DEC_TC_FRAME_2D[row] = new int32_t[width];
		QTC_FRAME_2D[row] = new int32_t[width];
		QP_FRAME_2D[row] = new int32_t[width];
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
		//Read the paratemters for the frame
		fread(&FrameType, sizeof(int32_t), 1, frame_header_file);
		fread(&block, sizeof(int32_t), 1, frame_header_file);
		fread(&width, sizeof(int32_t), 1, frame_header_file);
		fread(&height, sizeof(int32_t), 1, frame_header_file);

		if (FrameType == PFRAME) {
			// Go to the beginning of the previous reconstructed frame and copy it to buffer
			fseek(decfile, (frame - 1)*FRAME_SIZE, SEEK_SET);
			for (unsigned int row = 0; row < height; row++) {
				fread(PREV_DEC_FRAME[row], sizeof(uint8_t), width, decfile);
			}
			// Create a intermediate frame from the previous decoded frame and the motion vectors for the frame
	/*		for (int row = 0; row < height; row += block) {
				for (int col = 0; col < width; col += block) {
					for (int i = 0; i < block; i++) {
						for (int j = 0; j < block; j++) {
							PREV_DEC_FRAME[row + i][col + j] = DEC_FRAME_2D[row + i][col + j];
						}
					}
				}
			}*/
		}
		/*
		snprintf(mdiff_name, sizeof(mdiff_name), "%s/MDIFF_GOLOMB_%d", filepath, frame);
		mdiff_file = fopen(mdiff_name, "rb");

		for (int row = 0; row < height; row += block) {
			for (int col = 0; col < width; col += block) {
				if (FrameType == PFRAME) {
					 fread(&MDIFF_VECTOR_DIFF[row / block][col / block].X, sizeof(uint32_t), 1, mdiff_file);
					 fread(&MDIFF_VECTOR_DIFF[row / block][col / block].Y, sizeof(uint32_t), 1, mdiff_file);
					 fread(&MDIFF_VECTOR_DIFF[row / block][col / block].ref, sizeof(uint32_t), 1, mdiff_file);
				}
				else if (FrameType == IFRAME)
				{
					fread(&MDIFF_VECTOR_DIFF[row / block][col / block].MODE, sizeof(uint32_t), 1, mdiff_file);
				}
			}
		}

		fclose(mdiff_file);

		snprintf(coeff_name, sizeof(coeff_name), "%s/COEFF_GOLOMB_CODING_%d", filepath, frame);
		coeff_file = fopen(coeff_name, "rb");
		fseek(coeff_file, 0L, SEEK_END);
		sz = ftell(coeff_file);
		rewind(coeff_file);
		fread(QTC_FRAME_2D, sizeof(uint32_t), sz, coeff_file);

		fclose(coeff_file);
		*/
		reverse_entropy(QTC_FRAME_2D, block, height, width, frame);
		decode_mdiff_wrapper(MDIFF_VECTOR_DIFF, height, width, block, frame, FrameType); //x,y or intramode
		diff_dec_wrapper(MDIFF_VECTOR, MDIFF_VECTOR_DIFF, FrameType, height, width, block, frame);

		// Apply Decode Operations on Each Block
		for (int row = 0; row < height; row += block) {
			for (int col = 0; col < width; col += block) {
				// IDEALLY THREAD EVERYTHING IN THIS FOR LOOP FOR PFRAMES

				// SCALE
				ScaleBlock(DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D, row, col, width, height, QP, block);

				// IDCT
				IDCTBlock(DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, row, col, block);

				if (FrameType == PFRAME) {
					// RECONSTRUCT 
					ReconstructBlockDecodeP(DEC_FRAME_2D, DEC_RES_FRAME_2D, PREV_DEC_FRAME, REF_FRAME_2D, row, col, block, MDIFF_VECTOR);
				}
				else if (FrameType == IFRAME) {
					// RECONSTRUCT 
					ReconstructBlockDecodeI(DEC_FRAME_2D, DEC_RES_FRAME_2D, REF_FRAME_2D, row, col, block, MDIFF_VECTOR);
				}
			}
		}

		
		write_mat(reffile, REF_FRAME_2D, height, width);
		write_mat3(decresfile, DEC_RES_FRAME_2D, height, width);
		write_mat2(dectcfile, DEC_TC_FRAME_2D, height, width);

		uint8_t *DEC_FRAME = new uint8_t[FRAME_SIZE];
		for (int row = 0; row < height; row++)
			for (int col = 0; col < width; col++)
				DEC_FRAME[col + width*row] = DEC_FRAME_2D[row][col];
		fclose(decfile);//Weird behavior need to close file
		decfile = fopen(decfile_name, "a+b");
		fwrite(DEC_FRAME, sizeof(uint8_t), FRAME_SIZE, decfile);
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
	fclose(reffile);
	fclose(decresfile);
	fclose(dectcfile);
	//fclose(mdiff_bitcount_file);
	//fclose(coeff_bitcount_file);
	return 0;

}