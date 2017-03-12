#include <common.h>

struct MDIFF InterFramePrediction(uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME, int row, int col, int width, int height, int block, int range, int ref, MDIFF MDIFF_PREVIOUS, int QP, int RDO_EN);
struct MDIFF MinSADMinNORM(struct MDIFF BEST_GMV, struct MDIFF NEW_GMV);
struct MDIFF SelectRefWinner(struct MDIFF MDIFF_CUR, struct MDIFF MDIFFV_REF , uint8_t** REF_FRAME_CUR, uint8_t** REF_FRAME_REF, int block, int row, int col);

struct MDIFF SelectRefWinner(struct MDIFF CUR_MDIFF, struct MDIFF REF_MDIFF, uint8_t** REF_FRAME_CUR, uint8_t** REF_FRAME_REF, int block, int row, int col) {
	struct MDIFF WINNER = CUR_MDIFF;
	if (CUR_MDIFF.SAD > REF_MDIFF.SAD) {
		WINNER = REF_MDIFF;
		for (int i = 0; i < block; i++) {
			for (int j = 0; j < block; j++) {
				REF_FRAME_CUR[row + i][col + j] = REF_FRAME_REF[row + i][col + j];
			}
		}
	}
	return WINNER;
}

struct MDIFF InterFramePrediction(uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME, int row, int col, int width, int height, int block, int range, int ref, MDIFF **MDIFF_PREVIOUS, int QP, int RDO_EN) {

	struct MDIFF BEST_GMV;
	bool firstGMV = true;


	//RDO CODE
	int GMV_X_PREV = 0;
	int GMV_Y_PREV = 0;
	int SAD_PREV = 0;
	int NORM_PREV = 0;
	int REF_PREV = 0;

	int PREV_GVM_NOT_AVAILAIBLE=0;
	if (col != 0 && QP >= 7 && RDO_EN) {
		GMV_X_PREV	=	MDIFF_PREVIOUS[row/block][(col/block) - 1].X;
		GMV_Y_PREV	=	MDIFF_PREVIOUS[row/block][(col/block) - 1].Y;
		SAD_PREV	=		MDIFF_PREVIOUS[row / block][(col / block) - 1].SAD;
		NORM_PREV	=		MDIFF_PREVIOUS[row/block][(col/block) - 1].NORM;
		REF_PREV	=		MDIFF_PREVIOUS[row / block][(col / block) - 1].ref;

		if (((GMV_X_PREV + col) < 0) || ((GMV_X_PREV + col + block) > width)) {
			PREV_GVM_NOT_AVAILAIBLE=1;
		}
		if (((GMV_Y_PREV + row) < 0) || ((GMV_Y_PREV + row + block) > height)) {
			PREV_GVM_NOT_AVAILAIBLE=1;
		}
	}
	//RDO CODE END

	for (int GMV_X = 0 - range; GMV_X <= range; GMV_X++) {
		for (int GMV_Y = 0 - range; GMV_Y <= range; GMV_Y++) {
			if (((GMV_X + col) < 0) || ((GMV_X + col + block) > width)) {
				continue; //Block outside search space so don't compute
			}
			if (((GMV_Y + row) < 0) || ((GMV_Y + row + block) > height)) {
				continue; //Block outside search space so don't compute
			}

			//RDO CODE
			if (col != 0 && QP >= 7 && RDO_EN && !PREV_GVM_NOT_AVAILAIBLE) {
				BEST_GMV.ref = ref;
				BEST_GMV.X = GMV_X_PREV;
				BEST_GMV.Y = GMV_Y_PREV;
				BEST_GMV.SAD = SAD_PREV;
				BEST_GMV.NORM = NORM_PREV;
				BEST_GMV.ref = REF_PREV;
				break;
			}
			//RDO END

			struct MDIFF NEW_GMV;
			NEW_GMV.ref = ref;
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

			// Original Algorithm
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
