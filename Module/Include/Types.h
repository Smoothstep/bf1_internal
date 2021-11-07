#pragma once

#include <array>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <bitset>
#include <queue>
#include <sstream>
#include <ostream>
#include <istream>
#include <mutex>
#include <atomic>
#include <immintrin.h>
#include <intrin.h>
#include <xmmintrin.h>

#undef min
#undef max

typedef unsigned char Byte;

namespace Hyper
{
	typedef int					Int;
	typedef unsigned int		Uint;
	typedef float				Float;
	typedef double				Double;
	typedef __m128				M128;
	typedef __m128i				M128i;
#ifdef __AVX2__
	typedef __m256				M256;
	typedef __m256i				M256i;
#endif
	typedef float				Float1x1[1][1];
	typedef float				Float2x2[2][2];
	typedef float				Float3x3[3][3];
	typedef float				Float4x4[4][4];
	typedef float				Float4[4];
	typedef float				Float3[3];
	typedef float				Float2[2];
	typedef int					int32;
	typedef int					Int32;
	typedef long long			Int64;
	typedef long long			int64;
	typedef unsigned long long	Uint64;
	typedef unsigned long long	uint64;
	typedef unsigned int		uint32;
	typedef unsigned int		Uint32;
	typedef unsigned char		uint8;
	typedef unsigned char		Uint8;
	typedef unsigned short		uint16;
	typedef unsigned short		Uint16;
	typedef char				int8;
	typedef char				Int8;
	typedef short				int16;
	typedef short				Int16;
	typedef bool				Bool;
}

enum EAxis
{
	X, Y, Z
};