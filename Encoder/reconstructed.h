#include <stdint.h>
void recon(int8_t * residual_mem, uint8_t* recon_mem, int block_size, int frame_width, int frame_height, unsigned char** MOTION_FRAME);
void generate_recon(int8_t * residual_mem, uint8_t* recon_mem, int x, int y, int block_size, int frame_width, unsigned char** MOTION_FRAME);