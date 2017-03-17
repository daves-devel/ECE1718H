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
			MDIFF_VECTOR_DIFF[row][col].ref = MDIFF_VECTOR[row][col].ref;
		}
		else {
			MDIFF_VECTOR_DIFF[row][col].X = MDIFF_VECTOR[row][col].X - MDIFF_VECTOR[row][col - 1].X;
			MDIFF_VECTOR_DIFF[row][col].Y = MDIFF_VECTOR[row][col].Y - MDIFF_VECTOR[row][col - 1].Y;
			MDIFF_VECTOR_DIFF[row][col].ref = MDIFF_VECTOR[row][col].ref - MDIFF_VECTOR[row][col - 1].ref;
		}
	}
}

void diff_enc_wrapper(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int height, int width, int block, int frame) {
#ifdef TRACE_ON
	FILE* file_vector_org;
	FILE* file_vector_aft;
	char buf[0x100];
	snprintf(buf, sizeof(buf), "testdata\\MDIFF_ORG_ENC%d.txt", frame);
	file_vector_org = fopen(buf, "w");
	snprintf(buf, sizeof(buf), "testdata\\MDIFF_AFT_ENC%d.txt", frame);
	file_vector_aft = fopen(buf, "w");
#endif
	for (int row = 0; row < height; row=row+block) {
		for (int col = 0; col < width; col=col+block) {
			diff_enc(MDIFF_VECTOR, MDIFF_VECTOR_DIFF, FrameType, row/block, col/block);
#ifdef TRACE_ON
			if (MDIFF_VECTOR[row / block][col / block].split == 0) {
				fprintf(file_vector_org, "Block Y=%d X=%d\n", row / block, col / block);
				fprintf(file_vector_org, "FRAME_TYPE: %d MODE: %d X: %d Y: %d ref %d\n", FrameType, MDIFF_VECTOR[row / block][col / block].MODE, 
					MDIFF_VECTOR[row / block][col / block].X, MDIFF_VECTOR[row / block][col / block].Y, MDIFF_VECTOR[row / block][col / block].ref);
			}
			else {
				fprintf(file_vector_org, "Block Y=%d X=%d\n", row / block, col / block);
				fprintf(file_vector_org, "FRAME_TYPE: %d SPLIT %d MODE: %d %d %d %d X: %d %d %d %d Y: %d %d %d %d ref %d %d %d %d\n", FrameType, MDIFF_VECTOR[row / block][col / block].split,
					MDIFF_VECTOR[row / block][col / block].MODE, MDIFF_VECTOR[row / block][col / block].MODE2, MDIFF_VECTOR[row / block][col / block].MODE3, MDIFF_VECTOR[row / block][col / block].MODE4,
					MDIFF_VECTOR[row / block][col / block].X, MDIFF_VECTOR[row / block][col / block].X2, MDIFF_VECTOR[row / block][col / block].X3, MDIFF_VECTOR[row / block][col / block].X4, 
					MDIFF_VECTOR[row / block][col / block].Y, MDIFF_VECTOR[row / block][col / block].Y2, MDIFF_VECTOR[row / block][col / block].Y3, MDIFF_VECTOR[row / block][col / block].Y4,
					MDIFF_VECTOR[row / block][col / block].ref, MDIFF_VECTOR[row / block][col / block].ref2, MDIFF_VECTOR[row / block][col / block].ref3, MDIFF_VECTOR[row / block][col / block].ref4);
			}
			fprintf(file_vector_aft, "Block Y=%d X=%d\n", row / block, col / block);
			fprintf(file_vector_aft, "FRAME_TYPE: %d MODE: %d X: %d Y: %d ref %d\n", FrameType, MDIFF_VECTOR_DIFF[row / block][col / block].MODE, 
				MDIFF_VECTOR_DIFF[row / block][col / block].X, MDIFF_VECTOR_DIFF[row / block][col / block].Y, 
				MDIFF_VECTOR_DIFF[row / block][col / block].ref);
#endif // TRACE_ON

		}
	}
}