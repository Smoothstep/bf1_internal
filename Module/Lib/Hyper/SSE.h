#pragma once

#include "Defines.h"
#include "SSEConstants.h"

#define SHUFFLEMASK( A0,A1,B2,B3 )				( (A0) | ((A1)<<2) | ((B2)<<4) | ((B3)<<6) )
#define VectorSwizzle( Vec, X, Y, Z, W )		_mm_shuffle_ps( Vec, Vec, SHUFFLEMASK(X,Y,Z,W) )
#define VectorShuffle( Vec1, Vec2, X, Y, Z, W )	_mm_shuffle_ps( Vec1, Vec2, SHUFFLEMASK(X,Y,Z,W) )
#define VectorLoadAligned( Vector )				_mm_load_ps((Float*)Vector)
#define VectorStoreAligned( Vector, Result )	_mm_store_ps((Float*)Result, Vector)
#define VectorStoreFloat1( Vec, Ptr )			_mm_store_ss((Float*)(Ptr), Vec)
#define VectorReplicate( Vec, ElementIndex )	_mm_shuffle_ps( Vec, Vec, SHUFFLEMASK(ElementIndex,ElementIndex,ElementIndex,ElementIndex) )
#define VectorNegate( Vec )						_mm_sub_ps(_mm_setzero_ps(),Vec)
#define VectorAdd( Vec1, Vec2 )					_mm_add_ps( Vec1, Vec2 )
#define VectorAbs( Vec )						_mm_and_ps(Vec, SSE::SignMask)
#define VectorReciprocal(Vec)					_mm_rcp_ps(Vec)
#define VectorReciprocalSqrt(Vec)				_mm_rsqrt_ps( Vec )
#define VectorRegister(X, Y, Z, W)				_mm_setr_ps((float)X, (float)Y, (float)Z, (float)W)
#define VectorSubtract( Vec1, Vec2 )			_mm_sub_ps( Vec1, Vec2 )
#define VectorMultiply( Vec1, Vec2 )			_mm_mul_ps( Vec1, Vec2 )
#define VectorZero()							_mm_setzero_ps()
#define VectorOne()								(SSE::FloatOne)
#define VectorLoad( Ptr )						_mm_loadu_ps( (float*)(Ptr) )
#define VectorLoadFloat3( Ptr )					VectorRegister( ((const float*)(Ptr))[0], ((const float*)(Ptr))[1], ((const float*)(Ptr))[2], 0.0f )
#define VectorLoadFloat3_W0( Ptr )				VectorRegister( ((const float*)(Ptr))[0], ((const float*)(Ptr))[1], ((const float*)(Ptr))[2], 0.0f )
#define VectorLoadFloat3_W1( Ptr )				VectorRegister( ((const float*)(Ptr))[0], ((const float*)(Ptr))[1], ((const float*)(Ptr))[2], 1.0f )
#define VectorCompareEQ( Vec1, Vec2 )			_mm_cmpeq_ps( Vec1, Vec2 )
#define VectorCompareNE( Vec1, Vec2 )			_mm_cmpneq_ps( Vec1, Vec2 )
#define VectorCompareGT( Vec1, Vec2 )			_mm_cmpgt_ps( Vec1, Vec2 )
#define VectorCompareGE( Vec1, Vec2 )			_mm_cmpge_ps( Vec1, Vec2 )
#define VectorLoadFloat1( Ptr )					_mm_load1_ps((float*)Ptr)
#define VectorMultiplyAdd( Vec1, Vec2, Vec3 )	_mm_add_ps( _mm_mul_ps(Vec1, Vec2), Vec3 )
#define VectorSet_W0( Vec )						_mm_and_ps( Vec, SSE::XYZMask )
#define VectorAnyGreaterThan( Vec1, Vec2 )		_mm_movemask_ps( _mm_cmpgt_ps(Vec1, Vec2) )
#define VectorMoveMask( Vec )					_mm_movemask_ps( Vec )
#define Matrix4X2Load(Matrix)					_mm256_loadu_ps((const Float*)Matrix)
#define Matrix4X2Store(Matrix, Result)			_mm256_store_ps((Float*)Result, Matrix4X2Load(&Matrix))
#define Matrix4X2ResetRegisters					_mm256_zeroupper
#define Matrix4X2Compare(L, R, O)				_mm256_cmp_ps(Matrix4X2Load(&L), Matrix4X2Load(&R), O)
#define Matrix4X2MoveMask(Matrix)				_mm256_movemask_ps(Matrix)

FORCEINLINE __m128 MakeVectorRegister
(
	float X, 
	float Y, 
	float Z, 
	float W
)
{
	return _mm_setr_ps(X, Y, Z, W);
}

FORCEINLINE __m128 MakeVectorRegister
(
	unsigned long X, 
	unsigned long Y, 
	unsigned long Z, 
	unsigned long W
)
{
	union 
	{ 
		__m128 v; 
		__m128i i; 
	} Tmp;

	Tmp.i = _mm_setr_epi32(X, Y, Z, W);

	return Tmp.v;
}

FORCEINLINE __m128 VectorSet_W1(const __m128& Vector)
{
	return VectorShuffle(Vector, _mm_movehl_ps(VectorOne(), Vector), 0, 1, 0, 3);
}