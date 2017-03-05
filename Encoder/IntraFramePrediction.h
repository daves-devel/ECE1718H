#include <common.h>

struct MDIFF IntraFramePrediction(unsigned int row, unsigned int col, unsigned int block, unsigned char** CUR_FRAME);

struct MDIFF IntraFramePrediction(unsigned int row, unsigned int col, unsigned int block, unsigned char** CUR_FRAME) {
	
	unsigned char* HorizontalModeArray	= new unsigned char[block];
	unsigned char* VerticalModeArray	= new unsigned char[block];

	// Mode 0 Horizontal 
	// ==================================================

	if (col == 0) {
		for (unsigned int i = 0; i < block; i++) {
			HorizontalModeArray[i] = 128;
		}
	}
	else {
		for (unsigned int i = 0; i < block; i++) {
			HorizontalModeArray[i] = CUR_FRAME[row + i][col - 1];
		}
	}

	unsigned int HORIZONTAL_SAD = 0;

	for (unsigned int block_row = 0; block_row < block; block_row++) {
		for (unsigned int block_col = 0; block_col < block; block_col++) {
			HORIZONTAL_SAD += CUR_FRAME[row + block_row][col + block_col] - HorizontalModeArray[block_row];
		}
	}

	// Mode 1 Vertical
	// ==================================================

	if (row == 0) {
		for (unsigned int i = 0; i < block; i++) {
			VerticalModeArray[i] = 128;
		}
	}
	else {
		for (unsigned int i = 0; i < block; i++) {
			VerticalModeArray[i] = CUR_FRAME[row-1][col + i];
		}
	}

	unsigned int VERTICAL_SAD = 0;

	for (unsigned int block_row = 0; block_row < block; block_row++) {
		for (unsigned int block_col = 0; block_col < block; block_col++) {
			VERTICAL_SAD += CUR_FRAME[row + block_row][col + block_col] - VerticalModeArray[block_col ];
		}
	}

	// Mode Select
	// ==================================================
	struct MDIFF INTRA_MODE;

	if (HORIZONTAL_SAD < VERTICAL_SAD) {
		INTRA_MODE.MODE = HORIZONTAL;
	}
	else {
		INTRA_MODE.MODE = VERTICAL;
	}

	return INTRA_MODE;
}