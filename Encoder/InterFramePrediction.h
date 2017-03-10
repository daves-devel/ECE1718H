#include <common.h>

struct MDIFF InterFramePrediction(uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME, int row, int col, int width, int height, int block, int range);
struct MDIFF MinSADMinNORM(struct MDIFF BEST_GMV, struct MDIFF NEW_GMV);

struct MDIFF InterFramePrediction(uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME, int row, int col, int width, int height, int block, int range) {

	struct MDIFF BEST_GMV;
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
