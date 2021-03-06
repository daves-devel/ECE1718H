#ifndef __INTERFRAMEPREDICTION_H__
#define __INTERFRAMEPREDICTION_H__
#include <common.h>

struct MDIFF InterFramePrediction	(uint32_t INTERMODE, uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME, int row, int col, int width, int height, int block, int range, int ref, MDIFF** MDIFF_PREVIOUS);
struct MDIFF InterFrameDefault		(uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME, int row, int col, int width, int height, int block, int range);
struct MDIFF InterFrameRDO			(uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME, int row, int col, int width, int height, int block, int range, int ref, MDIFF** MDIFF_PREVIOUS);
struct MDIFF InterFrameThreeStep	(uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME, int row, int col, int width, int height, int block, int range);
struct MDIFF MinSADMinNORM			(struct MDIFF BEST_GMV, struct MDIFF NEW_GMV);
void VBSWinner(MDIFF** MDIFF_CUR, MDIFF** MDIFF_SLPIT, int row, int col, int block, uint8_t** CUR_FRAME_2D, uint8_t**CUR_FRAME_2DS, int **QTC_FRAME_2D, int **QTC_FRAME_2DS);

struct MDIFF InterFramePrediction(uint32_t INTERMODE, uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME, int row, int col, int width, int height, int block, int range, int ref, MDIFF** MDIFF_PREVIOUS) {

	if (INTERMODE == DEFAULT) {
		return InterFrameDefault(CUR_FRAME, REC_FRAME, REF_FRAME, row, col, width, height, block, range);
	}
	if (INTERMODE == RDO) {
		return InterFrameRDO(CUR_FRAME, REC_FRAME, REF_FRAME, row, col, width, height, block, range, ref, MDIFF_PREVIOUS);
	}
	if (INTERMODE == FME) {
		return InterFrameThreeStep(CUR_FRAME, REC_FRAME, REF_FRAME, row, col, width, height, block, range);
	}

}

struct MDIFF InterFrameDefault(uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME, int row, int col, int width, int height, int block, int range) {

	struct MDIFF	BEST_GMV;
	bool firstGMV = true;

	for (int GMV_X = 0 - range; GMV_X <= range; GMV_X++) {
		for (int GMV_Y = 0 - range; GMV_Y <= range; GMV_Y++) {
			if (((GMV_X + col) < 0) || ((GMV_X + col + block) > width)) {
				continue; //Block outside search space so don't compute
			}
			if (((GMV_Y + row) < 0) || ((GMV_Y + row + block) > height)) {
				continue; //Block outside search space so don't compute
			}

			struct MDIFF NEW_GMV;
			NEW_GMV.X = GMV_X;
			NEW_GMV.Y = GMV_Y;
			NEW_GMV.SAD = 0;
			NEW_GMV.NORM = abs(NEW_GMV.X) + abs(NEW_GMV.Y);

			//Calculate SAD
			for (int i = 0; i < block; i++) {
				for (int j = 0; j < block; j++) {
					NEW_GMV.SAD += abs(CUR_FRAME[row + i][+col + j] - REC_FRAME[GMV_Y + row + i][GMV_X + col + j]);
				}
			}

			// Pick Best Global Motion Vector 
			if (firstGMV) {
				firstGMV = false;
				BEST_GMV = NEW_GMV;
				continue;
			}

			// Lowest SAD and NORM 
			BEST_GMV = MinSADMinNORM(BEST_GMV, NEW_GMV);

		}
	}

	// Copy Best Matching Block to Reference Frame
	for (int i = 0; i < block; i++) {
		for (int j = 0; j < block; j++) {
			REF_FRAME[row + i][col + j] = REC_FRAME[row + BEST_GMV.Y + i][col + BEST_GMV.X + j];
		}
	}
	return BEST_GMV;
}

struct MDIFF InterFrameRDO(uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME, int row, int col, int width, int height, int block, int range, int ref, MDIFF** MDIFF_PREVIOUS) {

	struct MDIFF	BEST_GMV;
	bool firstGMV	= true;
	int GMV_X_PREV	= 0;
	int GMV_Y_PREV	= 0;
	int SAD_PREV	= 0;
	int NORM_PREV	= 0;
	int REF_PREV	= 0;

