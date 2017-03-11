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
#include <DiffEnc.h>

int main(int argCnt, char **args)
{

	char curfile_name[500] = "";
	char mvfile_name[500] = "";
	char resfile_name[500] = "";
	char recfile_name[500] = "";
	char matchfile_name[500] = "";
	char gmvx_name[500] = "";
	char gmvy_name[500] = "";
	char coeff_bitcount_name[500] = "";
	char mdiff_bitcount_name[500] = "";
	char frame_header_name[500] = "";

	int width = -1;
	int height = -1;
	int frames = -1;
	int range = -1;
	int block = -1;
	int i_period = -1;
	int FrameType = -1;
	int QP = -1;
	int nRefFrames = 1;
	int VBSEnable = 0;

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
		else if (!strcmp((*args) + 1, "curfile")) {
			args++;
			sscanf(*args, "%s", curfile_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "recfile")) {
			args++;
			sscanf(*args, "%s", recfile_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "frame_header")) {
			args++;
			sscanf(*args, "%s", frame_header_name);
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
		else if (!strcmp((*args) + 1, "nRefFrames")) {
			args++;
			nRefFrames = atoi(*args);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "VBSEnable")) {
			args++;
			VBSEnable = atoi(*args);
			args++;
			tmpArgCnt += 2;
		}

		else {
			printf("Huh? I don't know %s (option #%d) \n", *args, tmpArgCnt);
			exit(-1);
		}
	}

	FILE* curfile = fopen(curfile_name, "rb");
	coeff_bitcount_file = fopen(coeff_bitcount_name, "w");
	mdiff_bitcount_file = fopen(mdiff_bitcount_name, "w");
	frame_header_file = fopen(frame_header_name, "w+b");
	FILE* recfile = fopen(recfile_name, "w+b");

	// TODO Make these 2D buffers, and add the Encoder functions to the frame flow
	unsigned int  FRAME_SIZE = width*height;
	int block_split = block / 2;

	// Allocate Memory
	uint8_t** CUR_FRAME_2D	 = new uint8_t*[height];

	uint8_t** REC_FRAME_2D  = new uint8_t*[height];
	uint8_t** REC_FRAME_2D_2 = new uint8_t*[height];
	uint8_t** REC_FRAME_2D_3 = new uint8_t*[height];
	uint8_t** REC_FRAME_2D_4 = new uint8_t*[height];

	uint8_t** REF_FRAME_2D	 = new uint8_t*[height];
	uint8_t** REF_FRAME_2D_2 = new uint8_t*[height];
	uint8_t** REF_FRAME_2D_3 = new uint8_t*[height];
	uint8_t** REF_FRAME_2D_4 = new uint8_t*[height];

	 int8_t** ENC_RES_FRAME_2D	= new  int8_t*[height];
	int32_t** ENC_TC_FRAME_2D	= new int32_t*[height];
	 int8_t** DEC_RES_FRAME_2D	= new  int8_t*[height];
	int32_t** DEC_TC_FRAME_2D	= new int32_t*[height];
	int32_t** QTC_FRAME_2D		= new int32_t*[height];
	uint8_t** QP_FRAME_2D		= new uint8_t*[height];

	for (unsigned int row = 0; row < height; row++) {
		CUR_FRAME_2D[row] = new uint8_t[width];

		REC_FRAME_2D[row] = new uint8_t[width];
		REC_FRAME_2D_2[row] = new uint8_t[width];
		REC_FRAME_2D_3[row] = new uint8_t[width];
		REC_FRAME_2D_4[row] = new uint8_t[width];

		REF_FRAME_2D[row] = new uint8_t[width];
		REF_FRAME_2D_2[row] = new uint8_t[width];
		REF_FRAME_2D_3[row] = new uint8_t[width];
		REF_FRAME_2D_4[row] = new uint8_t[width];

		ENC_RES_FRAME_2D[row] = new  int8_t[width];
		ENC_TC_FRAME_2D[row] = new int32_t[width];
		DEC_RES_FRAME_2D[row] = new  int8_t[width];
		DEC_TC_FRAME_2D[row] = new int32_t[width];
		QTC_FRAME_2D[row] = new int32_t[width];
		QP_FRAME_2D[row] = new uint8_t[width];
	}

	
	// This 2D Buffer will Contain MDIFF data for each block 
	struct MDIFF** MDIFF_VECTOR = new struct MDIFF*[(height / block)];
	struct MDIFF** MDIFF_VECTOR_2 = new struct MDIFF*[(height / block)];
	struct MDIFF** MDIFF_VECTOR_3 = new struct MDIFF*[(height / block)];
	struct MDIFF** MDIFF_VECTOR_4 = new struct MDIFF*[(height / block)];

	struct MDIFF** MDIFF_VECTOR_DIFF = new struct MDIFF*[(height / block)];

	for (int row = 0; row < height; row = row + block) {
		MDIFF_VECTOR[row / block] = new struct MDIFF[width / block];
		MDIFF_VECTOR_2[row / block] = new struct MDIFF[width / block];
		MDIFF_VECTOR_3[row / block] = new struct MDIFF[width / block];
		MDIFF_VECTOR_4[row / block] = new struct MDIFF[width / block];
		MDIFF_VECTOR_DIFF[row / block] = new struct MDIFF[width / block];
	}

	// Allocate Memory for split buffer
	uint8_t** CUR_FRAME_2DS = new uint8_t*[height];

	uint8_t** REC_FRAME_2DS = new uint8_t*[height];
	uint8_t** REC_FRAME_2D_2S = new uint8_t*[height];
	uint8_t** REC_FRAME_2D_3S = new uint8_t*[height];
	uint8_t** REC_FRAME_2D_4S = new uint8_t*[height];

	uint8_t** REF_FRAME_2DS = new uint8_t*[height];
	uint8_t** REF_FRAME_2D_2S = new uint8_t*[height];
	uint8_t** REF_FRAME_2D_3S = new uint8_t*[height];
	uint8_t** REF_FRAME_2D_4S = new uint8_t*[height];

	int8_t** ENC_RES_FRAME_2DS = new  int8_t*[height];
	int32_t** ENC_TC_FRAME_2DS = new int32_t*[height];
	int8_t** DEC_RES_FRAME_2DS = new  int8_t*[height];
	int32_t** DEC_TC_FRAME_2DS = new int32_t*[height];
	int32_t** QTC_FRAME_2DS = new int32_t*[height];
	uint8_t** QP_FRAME_2DS = new uint8_t*[height];

	for (unsigned int row = 0; row < height; row++) {
		CUR_FRAME_2DS[row] = new uint8_t[width];

		REC_FRAME_2DS[row] = new uint8_t[width];
		REC_FRAME_2D_2S[row] = new uint8_t[width];
		REC_FRAME_2D_3S[row] = new uint8_t[width];
		REC_FRAME_2D_4S[row] = new uint8_t[width];

		REF_FRAME_2DS[row] = new uint8_t[width];
		REF_FRAME_2D_2S[row] = new uint8_t[width];
		REF_FRAME_2D_3S[row] = new uint8_t[width];
		REF_FRAME_2D_4S[row] = new uint8_t[width];

		ENC_RES_FRAME_2DS[row] = new  int8_t[width];
		ENC_TC_FRAME_2DS[row] = new int32_t[width];
		DEC_RES_FRAME_2DS[row] = new  int8_t[width];
		DEC_TC_FRAME_2DS[row] = new int32_t[width];
		QTC_FRAME_2DS[row] = new int32_t[width];
		QP_FRAME_2DS[row] = new uint8_t[width];
	}


	// This 2D Buffer will Contain MDIFF data for each block 
	struct MDIFF** MDIFF_VECTORS = new struct MDIFF*[(height / block_split)];
	struct MDIFF** MDIFF_VECTOR_2S = new struct MDIFF*[(height / block_split)];
	struct MDIFF** MDIFF_VECTOR_3S = new struct MDIFF*[(height / block_split)];
	struct MDIFF** MDIFF_VECTOR_4S = new struct MDIFF*[(height / block_split)];

	struct MDIFF** MDIFF_VECTOR_DIFFS = new struct MDIFF*[(height / block_split)];

	for (int row = 0; row < height; row = row + block_split) {
		MDIFF_VECTORS[row / block_split] = new struct MDIFF[width / block_split];
		MDIFF_VECTOR_2S[row / block_split] = new struct MDIFF[width / block_split];
		MDIFF_VECTOR_3S[row / block_split] = new struct MDIFF[width / block_split];
		MDIFF_VECTOR_4S[row / block_split] = new struct MDIFF[width / block_split];
		MDIFF_VECTOR_DIFFS[row / block_split] = new struct MDIFF[width / block_split];
	}


	// Encode Each Frame
	// =========================================
	for (int frame = 0; frame < frames; frame++) {

		if ((frame%i_period) == 0) { 
			FrameType = IFRAME;
			fwrite(&FrameType, sizeof(int32_t), 1, frame_header_file);
			fwrite(&block, sizeof(int32_t), 1, frame_header_file);
			fwrite(&width, sizeof(int32_t), 1, frame_header_file);
			fwrite(&height, sizeof(int32_t), 1, frame_header_file);
		}
		else {
			FrameType = PFRAME;
			fwrite(&FrameType, sizeof(int32_t), 1, frame_header_file);
			fwrite(&block, sizeof(int32_t), 1, frame_header_file);
			fwrite(&width, sizeof(int32_t), 1, frame_header_file);
			fwrite(&height, sizeof(int32_t), 1, frame_header_file);
		}

		if (FrameType == PFRAME) {
			// Go to the beginning of the previous reconstructed frame and copy it to buffer
			fseek(recfile, (frame - 1)*FRAME_SIZE, SEEK_SET);
			for (unsigned int row = 0; row++; row < height) {
				fread(REC_FRAME_2D[row], sizeof(uint8_t), width, recfile);
			}
			if (frame > 2) {
				fseek(recfile, (frame - 2)*FRAME_SIZE, SEEK_SET);
				for (unsigned int row = 0; row++; row < height) {
					fread(REC_FRAME_2D_2[row], sizeof(uint8_t), width, recfile);
				}
			}
			if (frame > 3) {
				fseek(recfile, (frame - 3)*FRAME_SIZE, SEEK_SET);
				for (unsigned int row = 0; row++; row < height) {
					fread(REC_FRAME_2D_3[row], sizeof(uint8_t), width, recfile);
				}
			}
			if (frame > 4) {
				fseek(recfile, (frame - 4)*FRAME_SIZE, SEEK_SET);
				for (unsigned int row = 0; row++; row < height) {
					fread(REC_FRAME_2D_4[row], sizeof(uint8_t), width, recfile);
				}
			}
			if (VBSEnable) { //fill split buffers
				fseek(recfile, (frame - 1)*FRAME_SIZE, SEEK_SET);
				for (unsigned int row = 0; row++; row < height) {
					fread(REC_FRAME_2DS[row], sizeof(uint8_t), width, recfile);
				}
				if (frame > 2) {
					fseek(recfile, (frame - 2)*FRAME_SIZE, SEEK_SET);
					for (unsigned int row = 0; row++; row < height) {
						fread(REC_FRAME_2D_2S[row], sizeof(uint8_t), width, recfile);
					}
				}
				if (frame > 3) {
					fseek(recfile, (frame - 3)*FRAME_SIZE, SEEK_SET);
					for (unsigned int row = 0; row++; row < height) {
						fread(REC_FRAME_2D_3S[row], sizeof(uint8_t), width, recfile);
					}
				}
				if (frame > 4) {
					fseek(recfile, (frame - 4)*FRAME_SIZE, SEEK_SET);
					for (unsigned int row = 0; row++; row < height) {
						fread(REC_FRAME_2D_4S[row], sizeof(uint8_t), width, recfile);
					}
				}
			}
		}//end split buffer

		// Go to the beginning of the current frame and copy it to buffer

		fseek(curfile, frame*FRAME_SIZE, SEEK_SET);
		for (unsigned int row = 0; row < height; row++) {
			fread(CUR_FRAME_2D[row], sizeof(uint8_t), width, curfile);
		}
		if (VBSEnable) { //fill split buffer curr_frame
			fseek(curfile, frame*FRAME_SIZE, SEEK_SET);
			for (unsigned int row = 0; row < height; row++) {
				fread(CUR_FRAME_2DS[row], sizeof(uint8_t), width, curfile);
			}
		}//end buffer curr_frame

		// Apply Encode Operations on Each Block
		for (int row = 0; row < height; row += block) {
			for (int col = 0; col < width; col += block) {

				// IDEALLY THREAD EVERYTHING IN THIS FOR LOOP FOR PFRAMES

				// PREDICTOR DATA GENERATION
				if (FrameType == IFRAME) {
					MDIFF_VECTOR[row/block][col/block] = IntraFramePrediction (CUR_FRAME_2D,REC_FRAME_2D,REF_FRAME_2D,row,col,block);
				}
				
				if (FrameType == PFRAME) {
					MDIFF_VECTOR[row/block][col/block] = InterFramePrediction (CUR_FRAME_2D, REC_FRAME_2D, REF_FRAME_2D,row, col, width, height, block, range, 1);
					//Multireference code start Only activated if nRefFrames>=2
					if ((frame%i_period) >= 2 && nRefFrames >= 2) {
						MDIFF_VECTOR_2[row / block][col / block] = InterFramePrediction(CUR_FRAME_2D, REC_FRAME_2D_2, REF_FRAME_2D_2, row, col, width, height, block, range, 2);
						MDIFF_VECTOR[row / block][col / block] = SelectRefWinner(MDIFF_VECTOR[row/block][col/block], MDIFF_VECTOR_2[row/block][col/block], REF_FRAME_2D, REF_FRAME_2D_2, block, row, col);
					}
					if ((frame%i_period) >= 3 && nRefFrames >= 3) {
						MDIFF_VECTOR_3[row / block][col / block] = InterFramePrediction(CUR_FRAME_2D, REC_FRAME_2D_3, REF_FRAME_2D_3, row, col, width, height, block, range, 3);
						MDIFF_VECTOR[row / block][col / block] = SelectRefWinner(MDIFF_VECTOR[row / block][col / block], MDIFF_VECTOR_3[row / block][col / block], REF_FRAME_2D, REF_FRAME_2D_3, block, row, col);

					}
					if ((frame%i_period) >= 4 && nRefFrames >= 4) {
						MDIFF_VECTOR_4[row / block][col / block] = InterFramePrediction(CUR_FRAME_2D, REC_FRAME_2D_4, REF_FRAME_2D_4, row, col, width, height, block, range, 4);
						MDIFF_VECTOR[row / block][col / block] = SelectRefWinner(MDIFF_VECTOR[row / block][col / block], MDIFF_VECTOR_4[row / block][col / block], REF_FRAME_2D, REF_FRAME_2D_4, block, row, col);
					}
					//Multireference code end
				}

				// RESIDUAL 
				GenerateResidualBlock (ENC_RES_FRAME_2D, CUR_FRAME_2D, REF_FRAME_2D, row, col, block);

				// DCT 
				DCTBlock (ENC_TC_FRAME_2D, ENC_RES_FRAME_2D, row, col, block);

				// QUANTIZE
				QuantizeBlock (QTC_FRAME_2D, ENC_TC_FRAME_2D, QP_FRAME_2D,row, col, width, height, QP, block);

				// SCALE
				ScaleBlock (DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,row, col, width, height, QP, block);
				
				// IDCT
				IDCTBlock (DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, row, col, block);

				// RECONSTRUCT 
				ReconstructBlock(REC_FRAME_2D, DEC_RES_FRAME_2D, REF_FRAME_2D, row, col, block);

				if (VBSEnable) {//start VBSEnable code
					for (int row2 = row; row2 < row + (block / block_split); row2 += block_split) {
						for (int col2 = col; col2 < col + (block / block_split); col2 += block_split) {

							// IDEALLY THREAD EVERYTHING IN THIS FOR LOOP FOR PFRAMES

							// PREDICTOR DATA GENERATION
							if (FrameType == IFRAME) {
								MDIFF_VECTORS[row2 / block_split][col2 / block_split] = IntraFramePrediction(CUR_FRAME_2DS, REC_FRAME_2DS, REF_FRAME_2DS, row2, col2, block_split);
							}

							if (FrameType == PFRAME) {
								MDIFF_VECTORS[row2 / block_split][col2 / block_split] = InterFramePrediction(CUR_FRAME_2DS, REC_FRAME_2DS, REF_FRAME_2DS, row2, col2, width, height, block_split, range, 1);
								//Multireference code start Only activated if nRefFrames>=2
								if ((frame%i_period) >= 2 && nRefFrames >= 2) {
									MDIFF_VECTOR_2[row2 / block_split][col2 / block_split] = InterFramePrediction(CUR_FRAME_2DS, REC_FRAME_2D_2S, REF_FRAME_2D_2S, row2, col2, width, height, block_split, range, 2);
									MDIFF_VECTORS[row2 / block_split][col2 / block_split] = SelectRefWinner(MDIFF_VECTORS[row2 / block_split][col2 / block_split], MDIFF_VECTOR_2S[row2 / block_split][col2 / block_split], REF_FRAME_2DS, REF_FRAME_2D_2S, block_split, row2, col2);
								}
								if ((frame%i_period) >= 3 && nRefFrames >= 3) {
									MDIFF_VECTOR_3S[row2 / block_split][col2 / block_split] = InterFramePrediction(CUR_FRAME_2D, REC_FRAME_2D_3, REF_FRAME_2D_3, row2, col2, width, height, block_split, range, 3);
									MDIFF_VECTORS[row2 / block_split][col2 / block_split] = SelectRefWinner(MDIFF_VECTORS[row2 / block_split][col2 / block_split], MDIFF_VECTOR_3S[row2 / block_split][col2 / block_split], REF_FRAME_2DS, REF_FRAME_2D_3S, block_split, row2, col2);

								}
								if ((frame%i_period) >= 4 && nRefFrames >= 4) {
									MDIFF_VECTOR_4S[row2 / block_split][col2 / block_split] = InterFramePrediction(CUR_FRAME_2D, REC_FRAME_2D_4, REF_FRAME_2D_4, row2, col2, width, height, block_split, range, 4);
									MDIFF_VECTORS[row2 / block_split][col2 / block_split] = SelectRefWinner(MDIFF_VECTORS[row2 / block_split][col2 / block_split], MDIFF_VECTOR_4S[row2 / block_split][col2 / block_split], REF_FRAME_2DS, REF_FRAME_2D_4S, block_split, row2, col2);
								}
								//Multireference code end
							}

							// RESIDUAL 
							GenerateResidualBlock(ENC_RES_FRAME_2DS, CUR_FRAME_2DS, REF_FRAME_2DS, row2, col2, block_split);

							// DCT 
							DCTBlock(ENC_TC_FRAME_2DS, ENC_RES_FRAME_2DS, row2, col2, block_split);

							// QUANTIZE
							QuantizeBlock(QTC_FRAME_2DS, ENC_TC_FRAME_2DS, QP_FRAME_2DS, row2, col2, width, height, QP, block_split);

							// SCALE
							ScaleBlock(DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS, row2, col2, width, height, QP, block_split);

							// IDCT
							IDCTBlock(DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, row2, col2, block_split);

							// RECONSTRUCT 
							ReconstructBlock(REC_FRAME_2DS, DEC_RES_FRAME_2DS, REF_FRAME_2DS, row2, col2, block_split);
						}
					}

				}//End of VBSenable code
				
			}
		}

		// Differential and Entropy Encode steps can be done on a whole frame here
		entropy_wrapper(QTC_FRAME_2D, block, height, width, frame);
		diff_enc_wrapper(MDIFF_VECTOR, MDIFF_VECTOR_DIFF, 0, height, width, block, frame);
		encode_mdiff_wrapper(MDIFF_VECTOR_DIFF, height, width, block, frame, 0);

		// File Dumps
		uint8_t *REC_FRAME = new uint8_t[FRAME_SIZE];
		for (int row = 0; row < height; row++)
			for (int col = 0; col < width; col++)
				REC_FRAME[col + width*row] = REC_FRAME_2D[row][col];

		fclose(recfile);//Weird behavior need to close file
		recfile = fopen(recfile_name, "a+b");
		fwrite(REC_FRAME, sizeof(uint8_t), FRAME_SIZE, recfile);

	}

	

	// Deallocate Memory
	for (unsigned int row = 0; row < height; row++) {
		delete		CUR_FRAME_2D[row];
		delete		REC_FRAME_2D[row]; 
		delete		REC_FRAME_2D_2[row];
		delete		REC_FRAME_2D_3[row];
		delete		REC_FRAME_2D_4[row];

		delete		REF_FRAME_2D[row];
		delete		REF_FRAME_2D_2[row];
		delete		REF_FRAME_2D_3[row];
		delete		REF_FRAME_2D_4[row];

		delete	ENC_RES_FRAME_2D[row]; 
		delete   ENC_TC_FRAME_2D[row];
		delete	DEC_RES_FRAME_2D[row];
		delete   DEC_TC_FRAME_2D[row];
		delete		QTC_FRAME_2D[row];
		delete		 QP_FRAME_2D[row];
	}

	delete CUR_FRAME_2D;
	delete REC_FRAME_2D;
	delete REC_FRAME_2D_2;
	delete REC_FRAME_2D_3;
	delete REC_FRAME_2D_4;

	delete REF_FRAME_2D;
	delete REF_FRAME_2D_2;
	delete REF_FRAME_2D_3;
	delete REF_FRAME_2D_4;

	delete ENC_RES_FRAME_2D;
	delete  ENC_TC_FRAME_2D;
	delete DEC_RES_FRAME_2D;
	delete  DEC_TC_FRAME_2D;
	delete QTC_FRAME_2D;
	delete  QP_FRAME_2D;
	
	for (unsigned int row = 0; row < (height / block); row++) {
		delete MDIFF_VECTOR[row];
		delete MDIFF_VECTOR_2[row];
		delete MDIFF_VECTOR_3[row];
		delete MDIFF_VECTOR_4[row];

	}

	delete MDIFF_VECTOR;
	delete MDIFF_VECTOR_2;
	delete MDIFF_VECTOR_3;
	delete MDIFF_VECTOR_4;


	// Close Files
	fclose(recfile);
	fclose(mdiff_bitcount_file);
	fclose(coeff_bitcount_file);
	return 0;

}

