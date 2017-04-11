#ifndef __COMMON_H__
#define __COMMON_H__
#define _USE_MATH_DEFINES
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>   
#include <stdlib.h> 
#include <stdint.h>
#include <algorithm>
#include <string.h>
#include <math.h>
#include <cmath>

FILE* coeff_bitcount_file;
FILE* mdiff_bitcount_file;
FILE* total_bitcount_file;
FILE* frame_header_file;
FILE* golomb_file;
FILE* mdiff_golomb;
char golomb_name[500] = "";
char filepath[500] = "";
char mdiff_name[500] = "";
int VBSEnable = 0;
int fps = 30;
int QCIF_WIDTH = 176;
double QPMultiplier = 0;


enum FRAME {
	IFRAME = 0,
	PFRAME = 1
};

enum INTRAMODE {
	HORIZONTAL = 0,
	VERTICAL = 1
};

enum INTERMODE {
	DEFAULT = 0,
	RDO     = 1,
	FME	    = 2
};

struct MDIFF {
	int X, X2, X3, X4; // For Inter GMV
	int Y, Y2, Y3, Y4; // For Inter GMV
	int MODE, MODE2, MODE3, MODE4; // Intra Mode
	unsigned int SAD;
	unsigned int NORM;
	int ref=1;
	int ref2 = 1;
	int ref3 = 1;
	int ref4 = 1;
	int split=0;
};

const uint32_t EVX_SEXP_GOLOMB_CODES[] = {
	0x00001, 0x00002, 0x00004, 0x0000c, 0x00008, 0x00028, 0x00018, 0x00038,
	0x00010, 0x00090, 0x00050, 0x000d0, 0x00030, 0x000b0, 0x00070, 0x000f0,
	0x00020, 0x00220, 0x00120, 0x00320, 0x000a0, 0x002a0, 0x001a0, 0x003a0,
	0x00060, 0x00260, 0x00160, 0x00360, 0x000e0, 0x002e0, 0x001e0, 0x003e0,
	0x00040, 0x00840, 0x00440, 0x00c40, 0x00240, 0x00a40, 0x00640, 0x00e40,
	0x00140, 0x00940, 0x00540, 0x00d40, 0x00340, 0x00b40, 0x00740, 0x00f40,
	0x000c0, 0x008c0, 0x004c0, 0x00cc0, 0x002c0, 0x00ac0, 0x006c0, 0x00ec0,
	0x001c0, 0x009c0, 0x005c0, 0x00dc0, 0x003c0, 0x00bc0, 0x007c0, 0x00fc0,
	0x00080, 0x02080, 0x01080, 0x03080, 0x00880, 0x02880, 0x01880, 0x03880,
	0x00480, 0x02480, 0x01480, 0x03480, 0x00c80, 0x02c80, 0x01c80, 0x03c80,
	0x00280, 0x02280, 0x01280, 0x03280, 0x00a80, 0x02a80, 0x01a80, 0x03a80,
	0x00680, 0x02680, 0x01680, 0x03680, 0x00e80, 0x02e80, 0x01e80, 0x03e80,
	0x00180, 0x02180, 0x01180, 0x03180, 0x00980, 0x02980, 0x01980, 0x03980,
	0x00580, 0x02580, 0x01580, 0x03580, 0x00d80, 0x02d80, 0x01d80, 0x03d80,
	0x00380, 0x02380, 0x01380, 0x03380, 0x00b80, 0x02b80, 0x01b80, 0x03b80,
	0x00780, 0x02780, 0x01780, 0x03780, 0x00f80, 0x02f80, 0x01f80, 0x03f80,
	0x10100, 0x07f80, 0x05f80, 0x06f80, 0x04f80, 0x07780, 0x05780, 0x06780,
	0x04780, 0x07b80, 0x05b80, 0x06b80, 0x04b80, 0x07380, 0x05380, 0x06380,
	0x04380, 0x07d80, 0x05d80, 0x06d80, 0x04d80, 0x07580, 0x05580, 0x06580,
	0x04580, 0x07980, 0x05980, 0x06980, 0x04980, 0x07180, 0x05180, 0x06180,
	0x04180, 0x07e80, 0x05e80, 0x06e80, 0x04e80, 0x07680, 0x05680, 0x06680,
	0x04680, 0x07a80, 0x05a80, 0x06a80, 0x04a80, 0x07280, 0x05280, 0x06280,
	0x04280, 0x07c80, 0x05c80, 0x06c80, 0x04c80, 0x07480, 0x05480, 0x06480,
	0x04480, 0x07880, 0x05880, 0x06880, 0x04880, 0x07080, 0x05080, 0x06080,
	0x04080, 0x01fc0, 0x017c0, 0x01bc0, 0x013c0, 0x01dc0, 0x015c0, 0x019c0,
	0x011c0, 0x01ec0, 0x016c0, 0x01ac0, 0x012c0, 0x01cc0, 0x014c0, 0x018c0,
	0x010c0, 0x01f40, 0x01740, 0x01b40, 0x01340, 0x01d40, 0x01540, 0x01940,
	0x01140, 0x01e40, 0x01640, 0x01a40, 0x01240, 0x01c40, 0x01440, 0x01840,
	0x01040, 0x007e0, 0x005e0, 0x006e0, 0x004e0, 0x00760, 0x00560, 0x00660,
	0x00460, 0x007a0, 0x005a0, 0x006a0, 0x004a0, 0x00720, 0x00520, 0x00620,
	0x00420, 0x001f0, 0x00170, 0x001b0, 0x00130, 0x001d0, 0x00150, 0x00190,
	0x00110, 0x00078, 0x00058, 0x00068, 0x00048, 0x0001c, 0x00014, 0x00006,
};

