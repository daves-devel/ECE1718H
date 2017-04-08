#include <common.h>
void diff_enc(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int row, int col);
void diff_enc_wrapper(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int height, int width, int block, int frame);

void diff_enc(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int row, int col) {
	if (FrameType == IFRAME) {
		if (col == 0) {
			MDIFF_VECTOR_DIFF[row][col].split = MDIFF_VECTOR[row][col].split;
			MDIFF_VECTOR_DIFF[row][col].MODE = MDIFF_VECTOR[row][col].MODE;
			MDIFF_VECTOR_DIFF[row][col].MODE2 = MDIFF_VECTOR[row][col].MODE2;
			MDIFF_VECTOR_DIFF[row][col].MODE3 = MDIFF_VECTOR[row][col].MODE3;
			MDIFF_VECTOR_DIFF[row][col].MODE4 = MDIFF_VECTOR[row][col].MODE4;
		}
		else {
			MDIFF_VECTOR_DIFF[row][col].split = MDIFF_VECTOR[row][col].split - MDIFF_VECTOR[row][col - 1].split;
			MDIFF_VECTOR_DIFF[row][col].MODE = MDIFF_VECTOR[row][col].MODE - MDIFF_VECTOR[row][col - 1].MODE;
			MDIFF_VECTOR_DIFF[row][col].MODE2 = MDIFF_VECTOR[row][col].MODE2 - MDIFF_VECTOR[row][col - 1].MODE2;
			MDIFF_VECTOR_DIFF[row][col].MODE3 = MDIFF_VECTOR[row][col].MODE3 - MDIFF_VECTOR[row][col - 1].MODE3;
			MDIFF_VECTOR_DIFF[row][col].MODE4 = MDIFF_VECTOR[row][col].MODE4 - MDIFF_VECTOR[row][col - 1].MODE4;
		}
	}
	else {//P frame
		if (col == 0) {
			MDIFF_VECTOR_DIFF[row][col].split = MDIFF_VECTOR[row][col].split;
			MDIFF_VECTOR_DIFF[row][col].X = MDIFF_VECTOR[row][col].X;
			MDIFF_VECTOR_DIFF[row][col].X2 = MDIFF_VECTOR[row][col].X2;
			MDIFF_VECTOR_DIFF[row][col].X3 = MDIFF_VECTOR[row][col].X3;
			MDIFF_VECTOR_DIFF[row][col].X4 = MDIFF_VECTOR[row][col].X4;
			MDIFF_VECTOR_DIFF[row][col].Y = MDIFF_VECTOR[row][col].Y;
			MDIFF_VECTOR_DIFF[row][col].Y2 = MDIFF_VECTOR[row][col].Y2;
			MDIFF_VECTOR_DIFF[row][col].Y3 = MDIFF_VECTOR[row][col].Y3;
			MDIFF_VECTOR_DIFF[row][col].Y4 = MDIFF_VECTOR[row][col].Y4;
		}
		else {
			MDIFF_VECTOR_DIFF[row][col].split = MDIFF_VECTOR[row][col].split - MDIFF_VECTOR[row][col - 1].split;
			MDIFF_VECTOR_DIFF[row][col].X = MDIFF_VECTOR[row][col].X - MDIFF_VECTOR[row][col - 1].X;
			MDIFF_VECTOR_DIFF[row][col].X2 = MDIFF_VECTOR[row][col].X2 - MDIFF_VECTOR[row][col - 1].X2;
			MDIFF_VECTOR_DIFF[row][col].X3 = MDIFF_VECTOR[row][col].X3 - MDIFF_VECTOR[row][col - 1].X3;
			MDIFF_VECTOR_DIFF[row][col].X4 = MDIFF_VECTOR[row][col].X4 - MDIFF_VECTOR[row][col - 1].X4;
			MDIFF_VECTOR_DIFF[row][col].Y = MDIFF_VECTOR[row][col].Y - MDIFF_VECTOR[row][col - 1].Y;
			MDIFF_VECTOR_DIFF[row][col].Y2 = MDIFF_VECTOR[row][col].Y2 - MDIFF_VECTOR[row][col - 1].Y2;
			MDIFF_VECTOR_DIFF[row][col].Y3 = MDIFF_VECTOR[row][col].Y3 - MDIFF_VECTOR[row][col - 1].Y3;
			MDIFF_VECTOR_DIFF[row][col].Y4 = MDIFF_VECTOR[row][col].Y4 - MDIFF_VECTOR[row][col - 1].Y4;
		}
	}
}

void diff_enc_wrapper(MDIFF** MDIFF_VECTOR, MDIFF** MDIFF_VECTOR_DIFF, int FrameType, int height, int width, int block, int frame, int row, int col) {
	diff_enc(MDIFF_VECTOR, MDIFF_VECTOR_DIFF, FrameType, row/block, col/block);
#ifdef TRACE_ON
			if (MDIFF_VECTOR[row / block][col / block].split == 0) {
				fprintf(file_vector_org, "Block Y=%d X=%d\n", row / block, col / block);
				fprintf(file_vector_org, "FRAME_TYPE: %d SPLIT: %d MODE: %d X: %d Y: %d ref %d\n", FrameType, MDIFF_VECTOR[row/block][col/block].split, 
					MDIFF_VECTOR[row / block][col / block].MODE, 
					MDIFF_VECTOR[row / block][col / block].X, MDIFF_VECTOR[row / block][col / block].Y, 
					MDIFF_VECTOR[row / block][col / block].ref);
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