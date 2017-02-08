// Decoder.cpp : Parses residual file and MV file (that were generated in the encoding process) and combines the values to generate a Y-only-decoded file.
//				 Decoded files will be compared against the Y - only - reconstructed files

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

void create_y_file() {
}

void parse_reconstructed() {

}

void compare_decode_recon() {

}

//#include <iostream>
//#include <algorithm>
//#include <boost/iostreams/device/mapped_file.hpp>
//	namespace io = boost::iostreams;
//	int main()
//	{
//		io::mapped_file_source f1("test.1");
//		io::mapped_file_source f2("test.2");
//
//		if (f1.size() == f2.size()
//			&& std::equal(f1.data(), f1.data() + f1.size(), f2.data())
//			)
//			std::cout << "The files are equal\n";
//		else
//			std::cout << "The files are not equal\n";
//	}

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


