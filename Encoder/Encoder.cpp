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
#include <RateControl.h>
#include <ctime>


void Encode(int FrameType, int row, int col, int block, int nRefFrames, uint32_t INTERMODE, int frame, int i_period, int width, int height, int QP,
	MDIFF **MDIFF_VECTOR, MDIFF **MDIFF_VECTOR_2, MDIFF **MDIFF_VECTOR_3, MDIFF **MDIFF_VECTOR_4, uint8_t **CUR_FRAME_2D,
	uint8_t **REC_FRAME_2D, uint8_t **REC_FRAME_2D_2, uint8_t **REC_FRAME_2D_3, uint8_t **REC_FRAME_2D_4,
	uint8_t **REF_FRAME_2D, uint8_t **REF_FRAME_2D_2, uint8_t **REF_FRAME_2D_3, uint8_t **REF_FRAME_2D_4,
	int32_t **ENC_RES_FRAME_2D, int32_t **QTC_FRAME_2D, int32_t **QP_FRAME_2D, int32_t **ENC_TC_FRAME_2D, int32_t **DEC_RES_FRAME_2D, int32_t **DEC_TC_FRAME_2D,
	int range, int RDOEnable)
{
	// PREDICTOR DATA GENERATION
	if (FrameType == IFRAME) {
		MDIFF_VECTOR[row / block][col / block] = IntraFramePrediction(CUR_FRAME_2D, REC_FRAME_2D, REF_FRAME_2D, row, col, block);
	}

	if (FrameType == PFRAME) {
		MDIFF_VECTOR[row / block][col / block] = InterFramePrediction(INTERMODE, CUR_FRAME_2D, REC_FRAME_2D, REF_FRAME_2D, row, col, width, height, block, range, 1, MDIFF_VECTOR);
		//Multireference code start Only activated if nRefFrames>=2
		if (nRefFrames >= 2) {
			MDIFF_VECTOR[row / block][col / block] = MultiRefInterPrediction(INTERMODE, CUR_FRAME_2D, REC_FRAME_2D_2, REC_FRAME_2D_3, REC_FRAME_2D_4,
				REF_FRAME_2D, REF_FRAME_2D_2, REF_FRAME_2D_3, REF_FRAME_2D_4,
				row, col, width, height, block, range, nRefFrames, frame, i_period,
				MDIFF_VECTOR, MDIFF_VECTOR_2, MDIFF_VECTOR_3, MDIFF_VECTOR_4);
		}
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
void EncodeVBS(int FrameType, int row, int col, int block, int nRefFrames, uint32_t INTERMODE, int frame, int i_period, int width, int height, int QP,
	MDIFF **MDIFF_VECTORS, MDIFF **MDIFF_VECTOR_2S, MDIFF **MDIFF_VECTOR_3S, MDIFF **MDIFF_VECTOR_4S, uint8_t **CUR_FRAME_2DS,
	uint8_t **REC_FRAME_2DS, uint8_t **REC_FRAME_2D_2S, uint8_t **REC_FRAME_2D_3S, uint8_t **REC_FRAME_2D_4S,
	uint8_t **REF_FRAME_2DS, uint8_t **REF_FRAME_2D_2S, uint8_t **REF_FRAME_2D_3S, uint8_t **REF_FRAME_2D_4S,
	int32_t **ENC_RES_FRAME_2DS, int32_t **QTC_FRAME_2DS, int32_t **QP_FRAME_2DS, int32_t **ENC_TC_FRAME_2DS, int32_t **DEC_RES_FRAME_2DS, int32_t **DEC_TC_FRAME_2DS,
	int block_split, int range, int RDOEnable)
{
	for (int row2 = row; row2 < row + block; row2 += block_split) {
		for (int col2 = col; col2 < col + block; col2 += block_split) {

			// IDEALLY THREAD EVERYTHING IN THIS FOR LOOP FOR PFRAMES

			// PREDICTOR DATA GENERATION
			if (FrameType == IFRAME) {
				MDIFF_VECTORS[row2 / block_split][col2 / block_split] = IntraFramePrediction(CUR_FRAME_2DS, REC_FRAME_2DS, REF_FRAME_2DS, row2, col2, block_split);
			}

			if (FrameType == PFRAME) {
				MDIFF_VECTORS[row2 / block_split][col2 / block_split] = InterFramePrediction(INTERMODE, CUR_FRAME_2DS, REC_FRAME_2DS, REF_FRAME_2DS, row2, col2, width, height, block_split, range, 1, MDIFF_VECTORS);
				//Multireference code start Only activated if nRefFrames>=2
				if (nRefFrames >= 2) {
					MDIFF_VECTORS[row / block][col / block] = MultiRefInterPrediction(INTERMODE, CUR_FRAME_2DS, REC_FRAME_2D_2S, REC_FRAME_2D_3S, REC_FRAME_2D_4S,
						REF_FRAME_2DS, REF_FRAME_2D_2S, REF_FRAME_2D_3S, REF_FRAME_2D_4S,
						row, col, width, height, block, range, QP, RDOEnable, nRefFrames,
						MDIFF_VECTORS, MDIFF_VECTOR_2S, MDIFF_VECTOR_3S, MDIFF_VECTOR_4S);
				}
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
}


int main(int argCnt, char **args)
{

	char curfile_name[500]			= "";
	char mvfile_name[500]			= "";
	char resfile_name[500]			= "";
	char recfile_name[500]			= "";
	char matchfile_name[500]		= "";
	char bitcount_row_name[500] = "";
	char coeff_bitcount_name[500]	= "";
	char mdiff_bitcount_name[500]	= "";
	char total_bitcount_name[500] = "";
	char frame_header_name[500]		= "";
	char runtime_name[500]			= "";

	int width			= -1;
	int height			= -1;
	int frames			= -1;
	int range			= -1;
	int block			= -1;
	int i_period		= -1;
	int FrameType		= -1;
	int QP				= -1;
	int nRefFrames		= 1;
	int RDOEnable		= 0;
	int FMEnable		= 0;
	int coeff_bitcount	= 0;
	int mdiff_bitcount	= 0;
	int bitcount_row	 = 0;
	int targetBr = 0;
	int	RCflag = 0;
	int SceneChange = 0;


	//  Parse Input Arguments
	// =======================

	args++;
	int tmpArgCnt = 1;

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
		else if (!strcmp((*args) + 1, "total_bitcount_name")) {
			args++;
			sscanf(*args, "%s", total_bitcount_name);
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
		else if (!strcmp((*args) + 1, "bitcount_row")) {
			args++;
			sscanf(*args, "%s", bitcount_row_name);
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
		else if (!strcmp((*args) + 1, "RDOEnable")) {
			args++;
			RDOEnable = atoi(*args);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "FMEnable")) {
			args++;
			FMEnable = atoi(*args);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "runtime_name")) {
			args++;
			sscanf(*args, "%s", runtime_name);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "targetBr")) {
			args++;
			targetBr = atoi(*args);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "RCflag")) {
			args++;
			RCflag = atoi(*args);
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
	total_bitcount_file = fopen(total_bitcount_name, "w");
	frame_header_file = fopen(frame_header_name, "w+b");
	FILE* recfile = fopen(recfile_name, "w+b");
	FILE* runtime_file = fopen(runtime_name, "w");

	FILE* reffile = fopen("ref_enc.csv", "w");
	FILE* dectcfile = fopen("dec_tc_enc.csv", "w");
	FILE* decresfile = fopen("dec_res_enc.csv", "w");
	FILE* bitcountrowfile = fopen(bitcount_row_name, "w");

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

	 int32_t** ENC_RES_FRAME_2D	= new  int32_t*[height];
	int32_t** ENC_TC_FRAME_2D	= new int32_t*[height];
	 int32_t** DEC_RES_FRAME_2D	= new  int32_t*[height];
	int32_t** DEC_TC_FRAME_2D	= new int32_t*[height];
	int32_t** QTC_FRAME_2D		= new int32_t*[height];
	int32_t** QP_FRAME_2D		= new int32_t*[height];

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

		ENC_RES_FRAME_2D[row] = new  int32_t[width];
		ENC_TC_FRAME_2D[row] = new int32_t[width];
		DEC_RES_FRAME_2D[row] = new  int32_t[width];
		DEC_TC_FRAME_2D[row] = new int32_t[width];
		QTC_FRAME_2D[row] = new int32_t[width];
		QP_FRAME_2D[row] = new int32_t[width];
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

	int32_t** ENC_RES_FRAME_2DS = new  int32_t*[height];
	int32_t** ENC_TC_FRAME_2DS = new int32_t*[height];
	int32_t** DEC_RES_FRAME_2DS = new  int32_t*[height];
	int32_t** DEC_TC_FRAME_2DS = new int32_t*[height];
	int32_t** QTC_FRAME_2DS = new int32_t*[height];
	int32_t** QP_FRAME_2DS = new int32_t*[height];

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

		ENC_RES_FRAME_2DS[row] = new  int32_t[width];
		ENC_TC_FRAME_2DS[row] = new int32_t[width];
		DEC_RES_FRAME_2DS[row] = new  int32_t[width];
		DEC_TC_FRAME_2DS[row] = new int32_t[width];
		QTC_FRAME_2DS[row] = new int32_t[width];
		QP_FRAME_2DS[row] = new int32_t[width];
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

	//Bitcount per row
	uint32_t *BITCOUNT_ROW = new uint32_t[height / block];
	double *BITCOUNT_ROW_PERCENT = new double[height / block];
	int *QP_ROW = new int[height / block];

	// PICK INTERFRAME ALGORITHM
	uint32_t INTERMODE;
	if ((RDOEnable) && (QP >= 8)) {
		INTERMODE = RDO;
		printf("RDO ENABLED\n");
	}
	else if (FMEnable) {
		INTERMODE = FME;
		printf("FME ENABLED\n");
	}
	else {
		INTERMODE = DEFAULT;
		printf("DEFAULT INTERMODE\n");
	}

	//Runtime
	int start_s = clock();

	FILE* qp_file = fopen("testdata\\QP_FILE.csv", "w");
	// Encode Each Frame
	// =========================================
	for (int frame = 0; frame < frames; frame++) {
		//Open GOLOMB and MDIFF files
		snprintf(golomb_name, sizeof(golomb_name), "testdata\\COEFF_GOLOMB_CODING_%d", frame);
		snprintf(mdiff_name, sizeof(mdiff_name), "testdata\\MDIFF_GOLOMB_%d", frame);
		mdiff_golomb = fopen(mdiff_name, "wb");
		golomb_file = fopen(golomb_name, "wb");
#ifdef TRACE_ON
		char buf[0x100];
		snprintf(buf, sizeof(buf), "testdata\\MDIFF_ORG_ENC%d.txt", frame);
		file_vector_org = fopen(buf, "w");
		snprintf(buf, sizeof(buf), "testdata\\MDIFF_AFT_ENC%d.txt", frame);
		file_vector_aft = fopen(buf, "w");
		snprintf(buf, sizeof(buf), "testdata\\QTC_BLOCK_ENC_%d.txt", frame);
		file_qtc = fopen(buf, "w");
		snprintf(buf, sizeof(buf), "testdata\\REORDER_BLOCK_ENC_%d.txt", frame);
		file_reorder = fopen(buf, "w");
		snprintf(buf, sizeof(buf), "testdata\\RLE_BLOCK_ENC_%d.txt", frame);
		file_rle = fopen(buf, "w");
#endif // TRACE_ON

		//Reset bitcounts
		coeff_bitcount = 0;
		mdiff_bitcount = 0;
		
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
		for (int pass = 0; pass < 2; pass++) {
			if (pass == 1) {//Second pass set up
				fclose(mdiff_golomb);
				fclose(golomb_file);
				mdiff_golomb = fopen(mdiff_name, "wb");
				golomb_file = fopen(golomb_name, "wb");
				coeff_bitcount = 0;
				mdiff_bitcount = 0;
			}

			for (int row = 0; row < height; row += block) {
				if (row == 0 && RCflag == 1 || row == 0 && RCflag >= 2 && pass == 1) {
					QP = row_rate_control(row - block, targetBr, RCflag, width, height, FrameType, block, 0, QP, BITCOUNT_ROW, BITCOUNT_ROW_PERCENT, pass);//TODO
					QP_ROW[0] = QP;
				}
				for (int col = 0; col < width; col += block) {
					Encode(FrameType, row, col, block, nRefFrames, INTERMODE, frame, i_period, width, height, QP,
						MDIFF_VECTOR, MDIFF_VECTOR_2, MDIFF_VECTOR_3, MDIFF_VECTOR_4,
						CUR_FRAME_2D, REC_FRAME_2D, REC_FRAME_2D_2, REC_FRAME_2D_3, REC_FRAME_2D_4,
						REF_FRAME_2D, REF_FRAME_2D_2, REF_FRAME_2D_3, REF_FRAME_2D_4,
						ENC_RES_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D,
						range, RDOEnable);

					if (VBSEnable) {//start VBSEnable code
						EncodeVBS(FrameType, row, col, block, nRefFrames, INTERMODE, frame, i_period, width, height, QP,
							MDIFF_VECTORS, MDIFF_VECTOR_2S, MDIFF_VECTOR_3S, MDIFF_VECTOR_4S, CUR_FRAME_2DS,
							REC_FRAME_2DS, REC_FRAME_2D_2S, REC_FRAME_2D_3S, REC_FRAME_2D_4S,
							REF_FRAME_2DS, REF_FRAME_2D_2S, REF_FRAME_2D_3S, REF_FRAME_2D_4S,
							ENC_RES_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS,
							block_split, range, RDOEnable);
						//Pick Winner
						VBSWinner(MDIFF_VECTOR, MDIFF_VECTORS, row, col, block, REC_FRAME_2D, REC_FRAME_2DS);
					}//End of VBSenable code
					 // Differential and Entropy Encode steps 
					int bitcount_temp = 0;
					bitcount_temp = entropy_wrapper(QTC_FRAME_2D, block, height, width, frame, row, col);
					coeff_bitcount = coeff_bitcount + bitcount_temp;
					bitcount_row = bitcount_row + bitcount_temp;
					diff_enc_wrapper(MDIFF_VECTOR, MDIFF_VECTOR_DIFF, FrameType, height, width, block, frame, row, col);
					bitcount_temp = encode_mdiff_wrapper(MDIFF_VECTOR_DIFF, MDIFF_VECTOR, height, width, block, frame, FrameType, row, col);
					mdiff_bitcount = mdiff_bitcount + bitcount_temp;
					bitcount_row = bitcount_row + bitcount_temp;
					if (col + block == width) {//Collect bitcount per row
						BITCOUNT_ROW[row / block] = bitcount_row;
						if (RCflag >= 1) {
							QP = row_rate_control(row, targetBr, RCflag, width, height, FrameType, block, coeff_bitcount + mdiff_bitcount, QP, BITCOUNT_ROW, BITCOUNT_ROW_PERCENT, pass);
							if(row+block != height)
								QP_ROW[(row +block) / block] = QP;
						}
						bitcount_row = 0;
					}
				}
			}//Encode
			if (RCflag <= 1) {
				break;
			}
		}
		//write_mat(reffile, REF_FRAME_2D, height, width);
		//write_mat3(decresfile, DEC_RES_FRAME_2D, height, width);
		//write_mat2(dectcfile, DEC_TC_FRAME_2D, height, width);

		// File Dumps
		//RECON
		uint8_t *REC_FRAME = new uint8_t[FRAME_SIZE];
		for (int row = 0; row < height; row++)
			for (int col = 0; col < width; col++)
				REC_FRAME[col + width*row] = REC_FRAME_2D[row][col];

		fclose(recfile);//Weird behavior need to close file
		recfile = fopen(recfile_name, "a+b");
		fwrite(REC_FRAME, sizeof(uint8_t), FRAME_SIZE, recfile);
		//Bitcount Per frame
		fprintf(coeff_bitcount_file, "%d,%d\n", frame, coeff_bitcount);
		fprintf(mdiff_bitcount_file, "%d,%d\n", frame, mdiff_bitcount);
		fprintf(total_bitcount_file, "%d,%d\n", frame, coeff_bitcount + mdiff_bitcount);
		fclose(mdiff_golomb);
		fclose(golomb_file);
		//Bitcount Per row
		int average=0;
		for (int i = 0; i < height; i = i + block) {
			average += BITCOUNT_ROW[i / block];
		}
		if (FrameType == IFRAME)
			fprintf(bitcountrowfile, "I_FRAME,%d,%d\n",frame, average / (height / block));
		else
			fprintf(bitcountrowfile, "P_FRAME,%d,%d\n",frame, average / (height / block));
		//Rate Control Dump
		for (int i = 0; i < height / block; i++) {
			if (i == 0)
				fprintf(qp_file, "Frame: %d ", frame);
			fprintf(qp_file, "%d ", QP_ROW[i]);
		}
		fprintf(qp_file, "\n");

#ifdef TRACE_ON
		fclose(file_vector_org);
		fclose(file_vector_aft);
		fclose(file_qtc);
		fclose(file_reorder);
		fclose(file_rle);
#endif 

	}//Frame
	//Runtime
	int stop_s = clock();
	fprintf(runtime_file,"%.2f\n", (double)(clock() - start_s) / CLOCKS_PER_SEC);
		
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
	fclose(total_bitcount_file);
	fclose(reffile);
	fclose(decresfile);
	fclose(dectcfile);
	return 0;

}