/*
----------------------
IRFAN TEST CODE START
----------------------

width		= 8;
height		= 8;
frames		= 1;
range		= 1;
block		= 2;
i_period	= 1;
QP			= 0;

----------------------
IRFAN TEST CODE END
----------------------
*/

/*
----------------------
IRFAN TEST CODE START
----------------------

char temp = 0;
for (int row = 0; row < 8; row++) {
for (int col = 0; col < 8; col++) {
CUR_FRAME_2D[row][col] = temp;
temp++;
temp = (temp % 32);
}

}


----------------------
IRFAN TEST CODE END
----------------------
*/

/*
----------------------
IRFAN TEST CODE START
----------------------


FILE *fp = fopen("INTRA.csv", "w");
fprintf(fp, "CUR FRAME\n");
for (int i = 0; i< height; i++) {
fprintf(fp, "%d", CUR_FRAME_2D[i][0]);
for (int j = 1; j < width; j++) {
fprintf(fp, ",%d", CUR_FRAME_2D[i][j]);
}
fprintf(fp, "\n");
}
fprintf(fp, "\n");
fprintf(fp, "REF FRAME\n");
for (int i = 0; i< height; i++) {
fprintf(fp, "%d", REF_FRAME_2D[i][0]);
for (int j = 1; j < width; j++) {
fprintf(fp, ",%d", REF_FRAME_2D[i][j]);
}
fprintf(fp, "\n");
}

fprintf(fp, "\n");
fprintf(fp, "ENC RES FRAME\n");
for (int i = 0; i< height; i++) {
fprintf(fp, "%d", ENC_RES_FRAME_2D[i][0]);
for (int j = 1; j < width; j++) {
fprintf(fp, ",%d", ENC_RES_FRAME_2D[i][j]);
}
fprintf(fp, "\n");
}

fprintf(fp, "\n");
fprintf(fp, "ENC TC FRAME\n");
for (int i = 0; i< height; i++) {
fprintf(fp, "%d", ENC_TC_FRAME_2D[i][0]);
for (int j = 1; j < width; j++) {
fprintf(fp, ",%d", ENC_TC_FRAME_2D[i][j]);
}
fprintf(fp, "\n");
}

fprintf(fp, "\n");
fprintf(fp, "QP FRAME\n");
for (int i = 0; i< height; i++) {
fprintf(fp, "%d", QP_FRAME_2D[i][0]);
for (int j = 1; j < width; j++) {
fprintf(fp, ",%d", QP_FRAME_2D[i][j]);
}
fprintf(fp, "\n");
}


fprintf(fp, "\n");
fprintf(fp, "QUANTIZED FRAME\n");
for (int i = 0; i< height; i++) {
fprintf(fp, "%d", QTC_FRAME_2D[i][0]);
for (int j = 1; j < width; j++) {
fprintf(fp, ",%d", QTC_FRAME_2D[i][j]);
}
fprintf(fp, "\n");
}

fprintf(fp, "\n");
fprintf(fp, "DEC TC FRAME\n");
for (int i = 0; i< height; i++) {
fprintf(fp, "%d", DEC_TC_FRAME_2D[i][0]);
for (int j = 1; j < width; j++) {
fprintf(fp, ",%d", DEC_TC_FRAME_2D[i][j]);
}
fprintf(fp, "\n");
}

fprintf(fp, "\n");
fprintf(fp, "DEC RES FRAME\n");
for (int i = 0; i< height; i++) {
fprintf(fp, "%d", DEC_RES_FRAME_2D[i][0]);
for (int j = 1; j < width; j++) {
fprintf(fp, ",%d", DEC_RES_FRAME_2D[i][j]);
}
fprintf(fp, "\n");
}

fprintf(fp, "\n");
fprintf(fp, "REC FRAME\n");
for (int i = 0; i< height; i++) {
fprintf(fp, "%d", REC_FRAME_2D[i][0]);
for (int j = 1; j < width; j++) {
fprintf(fp, ",%d", REC_FRAME_2D[i][j]);
}
fprintf(fp, "\n");
}
fprintf(fp, "\n");
fclose(fp);



----------------------
IRFAN TEST CODE END
----------------------
*/

/*
int GMV_X = MDIFF_VECTOR[((row*width / block) / block) + (col / block)].X;
int GMV_Y = MDIFF_VECTOR[((row*width / block) / block) + (col / block)].Y;

// Fill the Match Frame with the best matching block
for (int i = 0; i < block; i++) {
for (int j = 0; j < block; j++) {
MATCH_FRAME[row + i][col + j] = REC_FRAME[(row + GMV_Y + i) * width + (col + GMV_X + j)];
}
}
*/
/*
----------------------
IRFAN TEST CODE START
----------------------

for (int row = 0; row < height; row++) {
for (int col = 0; col < width; col++) {
ENC_TC_FRAME_2D[row][col] = ENC_RES_FRAME_2D[row][col];
}
}

----------------------
IRFAN TEST CODE END
----------------------
*/

/*
----------------------
IRFAN TEST CODE START
----------------------

for (int row = 0; row < height; row++) {
for (int col = 0; col < width; col++) {
DEC_RES_FRAME_2D[row][col] = DEC_TC_FRAME_2D[row][col];
}
}

----------------------
IRFAN TEST CODE END
----------------------
*/
/*
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
*/

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