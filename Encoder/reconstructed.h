#ifndef __RECONSTRUCTED_H__
#define __RECONSTRUCTED_H__
#include <common.h>

void ReconstructBlock(uint8_t** REC_FRAME,int32_t** RES_FRAME, uint8_t** REF_FRAME,int row, int col, int block);

void ReconstructBlock(uint8_t** REC_FRAME, int32_t** RES_FRAME, uint8_t** REF_FRAME, int row, int col, int block) {
	for (int i = 0; i < block; i++) {
		for (int j = 0; j < block; j++) {
			int temp = RES_FRAME[row + i][col + j] + REF_FRAME[row + i][col + j];
			if(temp<=0)
				REC_FRAME[row + i][col + j] = 0;
			else if (temp >= 255)
				REC_FRAME[row + i][col + j] = 255;
			else
				REC_FRAME[row + i][col + j] = RES_FRAME[row + i][col + j] + REF_FRAME[row + i][col + j];
		}
	}
}
#endif 
