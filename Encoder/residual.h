#include <common.h>
void residual(int8_t * residual_mem, uint8_t * frame_mem, int block_size, int frame_width, int frame_height, uint8_t rounding_n, unsigned char** MOTION_FRAME);
void generate_residual(int8_t* residual_mem, uint8_t* frame_mem, int x, int y, int block_size, int frame_width, uint8_t rounding_n, unsigned char** MOTION_FRAME);
void GenerateResidualBlock(int8_t** residual_mem, uint8_t** frame_mem, uint8_t** REF_FRAME, int row, int col , int block);

void residual(int8_t * residual_mem, uint8_t* frame_mem, int block_size, int frame_width, int frame_height, uint8_t rounding_n, unsigned char** MOTION_FRAME) {
	for (int y = 0; y<frame_height; y = y + block_size) {
		for (int x = 0; x<frame_width; x = x + block_size) {
			generate_residual(residual_mem, frame_mem, x, y, block_size, frame_width, rounding_n, MOTION_FRAME);
		}
	}
}

unsigned char rounding(uint8_t number, uint8_t multiple) {
	uint8_t result;
	result = number + multiple / 2;
	result -= result % multiple;
	return result;
}

void generate_residual(int8_t * residual_mem, uint8_t* frame_mem, int x, int y, int block_size, int frame_width, uint8_t rounding_n, unsigned char** MOTION_FRAME) {
	int start_position = x + (y*frame_width);
	for (int i = 0; i< block_size; i++) {
		for (int j = 0; j< block_size; j++) {
			residual_mem[(i*frame_width) + j + start_position] = frame_mem[(i*frame_width) + j + start_position] - MOTION_FRAME[y + i][x + j];
			//residual_mem[(i*frame_width) + j + start_position] = rounding(residual_mem[i*frame_width + j + start_position], rounding_n);
		}
	}
}

void GenerateResidualBlock(int8_t ** RESIDUAL_FRAME, uint8_t** CUR_FRAME, uint8_t** REF_FRAME,int row, int col, int block) {
	for (int i = 0; i< block; i++) {
		for (int j = 0; j< block; j++) {
			RESIDUAL_FRAME[row + i][col + j] = CUR_FRAME[row + i][col + j] - REF_FRAME[row + i][col + j];
		}
	}
}