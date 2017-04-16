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

void VBSMDIFFcopy( MDIFF** MDIFF_SPLIT, MDIFF** MDIFF_CUR, int row, int col, int block) {
	int row_split = row / (block / 2);
	int col_split = col / (block / 2);
	int row_org = row / block;
	int col_org = col / block;

	MDIFF_SPLIT[row_split][col_split].X = MDIFF_CUR[row_org][col_org].X;
	MDIFF_SPLIT[row_split][col_split].Y = MDIFF_CUR[row_org][col_org].Y;
	MDIFF_SPLIT[row_split][col_split].MODE = MDIFF_CUR[row_org][col_org].MODE;
	MDIFF_SPLIT[row_split][col_split].ref = MDIFF_CUR[row_org][col_org].ref;

	MDIFF_SPLIT[row_split][col_split].X = MDIFF_CUR[row_org][col_org].X2;
	MDIFF_SPLIT[row_split][col_split].Y = MDIFF_CUR[row_org][col_org].Y2;
	MDIFF_SPLIT[row_split][col_split].MODE = MDIFF_CUR[row_org][col_org].MODE2;
	MDIFF_SPLIT[row_split][col_split].ref = MDIFF_CUR[row_org][col_org].ref2;
	
	MDIFF_SPLIT[row_split][col_split].X = MDIFF_CUR[row_org][col_org].X3;
	MDIFF_SPLIT[row_split][col_split].Y = MDIFF_CUR[row_org][col_org].Y3;
	MDIFF_SPLIT[row_split][col_split].MODE = MDIFF_CUR[row_org][col_org].MODE3;
	MDIFF_SPLIT[row_split][col_split].ref = MDIFF_CUR[row_org][col_org].ref3;
	
	MDIFF_SPLIT[row_split][col_split].X = MDIFF_CUR[row_org][col_org].X4;
	MDIFF_SPLIT[row_split][col_split].Y = MDIFF_CUR[row_org][col_org].Y4;
	MDIFF_SPLIT[row_split][col_split].MODE = MDIFF_CUR[row_org][col_org].MODE4;
	MDIFF_SPLIT[row_split][col_split].ref = MDIFF_CUR[row_org][col_org].ref4;

}

void DecodeVBS(int FrameType, int row, int col, int block, int width, int height, int QP, 
	MDIFF **MDIFF_VECTORS, MDIFF **MDIFF_VECTOR, uint8_t **DEC_FRAME_2DS, uint8_t **REF_FRAME_2DS, uint8_t **PREV_DEC_FRAME,
	int32_t **QTC_FRAME_2DS, int32_t **QP_FRAME_2DS, int32_t **DEC_RES_FRAME_2DS, int32_t **DEC_TC_FRAME_2DS,
	int block_split)
{
	int iteration;
	for (int row2 = row; row2 < row + block; row2 += block_split) {
		for (int col2 = col; col2 < col + block; col2 += block_split) {

			VBSMDIFFcopy(MDIFF_VECTORS, MDIFF_VECTOR, row2, col2, block);
			// SCALE
			ScaleBlock(DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS, row2, col2, width, height, QP, block_split);

			// IDCT
			IDCTBlock(DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, row2, col2, block_split);

			if (FrameType == PFRAME) {
				// RECONSTRUCT 
				ReconstructBlockDecodeP(DEC_FRAME_2DS, DEC_RES_FRAME_2DS, PREV_DEC_FRAME, REF_FRAME_2DS, row2, col2, block_split, MDIFF_VECTORS);
			}
			else if (FrameType == IFRAME) {
				// RECONSTRUCT 
				ReconstructBlockDecodeI(DEC_FRAME_2DS, DEC_RES_FRAME_2DS, REF_FRAME_2DS, row2, col2, block_split, MDIFF_VECTORS);
			}
		}
	}
}

