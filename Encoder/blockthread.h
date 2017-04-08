#ifndef __BLOCKTHREAD_H__
#define __BLOCKTHREAD_H__
#include <common.h>
#include <residual.h>
#include <reconstructed.h>
#include <quantization.h>
#include <entropy.h>
#include <InterFramePrediction.h>
#include <IntraFramePrediction.h>
#include <discrete_cosine_transform.h>
#include <DiffEnc.h>

void BlockThread(
	int row,
	int col,
	int width,
	int height,
	int FrameType,
	uint32_t INTERMODE,
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
	int32_t** ENC_TC_FRAME_2D ,
	int32_t** DEC_RES_FRAME_2D,
	int32_t** DEC_TC_FRAME_2D,
	int32_t** QTC_FRAME_2D ,
	int32_t** QP_FRAME_2D ,
	uint8_t** CUR_FRAME_2DS ,
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
	){

	// PREDICTOR DATA GENERATION
	if (FrameType == IFRAME) {
		MDIFF_VECTOR[row / block][col / block] = IntraFramePrediction(CUR_FRAME_2D, CUR_REC_FRAME_2D, REF_FRAME_2D, row, col, block);
	}

	if (FrameType == PFRAME) {
		MDIFF_VECTOR[row / block][col / block] = InterFramePrediction(INTERMODE, CUR_FRAME_2D, PREV_REC_FRAME_2D, REF_FRAME_2D, row, col, width, height, block, range, 1, MDIFF_VECTOR);
	}

	// RESIDUAL 
	GenerateResidualBlock(ENC_RES_FRAME_2D, CUR_FRAME_2D, REF_FRAME_2D, row, col, block);

	// DCT 
	DCTBlock(ENC_TC_FRAME_2D, ENC_RES_FRAME_2D, row, col, block);

	// QUANTIZE
	QuantizeBlock(QTC_FRAME_2D, ENC_TC_FRAME_2D, QP_FRAME_2D, row, col, width, height, QP, block);

	// SCALE
	ScaleBlock(DEC_TC_FRAME_2D, QTC_FRAME_2D, QP_FRAME_2D, row, col, width, height, QP, block);

	// IDCT
	IDCTBlock(DEC_RES_FRAME_2D, DEC_TC_FRAME_2D, row, col, block);

	// RECONSTRUCT 
	ReconstructBlock(CUR_REC_FRAME_2D, DEC_RES_FRAME_2D, REF_FRAME_2D, row, col, block);

	if (VBSEnable) {//start VBSEnable code
		for (int row2 = row; row2 < row + block; row2 += block_split) {
			for (int col2 = col; col2 < col + block; col2 += block_split) {

				// PREDICTOR DATA GENERATION
				if (FrameType == IFRAME) {
					MDIFF_VECTORS[row2 / block_split][col2 / block_split] = IntraFramePrediction(CUR_FRAME_2DS, CUR_REC_FRAME_2DS, REF_FRAME_2DS, row2, col2, block_split);
				}

				if (FrameType == PFRAME) {
					MDIFF_VECTORS[row2 / block_split][col2 / block_split] = InterFramePrediction(INTERMODE, CUR_FRAME_2DS, PREV_REC_FRAME_2DS, REF_FRAME_2DS, row2, col2, width, height, block_split, range_split, 1, MDIFF_VECTORS);
				}

				// RESIDUAL 
				GenerateResidualBlock(ENC_RES_FRAME_2DS, CUR_FRAME_2DS, REF_FRAME_2DS, row2, col2, block_split);

				// DCT 
				DCTBlock(ENC_TC_FRAME_2DS, ENC_RES_FRAME_2DS, row2, col2, block_split);

				// QUANTIZE
				QuantizeBlock(QTC_FRAME_2DS, ENC_TC_FRAME_2DS, QP_FRAME_2DS, row2, col2, width, height, QP, block_split);

				// SCALE
				ScaleBlock(DEC_TC_FRAME_2DS, QTC_FRAME_2DS, QP_FRAME_2DS, row2, col2, width, height, QP, block_split);

				// IDCT
				IDCTBlock(DEC_RES_FRAME_2DS, DEC_TC_FRAME_2DS, row2, col2, block_split);

				// RECONSTRUCT 
				ReconstructBlock(CUR_REC_FRAME_2DS, DEC_RES_FRAME_2DS, REF_FRAME_2DS, row2, col2, block_split);
			}
		}
		//Pick Winner
		VBSWinner(MDIFF_VECTOR, MDIFF_VECTORS, row, col, block, CUR_REC_FRAME_2D, CUR_REC_FRAME_2DS);
	}//End of VBSenable code

	return;

}

#endif
