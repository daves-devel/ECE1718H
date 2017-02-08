// Decoder.cpp : Defines the entry point for the console application.
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <residual.h>  
#include <math.h>       /* round, floor, ceil, trunc */
#include <stdint.h>
#include <reconstructed.h>

void parse_residual_file() {
}

void parse_gmv_file() {
}

int main()
{
	int frame_width = 352;
	int frame_height = 288;
	int frame_count = 300;
	int block_size = 8;
	uint8_t rounding_n = 2;
	int frame_size_y = frame_width*frame_height;
	FILE *fgmv;
	FILE *fi;
	FILE *fo;
	FILE *fo2;
	fi = fopen("C:\\Users\\david.chakkuthara\\Documents\\Educational\\ECE1718H\\Assignment_1\\ECE1718H-master\\resource\\foreman_cif.yuv", "rb");//TODO change path name 
	fo = fopen("C:\\Users\\david.chakkuthara\\Documents\\Educational\\ECE1718H\\Assignment_1\\ECE1718H-master\\resource\\residual_dump", "wb");
	fo2 = fopen("C:\\Users\\david.chakkuthara\\Documents\\Educational\\ECE1718H\\Assignment_1\\ECE1718H-master\\resource\\reconstructed_dump", "wb");
	uint8_t *frame_buffer;
	uint8_t *residual_buffer;
	uint8_t *recon_buffer;
	frame_buffer = static_cast<uint8_t *> (malloc(frame_size_y));
	residual_buffer = static_cast<uint8_t *> (malloc(frame_size_y));
	recon_buffer = static_cast<uint8_t *> (malloc(frame_size_y));
	fread(frame_buffer, 1, frame_size_y, fi);
	residual(residual_buffer, frame_buffer, block_size, frame_width, frame_height, rounding_n);
	fwrite(residual_buffer, 1, frame_size_y, fo);
	fclose(fo);
	recon(residual_buffer, recon_buffer, block_size, frame_width, frame_height);
	fwrite(recon_buffer, 1, frame_size_y, fo2);
	fclose(fo2);
	return 0;
}


