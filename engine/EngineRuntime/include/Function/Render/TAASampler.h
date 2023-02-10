#pragma once
#define TAA_SAMPLER_COUNT 8

static const double Halton_2[TAA_SAMPLER_COUNT] =
{
	0.0,
	-1.0 / 2.0,
	1.0 / 2.0,
	-3.0 / 4.0,
	1.0 / 4.0,
	-1.0 / 4.0,
	3.0 / 4.0,
	-7.0 / 8.0
};

// 8x TAA, (-1, 1)
static const double Halton_3[TAA_SAMPLER_COUNT] =
{
	-1.0 / 3.0,
	1.0 / 3.0,
	-7.0 / 9.0,
	-1.0 / 9.0,
	5.0 / 9.0,
	-5.0 / 9.0,
	1.0 / 9.0,
	7.0 / 9.0
};