void VBSDecodeCopy(uint8_t **DEC_FRAME_2D, uint8_t **DEC_FRAME_2DS, int row, int col, int block, int32_t **QTC_FRAME_2D, int32_t **QTC_FRAME_2DS)
{
	int row_org = row / block;
	int col_org = col / block;
	for (int row = row_org; row < row_org + block; row++) {
		for (int col = col_org; col < col_org + block; col++) {
			DEC_FRAME_2D[row][col] = DEC_FRAME_2DS[row][col];
			QTC_FRAME_2D[row][col] = QTC_FRAME_2DS[row][col];
		}
	}
}

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
		else if (!strcmp((*args) + 1, "VBSEnable")) {
			args++;
			VBSEnable = atoi(*args);
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
	FILE* qtcfile = fopen("dec_qtc.csv", "w");
	FILE* qtcfile2 = fopen("dec_qtc2.csv", "w");

	frame_header_file = fopen(frame_header_name, "rb");
	//coeff_bitcount_file = fopen(coeff_bitcount_name, "r");
	//mdiff_bitcount_file = fopen(mdiff_bitcount_name, "r");
	uint32_t sz;

	//Read the frame header once to get frame paramters
	fread(&FrameType, sizeof(int32_t), 1, frame_header_file);
	fread(&block, sizeof(int32_t), 1, frame_header_file);
	fread(&width, sizeof(int32_t), 1, frame_header_file);
	fread(&height, sizeof(int32_t), 1, frame_header_file);

	int block_split = block / 2;

	unsigned int  FRAME_SIZE = width*height;
	//reset to begining of file
	fseek(frame_header_file, 0, SEEK_SET);

	// Allocate Memory
	uint8_t** DEC_FRAME_2D = new uint8_t*[height];
	uint8_t** DEC_FRAME_2DS = new uint8_t*[height];
	uint8_t** REF_FRAME_2D = new uint8_t*[height];
	uint8_t** REF_FRAME_2DS = new uint8_t*[height];
	uint8_t** PREV_DEC_FRAME = new uint8_t*[height];
	uint8_t** PREV_DEC_FRAME_2DS = new uint8_t*[height];
	int32_t** ENC_RES_FRAME_2D = new  int32_t*[height];
	int32_t** ENC_TC_FRAME_2D = new int32_t*[height];
	int32_t** DEC_RES_FRAME_2D = new  int32_t*[height];
	int32_t** DEC_RES_FRAME_2DS = new  int32_t*[height];
	int32_t** DEC_TC_FRAME_2D = new int32_t*[height];
	int32_t** DEC_TC_FRAME_2DS = new int32_t*[height];
	int32_t** QTC_FRAME_2D = new int32_t*[height];	
	int32_t** QTC_FRAME_2DS = new int32_t*[height];
	int32_t** QP_FRAME_2D = new int32_t*[height];
	int32_t** QP_FRAME_2DS = new int32_t*[height];

	for (unsigned int row = 0; row < height; row++) {
		DEC_FRAME_2D[row] = new uint8_t[width];
		DEC_FRAME_2DS[row] = new uint8_t[width];
		REF_FRAME_2D[row] = new uint8_t[width];
		REF_FRAME_2DS[row] = new uint8_t[width];
		PREV_DEC_FRAME[row] = new uint8_t[width];
		PREV_DEC_FRAME_2DS[row] = new uint8_t[width];
		ENC_RES_FRAME_2D[row] = new  int32_t[width];
		ENC_TC_FRAME_2D[row] = new int32_t[width];
		DEC_RES_FRAME_2D[row] = new  int32_t[width];
		DEC_RES_FRAME_2DS[row] = new  int32_t[width];
		DEC_TC_FRAME_2D[row] = new int32_t[width];
		DEC_TC_FRAME_2DS[row] = new int32_t[width];
		QTC_FRAME_2D[row] = new int32_t[width];
		QTC_FRAME_2DS[row] = new int32_t[width];
		QP_FRAME_2D[row] = new int32_t[width];
		QP_FRAME_2DS[row] = new int32_t[width];
	}


	// This 2D Buffer will Contain MDIFF data for each block 
	struct MDIFF** MDIFF_VECTOR = new struct MDIFF*[(height / block)];
	struct MDIFF** MDIFF_VECTOR_DIFF = new struct MDIFF*[(height / block)];

	for (int row = 0; row < height; row = row + block) {
		MDIFF_VECTOR[row / block] = new struct MDIFF[width / block];
		MDIFF_VECTOR_DIFF[row / block] = new struct MDIFF[width / block];
	}

	struct MDIFF** MDIFF_VECTORS = new struct MDIFF*[(height / block_split)];
	struct MDIFF** MDIFF_VECTOR_DIFFS = new struct MDIFF*[(height / block_split)];
	for (int row = 0; row < height; row = row + block_split) {
		MDIFF_VECTORS[row / block_split] = new struct MDIFF[width / block_split];
		MDIFF_VECTOR_DIFFS[row / block_split] = new struct MDIFF[width / block_split];
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
			if (VBSEnable) {
				fseek(decfile, (frame - 1)*FRAME_SIZE, SEEK_SET);
				for (unsigned int row = 0; row < height; row++) {
					fread(PREV_DEC_FRAME_2DS[row], sizeof(uint8_t), width, decfile);
				}
			}

		}
		
		
		reverse_entropy(QTC_FRAME_2D, block, height, width, frame);
		write_mat3(qtcfile, QTC_FRAME_2D, height, width);
		decode_mdiff_wrapper(MDIFF_VECTOR_DIFF, height, width, block, frame, FrameType); //x,y or intramode
		diff_dec_wrapper(MDIFF_VECTOR, MDIFF_VECTOR_DIFF, FrameType, height, width, block, frame);

		// Apply Decode Operations on Each Block
		for (int row = 0; row < height; row += block) {
			for (int col = 0; col < width; col += block) {
				// IDEALLY THREAD EVERYTHING IN THIS FOR LOOP FOR PFRAMES
				if (MDIFF_VECTOR[row / block][col / block].split == 0) {
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
				else {
					//VBSMDIFFcopy(MDIFF_VECTORS, MDIFF_VECTOR, row, col, block);
					DecodeVBS(FrameType, row, col, block, width, height, QP, MDIFF_VECTORS, MDIFF_VECTOR, DEC_FRAME_2D, REF_FRAME_2D, PREV_DEC_FRAME,
						QTC_FRAME_2D, QP_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D,
						block_split);
					//VBSDecodeCopy(DEC_FRAME_2D, DEC_FRAME_2DS, row, col, block, QTC_FRAME_2D, QTC_FRAME_2DS);
				}
			}
		}

		
		write_mat(reffile, REF_FRAME_2D, height, width);
		write_mat3(decresfile, DEC_RES_FRAME_2D, height, width);
		write_mat2(dectcfile, DEC_TC_FRAME_2D, height, width);
		write_mat3(qtcfile2, QTC_FRAME_2D, height, width);
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