#include <stdint.h>
void residual(int8_t * residual_mem, uint8_t * frame_mem, int block_size, int frame_width, int frame_height, uint8_t rounding_n, unsigned char** MOTION_FRAME);
void generate_residual(int8_t* residual_mem, uint8_t* frame_mem, int x, int y, int block_size, int frame_width, uint8_t rounding_n, unsigned char** MOTION_FRAME);