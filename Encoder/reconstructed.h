#include <stdint.h>
void recon(uint8_t * residual_mem, uint8_t* recon_mem, int block_size, int frame_width, int frame_height);
void generate_recon(uint8_t * residual_mem, uint8_t* recon_mem, int x, int y, int block_size, int frame_width);