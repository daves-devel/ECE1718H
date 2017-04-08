#include <common.h>

struct MDIFF IntraFramePrediction(uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME,uint32_t row, uint32_t col, uint32_t block);

struct MDIFF IntraFramePrediction(uint8_t** CUR_FRAME, uint8_t** REC_FRAME, uint8_t** REF_FRAME,uint32_t row, uint32_t col, uint32_t block) {
	
	uint8_t* HorizontalModeArray	= new uint8_t[block];
	uint8_t* VerticalModeArray		= new uint8_t[block];

	// Mode 0 Horizontal 
	// ==================================================

	if (col == 0) {
		for (uint32_t i = 0; i < block; i++) {
			HorizontalModeArray[i] = 128;
		}
	}
	else {
		for (uint32_t i = 0; i < block; i++) {
			HorizontalModeArray[i] = REC_FRAME[row + i][col - 1];
		}
	}

	uint32_t HORIZONTAL_SAD = 0;

	for (uint32_t block_row = 0; block_row < block; block_row++) {
		for (uint32_t block_col = 0; block_col < block; block_col++) {
			HORIZONTAL_SAD += abs(CUR_FRAME[row + block_row][col + block_col] - HorizontalModeArray[block_row]);
		}
	}

	// Mode 1 Vertical
	// ==================================================

	if (row == 0) {
		for (uint32_t i = 0; i < block; i++) {
			VerticalModeArray[i] = 128;
		}
	}
	else {
		for (uint32_t i = 0; i < block; i++) {
			VerticalModeArray[i] = REC_FRAME[row-1][col + i];
		}
	}

	uint32_t VERTICAL_SAD = 0;

	for (uint32_t block_row = 0; block_row < block; block_row++) {
		for (uint32_t block_col = 0; block_col < block; block_col++) {
			VERTICAL_SAD += abs(CUR_FRAME[row + block_row][col + block_col] - VerticalModeArray[block_col ]);
		}
	}

	// Mode Select
	// ==================================================
	struct MDIFF INTRA_MODE;

	if (HORIZONTAL_SAD < VERTICAL_SAD) {
		INTRA_MODE.MODE = HORIZONTAL;
		INTRA_MODE.SAD = HORIZONTAL_SAD;
	}
	else {
		INTRA_MODE.MODE = VERTICAL;
		INTRA_MODE.SAD = VERTICAL_SAD;
	}

	// Fill the Reference Frame Block
	// ==================================================
	for (int i = 0; i < block; i++) {
		for (int j = 0; j < block; j++) {
			if (INTRA_MODE.MODE == HORIZONTAL) {
				(col == 0) ? (REF_FRAME[row + i][col + j] = 128): (REF_FRAME[row + i][col + j] = REC_FRAME[row + i][col - 1]);
			}
			if (INTRA_MODE.MODE == VERTICAL) {
				(row == 0) ? (REF_FRAME[row + i][col + j] = 128): (REF_FRAME[row + i][col + j] = REC_FRAME[row - 1][col + j]);
			}
		}
	}

	return INTRA_MODE;


}