const uint8_t EVX_SEXP_GOLOMB_SIZE_LUT[] = {
	1,  3,  5,  5,  7,  7,  7,  7,  9,  9,  9,  9,  9,  9,  9,  9,
	11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	17, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
	11, 9,  9,  9,  9,  9,  9,  9,  9,  7,  7,  7,  7,  5,  5,  3,
};

const uint8_t EVX_LOG2_BYTE_LUT[] = {
	0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
};

void write_mat(FILE *fp, uint8_t**m, int N, int M) {

	int i, j;
	for (i = 0; i< N; i++) {
		fprintf(fp, "%d", m[i][0]);
		for (j = 1; j < M; j++) {
			fprintf(fp, ",%d", m[i][j]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
}

void write_mat2(FILE *fp, int32_t**m, int N, int M) {

	int i, j;
	for (i = 0; i< N; i++) {
		fprintf(fp, "%d", m[i][0]);
		for (j = 1; j < M; j++) {
			fprintf(fp, ",%d", m[i][j]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
}

void write_mat3(FILE *fp, int32_t**m, int N, int M) {

	int i, j;
	for (i = 0; i< N; i++) {
		fprintf(fp, "%d", m[i][0]);
		for (j = 1; j < M; j++) {
			fprintf(fp, ",%d", m[i][j]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
}

void write_MV(FILE *fp, MDIFF**m, int N, int M, int block, int frame) {

	int i, j;
	fprintf(fp, "Frame:%d\n", frame);

	for (i = 0; i*block< N; i++) {
		for (j = 0; j*block < M; j++) {
			fprintf(fp, "Block X %d Y %d\n", j, i);
			if (m[i][j].split == 0)
				fprintf(fp, "X0 %d Y0 %d\n", m[i][j].X, m[i][j].Y);
			else {
				fprintf(fp, "X0 %d Y0 %d X1 %d Y1 %d X2 %d Y2 %d X3 %d Y3 %d\n", m[i][j].X, m[i][j].Y, m[i][j].X2, m[i][j].Y2, m[i][j].X3, m[i][j].Y3, m[i][j].X4, m[i][j].Y4);
			}
		}
	}
	fprintf(fp, "\n");
}

void write_SPLIT(FILE *fp, MDIFF**m, int N, int M, int block, int SecondPass, int frame) {

	int i, j;
	fprintf(fp, "Frame:%d\n", frame);
	for (i = 0; i< N; i = block + i) {
		for (j = 0; j < M; j = j + block) {
		//	fprintf(fp, "SecondPass %d Block X %d Y %d SPLIT %d\n", SecondPass, j, i, m[i/block][j/block].split);
			fprintf(fp, "Block X %d Y %d SPLIT %d\n", j, i, m[i / block][j / block].split);

		}
	}
	fprintf(fp, "\n");
}
int CIF_I_TABLE[12] = { 22089,	15756,	10715,	6911,	4224,	2634,	1675,	1181,	892,	678,	212,	68 };//Modify QP 0 1 2 * 1.19
//int CIF_I_TABLE[12] = { 22089,	16756,	11715,	6911,	4224,	2634,	1675,	1181,	892,	678,	212,	68 };

int CIF_P_TABLE[12] = { 18582,	12991,	8739,	5497,	3267,	2102,	1623,	1507,	279,	238,	161,	125 };
//int CIF_P_TABLE[12] = { 19582,	15459,	10100,	5497,	3267,	2102,	1623,	1507,	279,	238,	161,	125 }; //Modify QP0 1 2  *1.19

int QCIF_I_TABLE[12] = { 12449,	9309,	6788,	4726,	3073,	1877,	1090,	683,	528,	326,	122,	35 };
int QCIF_P_TABLE[12] = { 9264,	6628,	4658,	3108,	1988,	1218,	809,	708,	197,	147,	120,	72 };

//int CIF_PFRAME_THRESHOLD[12] = { 351199,	245529,	165167,	103893,	61746,	39727,	30674,	28482,	5273,	4498,	3042,	2362};5% margin
//int CIF_PFRAME_THRESHOLD[12] = { 384647,	268913,	180897,	113787,	67626,	43511,	33596,	31194,	5775,	4926,	3332,	2587 };//15% margin
//int CIF_PFRAME_THRESHOLD[12] = { 97602,	283608,	192870,	124398,	76032,	47412,	30150,	21258,	16056,	12204,	3816,	1224 };//Use I-Frame as limit margin
int CIF_PFRAME_THRESHOLD[12] = { 437362,	311968,	212157,	136837,	83635,	52153,	33165,	23383,	17661,	13424,	4197,	1346 };//Use I-Frame 10% margin




#ifdef TRACE_ON
FILE* file_vector_org;
FILE* file_vector_aft;
FILE* file_qtc;
FILE* file_reorder;
FILE* file_rle;
#endif
//Entropy
#endif