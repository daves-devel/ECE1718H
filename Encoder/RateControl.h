#include <common.h>

int row_rate_control(int row, int targetBr, int RCflag, int width, int height, int FrameType, int block, int current_bitcount);


int row_rate_control(int row, int targetBr, int RCflag, int width, int height, int FrameType, int block, int current_bitcount) {
	int RCqp=0;
	int frame_remaining_bits;
	int row_target;
	int number_of_row_remaining = (height-(row/block+1)*block) / block;
	int table_value;

	frame_remaining_bits = (targetBr / (fps) - current_bitcount );
	row_target = frame_remaining_bits/ number_of_row_remaining;

	for (int i = 0; i < 12; i++) {
		if (width == QCIF_WIDTH) {
			if (FrameType == IFRAME)
				table_value = QCIF_I_TABLE[i];
			else
				table_value = QCIF_P_TABLE[i];
		}
		else {
			if (FrameType == PFRAME)
				table_value = CIF_I_TABLE[i];
			else
				table_value = CIF_P_TABLE[i];
		}
		if (row_target < table_value) {
			RCqp = i;
			break;
		}
		if (i == 11)
			RCqp == i;
	}
	return RCqp;

}
