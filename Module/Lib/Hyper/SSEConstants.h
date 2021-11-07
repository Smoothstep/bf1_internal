#pragma once

#ifdef __INTEL_COMPILER
#define __INTEL_COMPILER_USE_INTRINSIC_PROTOTYPES
#endif

#include <immintrin.h>
#include <xmmintrin.h>

namespace SSE
{
	extern const __m128 SignBit;
	extern const __m128 SignMask;
	extern const __m128 XYZMask;
	extern const __m128 FloatOne;
	extern const __m128 FloatOneHalf;
	extern const __m128 FloatMinus1_111;
	extern const __m128 Float111_Minus1;
	extern const __m128 Float0001;
	extern const __m128 SmallLengthThreshold;
}