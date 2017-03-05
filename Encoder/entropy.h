#include <common.h>
void entropy(unsigned char ** QTC_FRAME, unsigned char * COEFF_REORDER, int block);
void raster_to_diag(unsigned char ** QTC_FRAME, unsigned char * COEFF_REORDER, int block);

void entropy(unsigned char ** QTC_FRAME, unsigned char * COEFF_REORDER, int block) {
	raster_to_diag(QTC_FRAME, COEFF_REORDER, block);
}

void fprint_coeef(unsigned char ** in, unsigned char * out, int block, FILE* file);

void fprint_coeef(unsigned char ** in, unsigned char * out, int block, FILE* file) {
	for (int i = 0; i < block; i++) {
		for (int j = 0; j < block; j++) {
			fprintf(file, "%02x ", in[i][j]);
		}
		fprintf(file, "\n");
	}
	for (int i = 0; i < block*block; i++)
		fprintf(file, "%02x ", out[i]);
}

void raster_to_diag(unsigned char ** QTC_FRAME, unsigned char * COEFF_REORDER,int block) {
	int x = 0;
	int index = 0;
	for (int diag = 0; diag < (block * 2) - 1; diag++) {
		if (diag < block) {
			x = diag;
			for (int y = 0; y < diag + 1; y++) {
				COEFF_REORDER[index] = QTC_FRAME[y][x];
				printf("%x ", COEFF_REORDER[index]);
				index++;
				x = x - 1;
			}
		}
		else {
			x = block-1;
			for (int y = diag-block+1; y < block; y++) {
				COEFF_REORDER[index] = QTC_FRAME[y][x];
				printf("%x ", COEFF_REORDER[index]);
				index++;
				x = x - 1;
			}
		}
	}
}