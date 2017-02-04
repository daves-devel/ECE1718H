#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <residual.h>  
#include <math.h>       /* round, floor, ceil, trunc */
#include <stdint.h>
#include <reconstructed.h>

int main()
{
	int frame_width=176;
	int frame_height=144;
	int frame_count=300;
	int block_size=8;
	uint8_t rounding_n=2;
	int frame_size_y=frame_width*frame_height;
	FILE *fi;
	FILE *fo;
	FILE *fo2;
	fi = fopen("C:\\Users\\Juan Eloy\\Desktop\\akiyo_qcif.yuv", "rb");//TODO change path name 
	fo = fopen("C:\\Users\\Juan Eloy\\Desktop\\residual_dump", "wb");
	fo2 = fopen("C:\\Users\\Juan Eloy\\Desktop\\reconstructed_dump", "wb");
	uint8_t *frame_buffer;
	uint8_t *residual_buffer;
	uint8_t *recon_buffer;
	frame_buffer = static_cast<uint8_t *> (malloc(frame_size_y));
	residual_buffer = static_cast<uint8_t *> (malloc(frame_size_y));
	recon_buffer = static_cast<uint8_t *> (malloc(frame_size_y));
	fread(frame_buffer,1,frame_size_y,fi);
	residual(residual_buffer, frame_buffer, block_size, frame_width, frame_height, rounding_n);
	fwrite(residual_buffer, 1,  frame_size_y, fo);
	recon(residual_buffer, recon_buffer, block_size, frame_width, frame_height);
	return 0;
}
