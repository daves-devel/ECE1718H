#include <residual.h>      // std::ifstream
void residual(uint8_t * residual_mem, uint8_t* frame_mem, int block_size, int frame_width, int frame_height, uint8_t rounding_n){
	for(int y=0; y<frame_height; y=y+block_size){
		for(int x=0; x<frame_width; x=x+block_size){
			generate_residual(residual_mem, frame_mem, x, y, block_size, frame_width, rounding_n);
		}
	}
}

unsigned char rounding(uint8_t number, uint8_t multiple){
	uint8_t result;
	result = number + multiple/2;
	result -= result % multiple;
	return result;
}

void generate_residual(uint8_t * residual_mem, uint8_t* frame_mem, int x, int y, int block_size, int frame_width, uint8_t rounding_n){
	int start_position=x+y*frame_width;
	for(int i=0; i< block_size; i++){
		for(int j=0; j< block_size; j++){
 			residual_mem[i*frame_width+j+start_position]=frame_mem[i*frame_width+j+start_position]-10;
			residual_mem[i*frame_width+j+start_position] =rounding(residual_mem[i*frame_width+j+start_position], rounding_n);
		}
	}
}