#include <stdint.h>
void residual(uint8_t * residual_mem, uint8_t * frame_mem, int block_size, int frame_width, int frame_height, uint8_t rounding_n);
void generate_residual(uint8_t* residual_mem, uint8_t* frame_mem, int x, int y, int block_size, int frame_width, uint8_t rounding_n);