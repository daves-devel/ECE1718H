#include <common.h>

void recon(int8_t * residual_mem, uint8_t* recon_mem, int block_size, int frame_width, int frame_height, unsigned char** MOTION_FRAME);
void generate_recon(int8_t * residual_mem, uint8_t* recon_mem, int x, int y, int block_size, int frame_width, unsigned char** MOTION_FRAME);

void ReconstructBlock(uint8_t** REC_FRAME,int8_t** RES_FRAME, uint8_t** REF_FRAME,int row, int col, int block);

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
			REC_FRAME[row + i][col + i] = RES_FRAME[row + i][col + i] + REF_FRAME[row + i][col + i];
		}
	}
}
