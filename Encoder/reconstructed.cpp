#include <reconstructed.h>
void recon(uint8_t * residual_mem, uint8_t* recon_mem, int block_size, int frame_width, int frame_height){
	for(int y=0; y<frame_height; y=y+block_size){
		for(int x=0; x<frame_width; x=x+block_size){
			generate_recon(residual_mem, recon_mem, x, y, block_size, frame_width);
		}
	}
}

void generate_recon(uint8_t * residual_mem, uint8_t* recon_mem, int x, int y, int block_size, int frame_width){
	int start_position= x+y*frame_width;
	for(int i=0; i< block_size; i++){
		for(int j=0; j< block_size; j++){
			recon_mem[i*frame_width+j+start_position]=residual_mem[i*frame_width+j+start_position]+10;//TODO add prediction
		}
	}
}
