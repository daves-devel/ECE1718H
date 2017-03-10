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
	//FILE* mvfile = fopen(mvfile_name, "w");
	//FILE* resfile = fopen(resfile_name, "wb");
	FILE* recfile = fopen(recfile_name, "w+b");
	//FILE* matchfile = fopen(matchfile_name, "wb");
	//FILE* gmvXfile = fopen(gmvx_name, "wb");
	//FILE* gmvYfile = fopen(gmvy_name, "wb");

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

	// TODO Make these 2D buffers, and add the Encoder functions to the frame flow
	unsigned int  FRAME_SIZE = width*height;
	signed char* COEFF_REORDER = new signed char[FRAME_SIZE];
	signed char* RLE = new signed char[FRAME_SIZE];
	/*
	----------------------
	IRFAN TEST CODE START
	----------------------
	*/
	width		= 8;
	height		= 8;
	frames		= 1;
	range		= 1;
	block		= 8;
	round		= 1;
	i_period	= 1;
	QP			= 0;
	/*
	----------------------
	IRFAN TEST CODE END
	----------------------
	*/
	// Allocate Memory
	uint8_t** CUR_FRAME_2D		= new uint8_t*[height];
	uint8_t** REC_FRAME_2D		= new uint8_t*[height];
	uint8_t** REF_FRAME_2D		= new uint8_t*[height];
	 int8_t** ENC_RES_FRAME_2D	= new  int8_t*[height];
	int32_t** ENC_TC_FRAME_2D	= new int32_t*[height];
	 int8_t** DEC_RES_FRAME_2D	= new  int8_t*[height];
	int32_t** DEC_TC_FRAME_2D	= new int32_t*[height];
	int32_t** QTC_FRAME_2D		= new int32_t*[height];
	uint8_t** QP_FRAME_2D		= new uint8_t*[height];

	for (unsigned int row = 0; row < height; row++) {
		CUR_FRAME_2D[row] = new uint8_t[width];
		REC_FRAME_2D[row] = new uint8_t[width];
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

	/* 
	----------------------
	IRFAN TEST CODE START
	----------------------
	*/
	char temp = 0;
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			CUR_FRAME_2D[row][col] = temp;
			temp++;
			temp = (temp % 32);
		}
		
	}

	/*
	----------------------
	IRFAN TEST CODE END
	----------------------
	*/

	// Encode Each Frame
	// =========================================
	for (int frame = 0; frame < frames; frame++) {

		if ((frame%i_period) == 0) { 
			FrameType = IFRAME;
		}
		else {
			FrameType = PFRAME;
		}

		if (FrameType == PFRAME){
			// Go to the beginning of the previous reconstructed frame and copy it to buffer
			fseek(recfile, (frame - 1)*FRAME_SIZE, SEEK_SET);
			for (unsigned int row = 0; row++; row < height) {
				fread(REC_FRAME_2D[row], sizeof(uint8_t), width, recfile);
			}
		}

		// Go to the beginning of the current frame and copy it to buffer

		/* IRFAN TEST COMMENT OUT
		fseek(curfile, frame*FRAME_SIZE, SEEK_SET);
		for (unsigned int row = 0; row++; row < height) {
			fread(CUR_FRAME_2D[row], sizeof(uint8_t), width, curfile);
		}
		*/

		// Apply Encode Operations on Each Block
		for (int row = 0; row < height; row += block) {
			for (int col = 0; col < width; col += block) {

				// IDEALLY THREAD EVERYTHING IN THIS FOR LOOP FOR PFRAMES

				// PREDICTOR DATA GENERATION
				if (FrameType == IFRAME) {
					MDIFF_VECTOR[row/block][col/block] = IntraFramePrediction (CUR_FRAME_2D,REC_FRAME_2D,REF_FRAME_2D,row,col,block);
				}
				
				if (FrameType == PFRAME) {
					MDIFF_VECTOR[row/block][col/block] = InterFramePrediction (CUR_FRAME_2D, REC_FRAME_2D, REF_FRAME_2D,row, col, width, height, block, range);
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
				
			}
		}



		
		// =====================================================================================================
		// TODOOOOOO
		// Differential and Entropy Encode steps can be done on a whole frame here
		// OR they can be done on a block level in the previous nested for loop after the Quantization Step.
		// =====================================================================================================

		entropy_wrapper(QTC_FRAME_2D, block, height, width, frame);
		diff_enc_wrapper(MDIFF_VECTOR, MDIFF_VECTOR_DIFF, 0, height, width, block, frame);
		encode_mdiff_wrapper(MDIFF_VECTOR_DIFF, height, width, block, frame, 0);

		// =====================================================================================================
		// TODOOOOO
		// Any File Dumps can be added on any 2D array here for verification purpose
		// =====================================================================================================

	}

	/*
	----------------------
	IRFAN TEST CODE START
	----------------------
	*/

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


	/*
	----------------------
	IRFAN TEST CODE END
	----------------------
	*/

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