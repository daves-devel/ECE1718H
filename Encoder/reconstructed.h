#include <common.h>
#include <assert.h>
void recon(int8_t * residual_mem, uint8_t* recon_mem, int block_size, int frame_width, int frame_height, unsigned char** MOTION_FRAME);
void generate_recon(int8_t * residual_mem, uint8_t* recon_mem, int x, int y, int block_size, int frame_width, unsigned char** MOTION_FRAME);

void ReconstructBlock(uint8_t** REC_FRAME,int8_t** RES_FRAME, uint8_t** REF_FRAME,int row, int col, int block);
void ReconstructBlockDecodeI(uint8_t** REC_FRAME, int8_t** RES_FRAME, int row, int col, int block, MDIFF** MDIFF_VECTOR_DIFF);
void ReconstructBlockDecodeP(uint8_t** REC_FRAME, int8_t** RES_FRAME, uint8_t** REF_FRAME, int row, int col, int block, MDIFF** MDIFF_VECTOR_DIFF);

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

void ReconstructBlock(uint8_t** REC_FRAME, int8_t** RES_FRAME, uint8_t** REF_FRAME, int row, int col, int block) {
	for (int i = 0; i < block; i++) {
		for (int j = 0; j < block; j++) {
			REC_FRAME[row + i][col + j] = RES_FRAME[row + i][col + j] + REF_FRAME[row + i][col + j];
		}
	}
}

void ReconstructBlockDecodeI(uint8_t** REC_FRAME, int8_t** RES_FRAME, int row, int col, int block, MDIFF** MDIFF_VECTOR_DIFF) {
	uint32_t mode;
	uint8_t** REF_BLOCK = new uint8_t*[block];
	for (unsigned int row = 0; row < block; row++) {
		REF_BLOCK[row] = new uint8_t[block];
	}

	mode = MDIFF_VECTOR_DIFF[row / block][col / block].MODE;

	for (int i = 0; i < block; i++) {
		for (int j = 0; j < block; j++) {


			if (mode == HORIZONTAL) {
				(col == 0) ? (REF_BLOCK[i][j] = 128) : (REF_BLOCK[i][j] = REC_FRAME[row + i][col - 1]);
			}
			else if (mode == VERTICAL)
			{
				(row == 0) ? (REF_BLOCK[i][j] = 128) : (REF_BLOCK[i][j] = REC_FRAME[row - 1][col + j]);
			}
			else {
				assert(mode == 0 || mode == 1);
			}
			REC_FRAME[row + i][col + j] = RES_FRAME[row + i][col + j] + REF_BLOCK[i][j];
		}
	}

	for (unsigned int row = 0; row < block; row++) {
		delete		REF_BLOCK[row];
	}
	delete		REF_BLOCK;
}

void ReconstructBlockDecodeP(uint8_t** REC_FRAME, int8_t** RES_FRAME, uint8_t** REF_FRAME, int row, int col, int block, MDIFF** MDIFF_VECTOR_DIFF) {
	for (int i = 0; i < block; i++) {
		for (int j = 0; j < block; j++) {
			int32_t gmvx, gmvy;
			gmvx = MDIFF_VECTOR_DIFF[row / block][col / block].X;
			gmvy = MDIFF_VECTOR_DIFF[row / block][col / block].Y;
			REC_FRAME[row + i][col + j] = RES_FRAME[row + i][col + j] + REF_FRAME[row + i + gmvy][col + j + gmvx];
		}
	}
}
