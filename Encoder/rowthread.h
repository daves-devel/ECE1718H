#ifndef __ROWTHREAD_H__
#define __ROWTHREAD_H__
#include <common.h>
#include <residual.h>
#include <reconstructed.h>
#include <quantization.h>
#include <entropy.h>
#include <InterFramePrediction.h>
#include <IntraFramePrediction.h>
#include <discrete_cosine_transform.h>
#include <DiffEnc.h>
#include <blockthread.h>

void RowThread(
	int row,
	int width,
	int height,
	int FrameType,
	uint32_t INTERMODE,
	uint32_t PARALLELMODE,
	int range,
	int range_split,
	int block,
	int block_split,
	int i_period,
	int QP,
	int RDOEnable,
	int FMEnable,
	uint8_t** CUR_FRAME_2D,
	uint8_t** CUR_REC_FRAME_2D,
	uint8_t** PREV_REC_FRAME_2D,
	uint8_t** REF_FRAME_2D,
	int32_t** ENC_RES_FRAME_2D,
	int32_t** ENC_TC_FRAME_2D,
	int32_t** DEC_RES_FRAME_2D,
	int32_t** DEC_TC_FRAME_2D,
	int32_t** QTC_FRAME_2D,
	int32_t** QP_FRAME_2D,
	uint8_t** CUR_FRAME_2DS,
	uint8_t** CUR_REC_FRAME_2DS,
	uint8_t** PREV_REC_FRAME_2DS,
	uint8_t** REF_FRAME_2DS,
	int32_t** ENC_RES_FRAME_2DS,
	int32_t** ENC_TC_FRAME_2DS,
	int32_t** DEC_RES_FRAME_2DS,
	int32_t** DEC_TC_FRAME_2DS,
	int32_t** QTC_FRAME_2DS,
	int32_t** QP_FRAME_2DS,
	MDIFF** MDIFF_VECTOR,
	MDIFF** MDIFF_VECTOR_DIFF,
	MDIFF** MDIFF_VECTORS,
	MDIFF** MDIFF_VECTOR_DIFFS
) {

	if (PARALLELMODE == FRAMETHREADED) {
		for (int framerow = row; framerow <= row + block; framerow+= block) {
			for (int col = 0; col < width; col += block) {
				BlockThread(framerow, col,
					width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
					CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
					CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
					MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
				);
			}
		}
	}
	if (PARALLELMODE == SUPERTHREADED) {
		for (int col = 0; col <= width; col += block) {

			if (col == 0) { // First Block in First Row
				std::thread firstblock(BlockThread, row, col,
						width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
						CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
						CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
						MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
				);

				firstblock.join();
			}

			else if (col == width) { // Last Block in Second Row
				std::thread lastblock(BlockThread, row + block, col - block,
						width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
						CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
						CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
						MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
				);
				lastblock.join();
			}

			else {// Two blocks in Current Section
				std::thread rightblock(BlockThread, row, col,
						width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
						CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
						CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
						MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
				);
				std::thread bottomblock(BlockThread, row + block, col - block,
						width, height, FrameType, INTERMODE, range, range_split, block, block_split, i_period, QP, RDOEnable, FMEnable,
						CUR_FRAME_2D, CUR_REC_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, ENC_RES_FRAME_2D, ENC_TC_FRAME_2D, DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D,
						CUR_FRAME_2DS, CUR_REC_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, ENC_RES_FRAME_2DS, ENC_TC_FRAME_2DS, DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS,
						MDIFF_VECTOR, MDIFF_VECTOR_DIFF, MDIFF_VECTORS, MDIFF_VECTOR_DIFFS
				);
				rightblock.join();
				bottomblock.join();

			}
		}
	}
}
#endif