	int PREV_GVM_NOT_AVAILAIBLE=0;
	if (col != 0) {
		GMV_X_PREV	=	MDIFF_PREVIOUS[row/block][(col/block) - 1].X;
		GMV_Y_PREV	=	MDIFF_PREVIOUS[row/block][(col/block) - 1].Y;
		SAD_PREV	=	MDIFF_PREVIOUS[row / block][(col / block) - 1].SAD;
		NORM_PREV	=	MDIFF_PREVIOUS[row/block][(col/block) - 1].NORM;

		if (((GMV_X_PREV + col) < 0) || ((GMV_X_PREV + col + block) > width)) {
			PREV_GVM_NOT_AVAILAIBLE=1;
		}
		if (((GMV_Y_PREV + row) < 0) || ((GMV_Y_PREV + row + block) > height)) {
			PREV_GVM_NOT_AVAILAIBLE=1;
		}
	}

	for (int GMV_X = 0 - range; GMV_X <= range; GMV_X++) {
		for (int GMV_Y = 0 - range; GMV_Y <= range; GMV_Y++) {
			if (((GMV_X + col) < 0) || ((GMV_X + col + block) > width)) {
				continue; //Block outside search space so don't compute
			}
			if (((GMV_Y + row) < 0) || ((GMV_Y + row + block) > height)) {
				continue; //Block outside search space so don't compute
			}

			if (col != 0 && !PREV_GVM_NOT_AVAILAIBLE) { 
				BEST_GMV.X = GMV_X_PREV;
				BEST_GMV.Y = GMV_Y_PREV;
				BEST_GMV.SAD = SAD_PREV;
				BEST_GMV.NORM = NORM_PREV;
				break;
			}

			struct MDIFF NEW_GMV;
			NEW_GMV.X = GMV_X;
			NEW_GMV.Y = GMV_Y;
			NEW_GMV.SAD = 0;
			NEW_GMV.NORM = abs(NEW_GMV.X) + abs(NEW_GMV.Y);

			//Calculate SAD
			for (int i = 0; i < block; i++) {
				for (int j = 0; j < block; j++) {
					NEW_GMV.SAD += abs(CUR_FRAME[row + i][+col + j] - REC_FRAME[GMV_Y + row + i][GMV_X + col + j]);
				}
			}

			// Pick Best Global Motion Vector 
			if (firstGMV) {
				firstGMV = false;
				BEST_GMV = NEW_GMV;
				continue;
			}

			// Lowest SAD and NORM 
			BEST_GMV = MinSADMinNORM(BEST_GMV,NEW_GMV);

		}
	}

	// Copy Best Matching Block to Reference Frame
	for (int i = 0; i < block; i++) {
		for (int j = 0; j < block; j++) {
			REF_FRAME[row + i][col + j] = REC_FRAME[row + BEST_GMV.Y + i][col + BEST_GMV.X + j];
		}
	}
	return BEST_GMV;
}

struct MDIFF InterFrameThreeStep(uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME, int row, int col, int width, int height, int block, int range) {
	
	struct MDIFF	BEST_GMV;
	bool firstGMV = true;

	int search_range = range; 
	int origin_x = 0;
	int origin_y = 0;
	//DEBUG printf("Block (%d,%d)\n", row, col);
	do {

		for (int GMV_X = origin_x - search_range; GMV_X <= (origin_x + search_range); GMV_X += search_range) {
			for (int GMV_Y = origin_y - search_range; GMV_Y <= (origin_y + search_range); GMV_Y += search_range) {

				if (((GMV_X + col) < 0) || ((GMV_X + col + block) > width)) {
					continue; //Block outside search space so don't compute
				}
				if (((GMV_Y + row) < 0) || ((GMV_Y + row + block) > height)) {
					continue; //Block outside search space so don't compute
				}
				if ((abs(GMV_X) > 16) || (abs(GMV_Y) > 16)) {
					continue; // GMV outside search range
				}

				struct MDIFF NEW_GMV;
				NEW_GMV.X = GMV_X;
				NEW_GMV.Y = GMV_Y;
				NEW_GMV.SAD = 0;
				NEW_GMV.NORM = abs(NEW_GMV.X) + abs(NEW_GMV.Y);

				//Calculate SAD
				for (int i = 0; i < block; i++) {
					for (int j = 0; j < block; j++) {
						NEW_GMV.SAD += abs(CUR_FRAME[row + i][+col + j] - REC_FRAME[GMV_Y + row + i][GMV_X + col + j]);
					}
				}

				// Pick Best Global Motion Vector 
				if (firstGMV) {
					firstGMV = false;
					BEST_GMV = NEW_GMV;
					continue;
				}
			
				// Lowest SAD and NORM 
				BEST_GMV = MinSADMinNORM(BEST_GMV, NEW_GMV);

			}
		}

		origin_x = BEST_GMV.X;
		origin_y = BEST_GMV.Y;

		//DEBUG printf("(%d,%d) ", BEST_GMV.X, BEST_GMV.Y); 
		if (search_range == 1) {
			search_range = 0; // Done Searching
		}
		else {
			search_range = search_range / 2; // Keep Searching
		}

	} while (search_range > 0);

