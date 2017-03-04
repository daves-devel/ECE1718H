#include <InterFramePrediction.h>
struct GMV InterFramePrediction(int row, // Pixel Row in Current Frame
	int col, // Pixel Col in Current Frame
	int width,
	int height,
	int block,
	int range,
	unsigned char* CUR_FRAME,
	unsigned char* REC_FRAME) {

	struct GMV BEST_GMV;
	bool firstGMV = true;

	for (int GMV_X = 0 - range; GMV_X <= range; GMV_X++) {
		for (int GMV_Y = 0 - range; GMV_Y <= range; GMV_Y++) {
			if (((GMV_X + col) < 0) || ((GMV_X + col + block) > width)) {
				continue; //Block outisde search space so don't compute
			}
			if (((GMV_Y + row) < 0) || ((GMV_Y + row + block) > height)) {
				continue; //Block outisde search space so don't compute
			}

			int X = GMV_X;
			int Y = GMV_Y;
			unsigned int SAD = 0;
			unsigned int NORM = abs(X) + abs(Y);

			//Calculate SAD
			for (int i = 0; i < block; i++) {
				for (int j = 0; j < block; j++) {
					SAD += abs(CUR_FRAME[(row + i)*width + col + j] - REC_FRAME[(GMV_Y + row + i)*width + GMV_X + col + j]);
				}
			}

			// Pick Best Global Motion Vector 
			if (firstGMV) {
				firstGMV = false;
				BEST_GMV.X = X;
				BEST_GMV.Y = Y;
				BEST_GMV.SAD = SAD;
				BEST_GMV.NORM = NORM;
				continue;
			}

			if (BEST_GMV.SAD > SAD) {
				BEST_GMV.X = X;
				BEST_GMV.Y = Y;
				BEST_GMV.SAD = SAD;
				BEST_GMV.NORM = NORM;
			}
			else if (BEST_GMV.SAD == SAD) {
				if (BEST_GMV.NORM > NORM) {
					BEST_GMV.X = X;
					BEST_GMV.Y = Y;
					BEST_GMV.SAD = SAD;
					BEST_GMV.NORM = NORM;
				}
				else if (BEST_GMV.NORM == NORM) {
					if (abs(BEST_GMV.Y) > abs(Y)) {
						BEST_GMV.X = X;
						BEST_GMV.Y = Y;
						BEST_GMV.SAD = SAD;
						BEST_GMV.NORM = NORM;
					}
					else if (abs(BEST_GMV.Y) == abs(Y)) {
						if (abs(BEST_GMV.X) > abs(X)) {
							BEST_GMV.X = X;
							BEST_GMV.Y = Y;
							BEST_GMV.SAD = SAD;
							BEST_GMV.NORM = NORM;
						}
						else if (abs(BEST_GMV.X) == abs(X)) {
							// Either Do Nothing
							// Or we could Overwrite
						}
					}
				}
			}
		}
	}
	return BEST_GMV;
}
