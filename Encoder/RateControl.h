#include <common.h>

int row_rate_control(int row, int targetBr, int RCflag, int width, int height, int FrameType, int block, int current_bitcount, int QP, uint32_t *BITCOUNT_ROW, double *BITCOUNT_ROW_PERCENT, int FirstPass);
int row_rate_control1(int row, int targetBr, int RCflag, int width, int height, int FrameType, int block, int current_bitcount);
void row_rate_control2(int row, int targetBr, int RCflag, int width, int height, int FrameType, int block, int current_bitcount,
	uint32_t *BITCOUNT_ROW, double *BITCOUNT_ROW_PERCENT, int FirstPass);


int row_rate_control(int row, int targetBr, int RCflag, int width, int height, int FrameType, int block, int current_bitcount, int QP, uint32_t *BITCOUNT_ROW, double *BITCOUNT_ROW_PERCENT, int FirstPass) {
	if (RCflag == 1) {
		QP = row_rate_control1(row, targetBr, RCflag, width, height, FrameType, block, current_bitcount);
		return QP;
	}
	else if (RCflag == 2) {
		row_rate_control2(row, targetBr, RCflag, width, height, FrameType, block, current_bitcount, BITCOUNT_ROW, BITCOUNT_ROW_PERCENT, FirstPass);
	}
	return QP;
}

int row_rate_control1(int row, int targetBr, int RCflag, int width, int height, int FrameType, int block, int current_bitcount) {
	if (row + block == height)
		return 0;
	int RCqp = 0;
	int frame_remaining_bits;
	int row_target;
	int number_of_row_remaining = (height - (row / block + 1)*block) / block;
	int table_value;

	frame_remaining_bits = (targetBr / (fps)-current_bitcount);
	row_target = frame_remaining_bits / number_of_row_remaining;

	for (int i = 0; i < 12; i++) {
		if (width == QCIF_WIDTH) {
			if (FrameType == IFRAME)
				table_value = QCIF_I_TABLE[i];
			else
				table_value = QCIF_P_TABLE[i];
		}
		else {
			if (FrameType == IFRAME)
				table_value = CIF_I_TABLE[i];
			else
				table_value = CIF_P_TABLE[i];
		}
		if (row_target > table_value) {
			RCqp = i;
			break;
		}
		if (i == 11)
			RCqp == i;
	}
	return RCqp;
}

void row_rate_control2(int row, int targetBr, int RCflag, int width, int height, int FrameType, int block, int current_bitcount, 
						uint32_t *BITCOUNT_ROW, double *BITCOUNT_ROW_PERCENT, int FirstPass) {
	double percentage = 0;
	if (row + block == height && FirstPass) {
		for (int i = 0; i < height; i = i + block) {
			double temp = (double(BITCOUNT_ROW[i / block]) / double(current_bitcount));
			percentage = temp + percentage;
			BITCOUNT_ROW_PERCENT[i/block] = (double(BITCOUNT_ROW[i / block]) / double(current_bitcount));
		}
	}
	return;
}
