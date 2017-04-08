#ifndef __DIFFDEC_H__
#define __DIFFDEC_H__
#include <common.h>
void diff_dec(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int row, int col);
void diff_dec_wrapper(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int height, int width, int block, int frame);

void diff_dec(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int row, int col) {
	if (FrameType == IFRAME) {
		if (col == 0) {
			MDIFF_VECTOR[row][col].MODE = MDIFF_VECTOR_DIFF[row][col].MODE;
		}
		else {
			MDIFF_VECTOR[row][col].MODE = MDIFF_VECTOR_DIFF[row][col].MODE + MDIFF_VECTOR[row][col - 1].MODE;
		}
	}
	else {//P frame
		if (col == 0) {
			MDIFF_VECTOR[row][col].X = MDIFF_VECTOR_DIFF[row][col].X;
			MDIFF_VECTOR[row][col].Y = MDIFF_VECTOR_DIFF[row][col].Y;
		}
		else {
			MDIFF_VECTOR[row][col].X = MDIFF_VECTOR_DIFF[row][col].X + MDIFF_VECTOR[row][col - 1].X;
			MDIFF_VECTOR[row][col].Y = MDIFF_VECTOR_DIFF[row][col].Y + MDIFF_VECTOR[row][col - 1].Y;
		}
	}
}

void diff_dec_wrapper(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int height, int width, int block, int frame) {
#ifdef TRACE_ON
	FILE* file_vector_org;
	FILE* file_vector_aft;
	char buf[0x100];
	snprintf(buf, sizeof(buf), "testdata\\MDIFF_ORG_DEC%d.txt", frame);
	file_vector_org = fopen(buf, "w");
	snprintf(buf, sizeof(buf), "testdata\\MDIFF_AFT_DEC%d.txt", frame);
	file_vector_aft = fopen(buf, "w");
#endif
	for (int row = 0; row < height; row = row + block) {
		for (int col = 0; col < width; col = col + block) {
			diff_dec(MDIFF_VECTOR, MDIFF_VECTOR_DIFF, FrameType, row / block, col / block);
#ifdef TRACE_ON
			fprintf(file_vector_org, "Block Y=%d X=%d\n", row / block, col / block);
			fprintf(file_vector_org, "FRAME_TYPE: %d MODE: %d X: %d Y: %d ref %d\n", FrameType, MDIFF_VECTOR[row / block][col / block].MODE, MDIFF_VECTOR[row / block][col / block].X, MDIFF_VECTOR[row / block][col / block].Y, MDIFF_VECTOR[row / block][col / block].ref);
			fprintf(file_vector_aft, "Block Y=%d X=%d\n", row / block, col / block);
			fprintf(file_vector_aft, "FRAME_TYPE: %d MODE: %d X: %d Y: %d  ref %d\n", FrameType, MDIFF_VECTOR_DIFF[row / block][col / block].MODE, MDIFF_VECTOR_DIFF[row / block][col / block].X, MDIFF_VECTOR_DIFF[row / block][col / block].Y, MDIFF_VECTOR_DIFF[row / block][col / block].ref);
#endif // TRACE_ON

		}
	}
	fclose(file_vector_org);
	fclose(file_vector_aft);
}
#endif