	// Copy Best Matching Block to Reference Frame
	for (int i = 0; i < block; i++) {
		for (int j = 0; j < block; j++) {
			REF_FRAME[row + i][col + j] = REC_FRAME[row + BEST_GMV.Y + i][col + BEST_GMV.X + j];
		}
	}

	return BEST_GMV;

}

struct MDIFF MinSADMinNORM(struct MDIFF BEST_GMV,struct MDIFF NEW_GMV) {

	if (BEST_GMV.SAD > NEW_GMV.SAD) {
		BEST_GMV = NEW_GMV;
	}
	else if (BEST_GMV.SAD == NEW_GMV.SAD) {
		if (BEST_GMV.NORM > NEW_GMV.NORM) {
			BEST_GMV = NEW_GMV;
		}
		else if (BEST_GMV.NORM == NEW_GMV.NORM) {
			if (abs(BEST_GMV.Y) > abs(NEW_GMV.Y)) {
				BEST_GMV = NEW_GMV;
			}
			else if (abs(BEST_GMV.Y) == abs(NEW_GMV.Y)) {
				if (abs(BEST_GMV.X) > abs(NEW_GMV.X)) {
					BEST_GMV = NEW_GMV;
				}
				else if (abs(BEST_GMV.X) == abs(NEW_GMV.X)) {
					// Either Do Nothing
					// Or we could Overwrite
				}
			}
		}
	}

	return BEST_GMV;
}

void VBSWinner(MDIFF** MDIFF_CUR, MDIFF** MDIFF_SPLIT, int row, int col, int block, uint8_t** CUR_FRAME_2D, uint8_t**CUR_FRAME_2DS, int **QTC_FRAME_2D, int **QTC_FRAME_2DS) {
	int row_split = row / (block / 2);
	int col_split = col / (block / 2);
	int row_org = row / block;
	int col_org = col / block;
	int SPLIT_SAD = 0;
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 2; i++) {
			SPLIT_SAD = MDIFF_SPLIT[row_split + j][col_split + i].SAD + SPLIT_SAD;
		}
	}
	MDIFF_CUR[row_org][col_org].split = 0;
	if (SPLIT_SAD < MDIFF_CUR[row_org][col_org].SAD) {
		//Adding X and Y for Inter
		MDIFF_CUR[row_org][col_org].X = MDIFF_SPLIT[row_split][col_split].X;
		MDIFF_CUR[row_org][col_org].X2 = MDIFF_SPLIT[row_split][col_split + 1].X;
		MDIFF_CUR[row_org][col_org].X3 = MDIFF_SPLIT[row_split + 1][col_split].X;
		MDIFF_CUR[row_org][col_org].X4 = MDIFF_SPLIT[row_split + 1][col_split + 1].X;
		MDIFF_CUR[row_org][col_org].Y = MDIFF_SPLIT[row_split][col_split].Y;
		MDIFF_CUR[row_org][col_org].Y2 = MDIFF_SPLIT[row_split][col_split + 1].Y;
		MDIFF_CUR[row_org][col_org].Y3 = MDIFF_SPLIT[row_split + 1][col_split].Y;
		MDIFF_CUR[row_org][col_org].Y4 = MDIFF_SPLIT[row_split + 1][col_split + 1].Y;
		//Adding modes for Intra
		MDIFF_CUR[row_org][col_org].MODE = MDIFF_SPLIT[row_split][col_split].MODE;
		MDIFF_CUR[row_org][col_org].MODE2 = MDIFF_SPLIT[row_split][col_split + 1].MODE;
		MDIFF_CUR[row_org][col_org].MODE3 = MDIFF_SPLIT[row_split + 1][col_split].MODE;
		MDIFF_CUR[row_org][col_org].MODE4 = MDIFF_SPLIT[row_split + 1][col_split + 1].MODE;
		//SAD
		MDIFF_CUR[row_org][col_org].SAD = SPLIT_SAD;

		//SPLIT
		MDIFF_CUR[row_org][col_org].split = 1;
		for (int row = row_org; row < row_org + block; row++) {
			for (int col = col_org; col < col_org + block; col++) {
				CUR_FRAME_2D[row][col] = CUR_FRAME_2DS[row][col];
				QTC_FRAME_2D[row][col] = QTC_FRAME_2DS[row][col];
			}
		}
	}

}

#endif 