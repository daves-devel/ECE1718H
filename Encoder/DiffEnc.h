#include <common.h>
void diff_enc(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int row, int col);
void diff_enc_wrapper(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int height, int width, int block, int frame);

void diff_enc(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int row, int col) {
	if (FrameType == IFRAME) {
		if (col == 0) {
			MDIFF_VECTOR_DIFF[row][col].MODE = MDIFF_VECTOR[row][col].MODE;
		}
		else {
			MDIFF_VECTOR_DIFF[row][col].MODE = MDIFF_VECTOR[row][col].MODE - MDIFF_VECTOR[row][col - 1].MODE;
		}
	}
	else {//P frame
		if (col == 0) {
			MDIFF_VECTOR_DIFF[row][col].X = MDIFF_VECTOR[row][col].X;
			MDIFF_VECTOR_DIFF[row][col].Y = MDIFF_VECTOR[row][col].Y;
		}
		else {
			MDIFF_VECTOR_DIFF[row][col].X = MDIFF_VECTOR[row][col].X - MDIFF_VECTOR[row][col - 1].X;
			MDIFF_VECTOR_DIFF[row][col].Y = MDIFF_VECTOR[row][col].Y - MDIFF_VECTOR[row][col - 1].Y;
		}
	}
}

void diff_enc_wrapper(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int height, int width, int block, int frame) {
#ifdef TRACE_ON
	FILE* file_vector_org;
	FILE* file_vector_aft;
	char buf[0x100];
	snprintf(buf, sizeof(buf), "MDIFF_ORG_ENC%d.txt", frame);
	file_vector_org = fopen(buf, "w");
	snprintf(buf, sizeof(buf), "MDIFF_AFT_ENC%d.txt", frame);
	file_vector_aft = fopen(buf, "w");
#endif
	for (int row = 0; row < height; row=row+block) {
		for (int col = 0; col < width; col=col+block) {
			diff_enc(MDIFF_VECTOR, MDIFF_VECTOR_DIFF, FrameType, row/block, col/block);
#ifdef TRACE_ON
			fprintf(file_vector_org, "Block Y=%d X=%d\n", row/block, col/block);
			fprintf(file_vector_org, "FRAME_TYPE: %d MODE: %d X: %d Y: %d\n", FrameType, MDIFF_VECTOR[row / block][col / block].MODE, MDIFF_VECTOR[row / block][col / block].X, MDIFF_VECTOR[row / block][col / block].Y);
			fprintf(file_vector_aft, "Block Y=%d X=%d\n", row / block, col / block);
			fprintf(file_vector_aft, "FRAME_TYPE: %d MODE: %d X: %d Y: %d\n", FrameType, MDIFF_VECTOR_DIFF[row / block][col / block].MODE, MDIFF_VECTOR_DIFF[row / block][col / block].X, MDIFF_VECTOR_DIFF[row / block][col / block].Y);
#endif // TRACE_ON

		}
	}
}