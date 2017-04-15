#include <common.h>
static const int MAX_QUANT = 60;

const int alphas[] = {
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 4, 4,
	5, 6, 7, 8, 9, 10,
	12, 13, 15, 17, 20,
	22, 25, 28, 32, 36,
	40, 45, 50, 56, 63,
	71, 80, 90, 101, 113,
	127, 144, 162, 182,
	203, 226, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255
};

const int betas[] = {
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 2, 2,
	2, 3, 3, 3, 3, 4,
	4, 4, 6, 6,
	7, 7, 8, 8, 9, 9,
	10, 10, 11, 11, 12,
	12, 13, 13, 14, 14,
	15, 15, 16, 16, 17,
	17, 18, 18,
	19, 20, 21, 22, 23, 24, 25, 26, 27
};

const int cs[] = {
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1,
	1, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 2, 3,
	3, 3, 4, 4, 5, 5,
	6, 7, 8, 8, 10,
	11, 12, 13, 15, 17,
	19, 21, 23, 25, 27, 29, 31, 33, 35
};

static inline int clip(int x, int min, int max)
{
	if (x < min) {
		return min;
	}
	if (x > max) {
		return max;
	}
	return x;
}


static inline void deblock_vertical_edge_c(uint8_t *srcp, int src_pitch, int alpha, int beta, int c0)
{
	for (int i = 0; i < 4; i++)
	{
		if ((std::abs(srcp[0] - srcp[-1]) < alpha) && (std::abs(srcp[1] - srcp[0]) < beta) && (std::abs(srcp[-1] - srcp[-2]) < beta))
		{
			int ap = std::abs(srcp[2] - srcp[0]);
			int aq = std::abs(srcp[-3] - srcp[-1]);
			int c = c0;
			if (aq < beta) c++;
			if (ap < beta) c++;

			int avg0 = (srcp[0] + srcp[-1] + 1) >> 1;

			int delta = clip((((srcp[0] - srcp[-1]) << 2) + (srcp[-2] - srcp[1]) + 4) >> 3, -c, c);
			int deltaq1 = clip((srcp[2] + avg0 - (srcp[1] << 1)) >> 1, -c0, c0);
			int deltap1 = clip((srcp[-3] + avg0 - (srcp[-2] << 1)) >> 1, -c0, c0);

			if (aq < beta) {
				srcp[-2] = (srcp[-2] + deltap1);
			}

			srcp[-1] = clip(srcp[-1] + delta, 0, 255);
			srcp[0] = clip(srcp[0] - delta, 0, 255);

			if (ap < beta) {
				srcp[1] = (srcp[1] + deltaq1);
			}
		}
		srcp += src_pitch;
	}
}

static inline void deblock_horizontal_edge_c(uint8_t *srcp, int srcPitch, int alpha, int beta, int c0)
{
	uint8_t *sq0 = srcp;
	uint8_t *sq1 = srcp + srcPitch;
	uint8_t *sq2 = srcp + 2 * srcPitch;
	uint8_t *sp0 = srcp - srcPitch;
	uint8_t *sp1 = srcp - 2 * srcPitch;
	uint8_t *sp2 = srcp - 3 * srcPitch;

	for (int i = 0; i < 4; i++)
	{
		if ((std::abs(sp0[i] - sq0[i]) < alpha) && (std::abs(sp1[i] - sp0[i]) < beta) && (std::abs(sq0[i] - sq1[i]) < beta))
		{
			int ap = std::abs(sp2[i] - sp0[i]);
			int aq = std::abs(sq2[i] - sq0[i]);
			int c = c0;
			if (aq < beta) c++;
			if (ap < beta) c++;

			int avg0 = (sp0[i] + sq0[i] + 1) >> 1;

			int delta = clip((((sq0[i] - sp0[i]) << 2) + (sp1[i] - sq1[i]) + 4) >> 3, -c, c);
			int deltap1 = clip((sp2[i] + avg0 - (sp1[i] << 1)) >> 1, -c0, c0);
			int deltaq1 = clip((sq2[i] + avg0 - (sq1[i] << 1)) >> 1, -c0, c0);

			if (ap < beta) {
				sp1[i] = sp1[i] + deltap1;
			}

			sp0[i] = clip(sp0[i] + delta, 0, 255);
			sq0[i] = clip(sq0[i] - delta, 0, 255);

			if (aq < beta) {
				sq1[i] = sq1[i] + deltaq1;
			}
		}
	}
}
static void deblock_c(uint8_t *srcp, int src_pitch, int width, int height, int alpha, int beta, int c0) {
	for (int x = 4; x < width; x += 4) {
		deblock_vertical_edge_c(srcp + x, src_pitch, alpha, beta, c0);
	}

	srcp += 4 * src_pitch;
	for (int y = 4; y < height; y += 4) {
		deblock_horizontal_edge_c(srcp, src_pitch, alpha, beta, c0);

		for (int x = 4; x < width; x += 4) {
			deblock_horizontal_edge_c(srcp + x, src_pitch, alpha, beta, c0);
			deblock_vertical_edge_c(srcp + x, src_pitch, alpha, beta, c0);
		}
		srcp += 4 * src_pitch;
	}
}

