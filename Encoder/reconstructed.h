#include <common.h>
#include <assert.h>
void recon(int8_t * residual_mem, uint8_t* recon_mem, int block_size, int frame_width, int frame_height, unsigned char** MOTION_FRAME);
void generate_recon(int8_t * residual_mem, uint8_t* recon_mem, int x, int y, int block_size, int frame_width, unsigned char** MOTION_FRAME);

void ReconstructBlock(uint8_t** REC_FRAME,int32_t** RES_FRAME, uint8_t** REF_FRAME,int row, int col, int block);
void ReconstructBlockDecodeI(uint8_t** REC_FRAME, int8_t** RES_FRAME, uint8_t** REF_FRAME, int row, int col, int block, MDIFF** MDIFF_VECTOR_DIFF);
void ReconstructBlockDecodeP(uint8_t** REC_FRAME, int8_t** RES_FRAME, uint8_t** PREV_DEC_FRAME, uint8_t** REF_FRAME, int row, int col, int block, MDIFF** MDIFF_VECTOR_DIFF);

void recon(int8_t * residual_mem, uint8_t* recon_mem, int block_size, int frame_width, int frame_height, unsigned char** MOTION_FRAME) {
	for (int y = 0; y<frame_height; y = y + block_size) {
		for (int x = 0; x<frame_width; x = x + block_size) {
			generate_recon(residual_mem, recon_mem, x, y, block_size, frame_width, MOTION_FRAME);
		}
	}
}

void generate_recon(int8_t * residual_mem, uint8_t* recon_mem, int x, int y, int block_size, int frame_width, unsigned char** MOTION_FRAME) {
	int start_position = x + y*frame_width;
	for (int i = 0; i< block_size; i++) {
		for (int j = 0; j< block_size; j++) {
			recon_mem[i*frame_width + j + start_position] = residual_mem[i*frame_width + j + start_position] + MOTION_FRAME[y + i][x + j];//TODO add prediction
		}
	}
}

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

void ReconstructBlockDecodeI(uint8_t** REC_FRAME, int32_t** RES_FRAME, uint8_t** REF_FRAME, int row, int col, int block, MDIFF** MDIFF_VECTOR_DIFF) {
	uint32_t mode;
	

	mode = MDIFF_VECTOR_DIFF[row / block][col / block].MODE;

	for (int i = 0; i < block; i++) {
		for (int j = 0; j < block; j++) {


			if (mode == HORIZONTAL) {
				(col == 0) ? (REF_FRAME[row + i][col + j] = 128) : (REF_FRAME[row + i][col + j] = REC_FRAME[row + i][col - 1]);
			}
			else if (mode == VERTICAL)
			{
				(row == 0) ? (REF_FRAME[row + i][col + j] = 128) : (REF_FRAME[row + i][col + j] = REC_FRAME[row - 1][col + j]);
			}
			else {
				assert(mode == 0 || mode == 1);
			}
			REC_FRAME[row + i][col + j] = RES_FRAME[row + i][col + j] + REF_FRAME[row + i][col + j];
		}
	}
}

void ReconstructBlockDecodeP(uint8_t** REC_FRAME, int32_t** RES_FRAME, uint8_t** PREV_DEC_FRAME, uint8_t** REF_FRAME, int row, int col, int block, MDIFF** MDIFF_VECTOR_DIFF) {
	int32_t gmvx, gmvy;
	for (int i = 0; i < block; i++) {
		for (int j = 0; j < block; j++) {
			gmvx = MDIFF_VECTOR_DIFF[row / block][col / block].X;
			gmvy = MDIFF_VECTOR_DIFF[row / block][col / block].Y;
			REF_FRAME[row + i][ col + j] = PREV_DEC_FRAME[row + i + gmvy][col + j + gmvx];
			REC_FRAME[row + i][col + j] = RES_FRAME[row + i][col + j] + REF_FRAME[row + i][col + j];
		}
	}
}
