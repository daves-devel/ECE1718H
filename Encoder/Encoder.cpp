// Authors:		Juan Fuentes
//				Irfan Khan (khanirf1) 999207665
//				David Chakkuthara 995435266
// Date:		April 7th, 2016
// Description: Given a Y (LUMA) only File, it will be encoded 
//				Outputs an encoded MDiff File
//				Outputs an encoded QTC Coeff. File

#include <common.h>
#include <blockthread.h>

int main(int argCnt, char **args){

	char curfile_name[500]			= "";
	char mvfile_name[500]			= "";
	char resfile_name[500]			= "";
	char recfile_name[500]			= "";
	char matchfile_name[500]		= "";
	char bitcount_row_name[500]		= "";
	char coeff_bitcount_name[500]	= "";
	char mdiff_bitcount_name[500]	= "";
	char total_bitcount_name[500]	= "";
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
	int RDOEnable		= -1;
	int FMEnable		= -1;
	int ParallelMode	= -1;
	int coeff_bitcount	= 0;
	int mdiff_bitcount	= 0;

	//  Parse Input Arguments
	// ================================================

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
		else if (!strcmp((*args) + 1, "ParallelMode")) {
			args++;
			ParallelMode = atoi(*args);
			args++;
			tmpArgCnt += 2;
		}
		else if (!strcmp((*args) + 1, "runtime_name")) {
			args++;
			sscanf(*args, "%s", runtime_name);
			args++;
			tmpArgCnt += 2;
		}

		else {
			printf("Huh? I don't know %s (option #%d) \n", *args, tmpArgCnt);
			exit(-1);
		}
	}

	// Files
	// ================================================
	FILE* curfile				= fopen(curfile_name, "rb");
	FILE* coeff_bitcount_file	= fopen(coeff_bitcount_name, "w");
	FILE* mdiff_bitcount_file	= fopen(mdiff_bitcount_name, "w");
	FILE* total_bitcount_file	= fopen(total_bitcount_name, "w");
	FILE* frame_header_file		= fopen(frame_header_name, "w+b");
	FILE* recfile				= fopen(recfile_name, "w+b");
	FILE* runtime_file			= fopen(runtime_name, "w");
	FILE* bitcountrowfile		= fopen(bitcount_row_name, "w");
	FILE* reffile				= fopen("ref_enc.csv", "w");
	FILE* dectcfile				= fopen("dec_tc_enc.csv", "w");
	FILE* decresfile			= fopen("dec_res_enc.csv", "w");

	unsigned int  FRAME_SIZE = width*height;
	int block_split = block / 2;
	int range_split = range / 2;
	time_t start_time;
	time_t stop_time;
	// Allocate Memory
	// ================================================

	// Regular
	uint8_t** CUR_FRAME_2D		= new uint8_t*[height];
	uint8_t** CUR_REC_FRAME_2D	= new uint8_t*[height];
	uint8_t** PREV_REC_FRAME_2D = new uint8_t*[height];
	uint8_t** REF_FRAME_2D		= new uint8_t*[height];
	int32_t** ENC_RES_FRAME_2D	= new int32_t*[height];
	int32_t** ENC_TC_FRAME_2D	= new int32_t*[height];
	int32_t** DEC_RES_FRAME_2D	= new int32_t*[height];
	int32_t** DEC_TC_FRAME_2D	= new int32_t*[height];
	int32_t** QTC_FRAME_2D		= new int32_t*[height];
	int32_t** QP_FRAME_2D		= new int32_t*[height];

	// Split
	uint8_t** CUR_FRAME_2DS		= new uint8_t*[height];
	uint8_t** CUR_REC_FRAME_2DS = new uint8_t*[height];
	uint8_t** PREV_REC_FRAME_2DS= new uint8_t*[height];
	uint8_t** REF_FRAME_2DS		= new uint8_t*[height];
	int32_t** ENC_RES_FRAME_2DS = new int32_t*[height];
	int32_t** ENC_TC_FRAME_2DS	= new int32_t*[height];
	int32_t** DEC_RES_FRAME_2DS = new int32_t*[height];
	int32_t** DEC_TC_FRAME_2DS	= new int32_t*[height];
	int32_t** QTC_FRAME_2DS		= new int32_t*[height];
	int32_t** QP_FRAME_2DS		= new int32_t*[height];

	for (unsigned int row = 0; row < height; row++) {

		// Regular
		CUR_FRAME_2D[row]		= new uint8_t[width];
		CUR_REC_FRAME_2D[row]	= new uint8_t[width];
		PREV_REC_FRAME_2D[row]  = new uint8_t[width];
		REF_FRAME_2D[row]		= new uint8_t[width];
		ENC_RES_FRAME_2D[row]	= new int32_t[width];
		ENC_TC_FRAME_2D[row]	= new int32_t[width];
		DEC_RES_FRAME_2D[row]	= new int32_t[width];
		DEC_TC_FRAME_2D[row]	= new int32_t[width];
		QTC_FRAME_2D[row]		= new int32_t[width];
		QP_FRAME_2D[row]		= new int32_t[width];

		// Split
		CUR_FRAME_2DS[row]		= new uint8_t[width];
		CUR_REC_FRAME_2DS[row]	= new uint8_t[width];
		PREV_REC_FRAME_2DS[row] = new uint8_t[width];
		REF_FRAME_2DS[row]		= new uint8_t[width];
		ENC_RES_FRAME_2DS[row]	= new int32_t[width];
		ENC_TC_FRAME_2DS[row]	= new int32_t[width];
		DEC_RES_FRAME_2DS[row]	= new int32_t[width];
		DEC_TC_FRAME_2DS[row]	= new int32_t[width];
		QTC_FRAME_2DS[row]		= new int32_t[width];
		QP_FRAME_2DS[row]		= new int32_t[width];
	}

	// MDIFF Data for Regular Blocks 
	struct MDIFF** MDIFF_VECTOR			= new struct MDIFF*[(height / block)];
	struct MDIFF** MDIFF_VECTOR_DIFF	= new struct MDIFF*[(height / block)];
	for (int row = 0; row < height; row += block) {
		MDIFF_VECTOR[row / block]		= new struct MDIFF[width / block];
		MDIFF_VECTOR_DIFF[row / block]	= new struct MDIFF[width / block];
	}

	// MDIFF Data for Split Blocks
	struct MDIFF** MDIFF_VECTORS				= new struct MDIFF*[(height / block_split)];
	struct MDIFF** MDIFF_VECTOR_DIFFS			= new struct MDIFF*[(height / block_split)];
	for (int row = 0; row < height; row += block_split) {
		MDIFF_VECTORS[row / block_split]		= new struct MDIFF[width / block_split];
		MDIFF_VECTOR_DIFFS[row / block_split]	= new struct MDIFF[width / block_split];
	}

	// Interframe Algorithm Selection
	// ================================================
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

	if (ParallelMode == SINGLETHREADED) {
		printf("SINGLETHREADED\n");
	}
	if (ParallelMode == BLOCKTHREADED) {
		printf("BLOCKTHREADED\n");
	}
	// Runtime Start
	// =============================================
	int start_s = clock();

	// Encode Each Frame
	// =========================================
	for (int frame = 0; frame < frames; frame++) {
		//Open GOLOMB and MDIFF files
		snprintf(golomb_name, sizeof(golomb_name), "testdata\\COEFF_GOLOMB_CODING_%d", frame);
		snprintf(mdiff_name, sizeof(mdiff_name), "testdata\\MDIFF_GOLOMB_%d", frame);
		mdiff_golomb = fopen(mdiff_name, "wb");
		golomb_file = fopen(golomb_name, "wb");

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

		if (FrameType == PFRAME) { // Go to the beginning of the previous reconstructed frame and copy it to buffer
			fseek(recfile, (frame - 1)*FRAME_SIZE, SEEK_SET);
			for (unsigned int row = 0; row++; row < height) {
				fread(PREV_REC_FRAME_2D[row], sizeof(uint8_t), width, recfile);
			}
			if (VBSEnable) { //fill split buffers
				fseek(recfile, (frame - 1)*FRAME_SIZE, SEEK_SET);
				for (unsigned int row = 0; row++; row < height) {
					fread(PREV_REC_FRAME_2DS[row], sizeof(uint8_t), width, recfile);
				}
			}//end split buffer
		}

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

		if (ParallelMode == SINGLETHREADED) {
			time(&start_time);
			for (int row = 0; row < height; row += block) {
				for (int col = 0; col < width; col += block) {

					BlockThread(row, col,
						width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
						CUR_FRAME_2D,CUR_REC_FRAME_2D,PREV_REC_FRAME_2D,REF_FRAME_2D,ENC_RES_FRAME_2D,ENC_TC_FRAME_2D,DEC_RES_FRAME_2D,DEC_TC_FRAME_2D,QTC_FRAME_2D,QP_FRAME_2D,
						CUR_FRAME_2DS,CUR_REC_FRAME_2DS,PREV_REC_FRAME_2DS,REF_FRAME_2DS,ENC_RES_FRAME_2DS,ENC_TC_FRAME_2DS,DEC_RES_FRAME_2DS,DEC_TC_FRAME_2DS,QTC_FRAME_2DS,QP_FRAME_2DS,
						MDIFF_VECTOR,MDIFF_VECTOR_DIFF,MDIFF_VECTORS,MDIFF_VECTOR_DIFFS
					);
				}
			}
			time(&stop_time);
			printf("SINGLETHREAD TIME %.2f\n", (double)(stop_time - start_time));
		}
		
		if (ParallelMode == BLOCKTHREADED) {
			time(&start_time);
			if (FrameType == IFRAME) {
				for (uint32_t row = 0; row < height; row += 2 * block) {
					for (uint32_t col = 0; col <= width; col += block) {

						if ((col == 0) && (row == 0)) { // Most Top Left Block
							std::thread firstblock(BlockThread,row,col,
								width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
								CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
								CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
								MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
							);

							firstblock.join();
						}
						else if ((col == width) & (row == height - 2 * block)) { // Last Bottom Right Block
							std::thread lastblock(BlockThread, row + block, col - block,
								width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
								CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
								CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
								MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
							);
							lastblock.join();
						}
						else if (col == width) { // First Block in Next Section, Last Block in Current Section
							std::thread firstblock(BlockThread, row + 2 * block, 0,
								width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
								CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
								CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
								MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
							);
							std::thread lastblock(BlockThread, row + block, col - block,
								width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
								CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
								CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
								MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
							);
							firstblock.join();
							lastblock.join();
						}
						else {// Two blocks in Current Section
							std::thread rightblock(BlockThread, row, col,
								width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
								CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
								CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
								MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
							);
							std::thread bottomblock(BlockThread, row + block, col,
								width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
								CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
								CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
								MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
							);
							rightblock.join();
							bottomblock.join();
						}
					}
				}

			}

			if (FrameType == PFRAME) {
				for (uint32_t row = 0; row < height; row += 2 * block) {
					for (uint32_t col = 0; col < width; col += block) {
						std::thread firstrow(BlockThread, row, col,
							width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
							CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
							CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
							MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
						);
						std::thread secondrow(BlockThread, row + block, col,
							width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
							CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
							CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
							MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
						);
						firstrow.join();
						secondrow.join();
					}
				}
			}

			time(&stop_time);
			printf("BLOCKTHREAD TIME %.2f\n", (double)(stop_time - start_time));
		}

		// Differential and Entropy Encode steps 
		// ================================================
		for (int row = 0; row < height; row += block) {
			for (int col = 0; col < width; col += block) {
				int bitcount_temp = 0;
				bitcount_temp = entropy_wrapper(QTC_FRAME_2D, block, height, width, frame, row, col);
				coeff_bitcount = coeff_bitcount + bitcount_temp;
				diff_enc_wrapper(MDIFF_VECTOR, MDIFF_VECTOR_DIFF, FrameType, height, width, block, frame, row, col);
				bitcount_temp = encode_mdiff_wrapper(MDIFF_VECTOR_DIFF, MDIFF_VECTOR, height, width, block, frame, FrameType, row, col);
				mdiff_bitcount = mdiff_bitcount + bitcount_temp;
			}
		}

		// File Dumps
		// ================================================


		// Reconstructed Frames
		uint8_t *REC_FRAME = new uint8_t[FRAME_SIZE];
		for (int row = 0; row < height; row++)
			for (int col = 0; col < width; col++)
				REC_FRAME[col + width*row] = CUR_REC_FRAME_2D[row][col];
		fclose(recfile);//Weird behavior need to close file
		recfile = fopen(recfile_name, "a+b");
		fwrite(REC_FRAME, sizeof(uint8_t), FRAME_SIZE, recfile);

		// Bitcount Per frame
		fprintf(coeff_bitcount_file, "%d,%d\n", frame, coeff_bitcount);
		fprintf(mdiff_bitcount_file, "%d,%d\n", frame, mdiff_bitcount);
		fprintf(total_bitcount_file, "%d,%d\n", frame, coeff_bitcount + mdiff_bitcount);
		fclose(mdiff_golomb);
		fclose(golomb_file);


	}

	// Runtime Stop 
	// =============================================
	int stop_s = clock();
	fprintf(runtime_file,"%.2f\n", (double)(clock() - start_s) / CLOCKS_PER_SEC);
		
	// Deallocate Memory
	// =============================================

	for (unsigned int row = 0; row < height; row++) {

		// Regular
		delete CUR_FRAME_2D[row];		
		delete CUR_REC_FRAME_2D[row];		
		delete PREV_REC_FRAME_2D[row];  
		delete REF_FRAME_2D[row];	
		delete ENC_RES_FRAME_2D[row];	
		delete ENC_TC_FRAME_2D[row];	
		delete DEC_RES_FRAME_2D[row];	
		delete DEC_TC_FRAME_2D[row];	
		delete QTC_FRAME_2D[row];		
		delete QP_FRAME_2D[row];		

		// Split
		delete CUR_FRAME_2DS[row];		
		delete CUR_REC_FRAME_2DS[row];		
		delete PREV_REC_FRAME_2DS[row]; 
		delete REF_FRAME_2DS[row];	
		delete ENC_RES_FRAME_2DS[row];	
		delete ENC_TC_FRAME_2DS[row];	
		delete DEC_RES_FRAME_2DS[row];	
		delete DEC_TC_FRAME_2DS[row];	
		delete QTC_FRAME_2DS[row];		
		delete QP_FRAME_2DS[row];		
	}

	// Regular
	delete CUR_FRAME_2D;		
	delete CUR_REC_FRAME_2D;		
	delete PREV_REC_FRAME_2D;
	delete REF_FRAME_2D;		
	delete ENC_RES_FRAME_2D;	
	delete ENC_TC_FRAME_2D;	
	delete DEC_RES_FRAME_2D;	
	delete DEC_TC_FRAME_2D;	
	delete QTC_FRAME_2D;		
	delete QP_FRAME_2D;		

	// Split
	delete CUR_FRAME_2DS;
	delete CUR_REC_FRAME_2DS;
	delete PREV_REC_FRAME_2DS;
	delete REF_FRAME_2DS;	  
	delete ENC_RES_FRAME_2DS;
	delete ENC_TC_FRAME_2DS; 
	delete DEC_RES_FRAME_2DS;
	delete DEC_TC_FRAME_2DS; 
	delete QTC_FRAME_2DS;		
	delete QP_FRAME_2DS;

	// Close Files
	// =============================================
	fclose(recfile);
	fclose(mdiff_bitcount_file);
	fclose(coeff_bitcount_file);
	fclose(total_bitcount_file);
	fclose(reffile);
	fclose(decresfile);
	fclose(dectcfile);
	fclose(runtime_file);
	return 0;

}
