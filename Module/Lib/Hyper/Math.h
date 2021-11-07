#pragma once

#include <d3d12.h>

#include "MathDefines.h"
#include "Point.h"
#include "Types.h"
#include "SSE.h"

namespace Hyper
{
	struct RGColor;
	struct RGColor16;
	struct RGBColor;
	struct RGBColor16;
	struct RGBAColor;
	struct RGBAColor16;

	struct Vector2f;
	struct Vector3f;
	struct Vector4f;
	struct Plane;
	struct Quaternion;
	struct Matrix4x4;
	struct Rotation;

	namespace Math
	{
		static constexpr int Power10LookupTable[10] =
		{
			1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
		};

		static constexpr double FPower10LookupTable[10] =
		{
			1, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001, 0.00000001, 0.000000001
		};

		namespace SSE
		{
			static FORCEINLINE M128 MakeVectorRegister
			(
				const float X,
				const float Y,
				const float Z,
				const float W
			)
			{
				return _mm_setr_ps(X, Y, Z, W);
			}

			static FORCEINLINE M128i MakeVectorRegisterInt
			(
				const int32 X,
				const int32 Y,
				const int32 Z,
				const int32 W
			)
			{
				return _mm_setr_epi32(X, Y, Z, W);
			}

			static FORCEINLINE M128 MakeVectorRegister
			(
				uint32 X,
				uint32 Y,
				uint32 Z,
				uint32 W
			)
			{
				union
				{
					M128  v;
					M128i i;
				} Tmp;

				Tmp.i = _mm_setr_epi32(X, Y, Z, W);

				return Tmp.v;
			}

			static const M128 QMULTI_SIGN_MASK0 = MakeVectorRegister( 1.f,-1.f, 1.f,-1.f);
			static const M128 QMULTI_SIGN_MASK1 = MakeVectorRegister( 1.f, 1.f,-1.f,-1.f);
			static const M128 QMULTI_SIGN_MASK2 = MakeVectorRegister(-1.f, 1.f, 1.f,-1.f);

			static CONSTEXPR M128 VectorQuaternionMultiply2
			(
				const M128 & Quat1, 
				const M128 & Quat2
			)
			{
				M128 Result = VectorMultiply(VectorReplicate(Quat1, 3), Quat2);

				Result = VectorMultiplyAdd(VectorMultiply(VectorReplicate(Quat1, 0), VectorSwizzle(Quat2, 3, 2, 1, 0)), QMULTI_SIGN_MASK0, Result);
				Result = VectorMultiplyAdd(VectorMultiply(VectorReplicate(Quat1, 1), VectorSwizzle(Quat2, 2, 3, 0, 1)), QMULTI_SIGN_MASK1, Result);
				Result = VectorMultiplyAdd(VectorMultiply(VectorReplicate(Quat1, 2), VectorSwizzle(Quat2, 1, 0, 3, 2)), QMULTI_SIGN_MASK2, Result);

				return Result;
			}

			static CONSTEXPR void VectorQuaternionMultiply
			(
						void * RESTRICT Result, 
				const	void * RESTRICT Quat1, 
				const	void * RESTRICT Quat2)
			{
				*reinterpret_cast<M128*>(Result) = VectorQuaternionMultiply2
				(
					*reinterpret_cast<const M128 *>(Quat1),
					*reinterpret_cast<const M128 *>(Quat2)
				);
			}

			static CONSTEXPR M128 VectorSelect
			(
				const M128 & Mask, 
				const M128 & Vec1,
				const M128 & Vec2
			)
			{
				return _mm_xor_ps(Vec2, _mm_and_ps(Mask, _mm_xor_ps(Vec1, Vec2)));
			}

			static CONSTEXPR M128 VectorDot3
			(
				const M128 & Vec1, 
				const M128 & Vec2
			)
			{
				M128 Temp = VectorMultiply(Vec1, Vec2);
				return VectorAdd(VectorReplicate(Temp, 0), VectorAdd(VectorReplicate(Temp, 1), VectorReplicate(Temp, 2)));
			}

			static CONSTEXPR M128 VectorDot4
			(
				const M128 & Vec1, 
				const M128 & Vec2
			)
			{
				M128 Temp1 = VectorMultiply(Vec1, Vec2);
				M128 Temp2 = _mm_shuffle_ps(Temp1, Temp1, SHUFFLEMASK(2, 3, 0, 1));
				Temp1 = VectorAdd(Temp1, Temp2);
				Temp2 = _mm_shuffle_ps(Temp1, Temp1, SHUFFLEMASK(1, 2, 3, 0));
				return VectorAdd(Temp1, Temp2);
			}

			static CONSTEXPR M128 VectorReciprocalLen
			(
				const M128 & Vector
			)
			{
				M128 RecipLen = VectorDot4(Vector, Vector);
				return VectorReciprocalSqrt(RecipLen);
			}

			static CONSTEXPR M128 VectorReciprocalSqrtAccurate
			(
				const M128 & Vec
			)
			{
				const M128 OneHalf = ::SSE::FloatOneHalf;
				const M128 VecDivBy2 = VectorMultiply(Vec, OneHalf);

				const M128 x0 = VectorReciprocalSqrt(Vec);

				M128 x1 = VectorMultiply(x0, x0);
				x1 = VectorSubtract(OneHalf, VectorMultiply(VecDivBy2, x1));
				x1 = VectorMultiplyAdd(x0, x1, x0);

				M128 x2 = VectorMultiply(x1, x1);
				x2 = VectorSubtract(OneHalf, VectorMultiply(VecDivBy2, x2));
				x2 = VectorMultiplyAdd(x1, x2, x1);

				return x2;
			}

			static CONSTEXPR M128 VectorReciprocalAccurate
			(
				const M128 & Vec
			)
			{
				const M128 x0 = VectorReciprocal(Vec);

				const M128 x0Squared = VectorMultiply(x0, x0);
				const M128 x0Times2 = VectorAdd(x0, x0);
				const M128 x1 = VectorSubtract(x0Times2, VectorMultiply(Vec, x0Squared));

				const M128 x1Squared = VectorMultiply(x1, x1);
				const M128 x1Times2 = VectorAdd(x1, x1);
				const M128 x2 = VectorSubtract(x1Times2, VectorMultiply(Vec, x1Squared));

				return x2;
			}

			static CONSTEXPR M128 VectorNormalizeSafe
			(
				const M128 & Vector,
				const M128 & VectorDefaultValue
			)
			{
				const M128 SquareSum = VectorDot4(Vector, Vector);
				const M128 NonZeroMask = VectorCompareGE(SquareSum, ::SSE::SmallLengthThreshold);
				const M128 InvLength = VectorReciprocalSqrtAccurate(SquareSum);
				const M128 NormalizedVector = VectorMultiply(InvLength, Vector);
				return VectorSelect(NonZeroMask, NormalizedVector, VectorDefaultValue);
			}

			static CONSTEXPR M128 VectorNormalizeQuaternion
			(
				const M128 & UnnormalizedQuat
			)
			{
				return VectorNormalizeSafe(UnnormalizedQuat, ::SSE::Float0001);
			}

			static CONSTEXPR M128 VectorCross
			(
				const M128 & Vec1,
				const M128 & Vec2
			)
			{
				M128 A_YZXW = _mm_shuffle_ps(Vec1, Vec1, SHUFFLEMASK(1, 2, 0, 3));
				M128 B_ZXYW = _mm_shuffle_ps(Vec2, Vec2, SHUFFLEMASK(2, 0, 1, 3));
				M128 A_ZXYW = _mm_shuffle_ps(Vec1, Vec1, SHUFFLEMASK(2, 0, 1, 3));
				M128 B_YZXW = _mm_shuffle_ps(Vec2, Vec2, SHUFFLEMASK(1, 2, 0, 3));
				return VectorSubtract(VectorMultiply(A_YZXW, B_ZXYW), VectorMultiply(A_ZXYW, B_YZXW));
			}

			static FORCEINLINE M128 VectorPow
			(
				const M128 & Base, 
				const M128 & Exponent
			)
			{
				union 
				{ 
					M128 v; 
					Float4 f; 
				} B, E;
				B.v = Base;
				E.v = Exponent;
				return _mm_setr_ps(powf(B.f[0], E.f[0]), powf(B.f[1], E.f[1]), powf(B.f[2], E.f[2]), powf(B.f[3], E.f[3]));
			}
		}

		template
		<
			typename IntegerType
		>
		static CONSTEXPR IntegerType PreviousPowerOfTwo
		(
			IntegerType Integer
		)
		{
			Integer |= (Integer >> 1);
			Integer |= (Integer >> 2);
			Integer |= (Integer >> 4);
			Integer |= (Integer >> 8);
			Integer |= (Integer >> 16);
			return Integer - (Integer >> 1);
		}

		template
		<
			typename IntegerType
		>
		static CONSTEXPR IntegerType NextPowerOfTwo
		(
			IntegerType Integer
		)
		{
			Integer--;
			Integer |= (Integer >> 1);
			Integer |= (Integer >> 2);
			Integer |= (Integer >> 4);
			Integer |= (Integer >> 8);
			Integer |= (Integer >> 16);
			Integer++;
			return Integer - (Integer >> 1);
		}

		static CONSTEXPR Float TruncateToFloat
		(
			const int Integer
		)
		{
			return static_cast<float>(Integer);
		}

		static CONSTEXPR Int TruncateToInt
		(
			const Float FloatingPoint
		)
		{
			return static_cast<int>(FloatingPoint);
		}

		static CONSTEXPR Float TruncateToFloat
		(
			const Float FloatingPoint
		)
		{
			return
				static_cast<float>(
					static_cast<int>(FloatingPoint));
		}

		template<typename F> static FORCEINLINE F Floor
		(
			const F Value
		)
		{
			return std::floor(Value);
		}

		template<typename F> static FORCEINLINE F Ceil
		(
			const F Value
		)
		{
			return std::ceil(Value);
		}

		static FORCEINLINE Float GridSnap
		(
			const Float Location,
			const Float Grid
		)
		{
			if (Grid == 0.0f)
			{
				return Location;
			}

			return Floor(static_cast<float>((Location + 0.5f * Grid) / Grid)) * Grid;
		}

		static FORCEINLINE Float Log2
		(
			const Float Value
		)
		{
			return log2f(Value);
		}

		template
		<
			class T
		>
		static constexpr T Square
		(
			const T A
		)
		{
			return A * A;
		}

		template
		<
			class T
		>
		static constexpr T Clamp
		(
			const T X,
			const T Min,
			const T Max
		)
		{
			return	X < Min ? Min : 
					X < Max ? X : Max;
		}

		template
		<
			typename T
		>
		static CONSTEXPR int RoundToInt
		(
			const T FloatingPoint
		)
		{
			int Value = 0;
#ifdef __INTEL_COMPILER
			__asm 
			{
				fld		FloatingPoint
				fistp	Value
			};
#else
			Value = _mm_cvt_ss2si(_mm_set_ss(FloatingPoint + FloatingPoint + 0.5f)) >> 1;
#endif
			return Value;
		}

		template
		<
			class T
		>
		static constexpr T Abs
		(
			const T N
		)
		{
			return N < 0 ? -N : N;
		}

		FORCEINLINE float Sqrt
		(
			const Float FloatingPoint
		)
		{
			return std::sqrt(FloatingPoint);
		}

		FORCEINLINE float Exp2
		(
			const Float FloatingPoint
		)
		{
			return powf(2.0f, FloatingPoint);
		}

		extern FORCEINLINE float Exp
		(
			const Float FloatingPoint
		)
		{
			return expf(FloatingPoint);
		}

		static constexpr float FloatSelect
		(
			const Float Comparand,
			const Float ValueGEZero,
			const Float ValueLTZero
		)
		{
			return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
		}

		template
		<
			typename T
		>
		static constexpr T Max
		(
			const T L,
			const T R
		)
		{
			return L < R ? R : L;
		}

		template
		<
			typename T
		>
		static constexpr T Min
		(
			const T L,
			const T R
		)
		{
			return L > R ? R : L;
		}

		template
		<
			class T
		>
		static constexpr auto RadiansToDegrees
		(
			T const& RadVal
		) 
			-> decltype(RadVal * (180.0f / PI))
		{
			return RadVal * (180.0f / PI);
		}

		template
		<
			class T
		>
		static constexpr auto DegreesToRadians
		(
			T const& DegVal
		) 
			-> decltype(DegVal * (PI / 180.0f))
		{
			return DegVal * (PI / 180.0f);
		}

		static constexpr void SinCos
		(
			OUT			Float *	ScalarSin,
			OUT			Float *	ScalarCos,
			const		Float	Value)
		{
			float quotient = (INV_PI * 0.5f) * Value;

			if (Value >= 0.0f)
			{
				quotient = (float)((int)(quotient + 0.5f));
			}
			else
			{
				quotient = (float)((int)(quotient - 0.5f));
			}

			float y = Value - (2.0f * PI) * quotient;
			float sign = 0.0f;

			if (y > HALF_PI)
			{
				y = PI - y;
				sign = -1.0f;
			}
			else if (y < -HALF_PI)
			{
				y = -PI - y;
				sign = -1.0f;
			}
			else
			{
				sign = +1.0f;
			}

			const float y2 = y * y;
			*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

			const float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
			*ScalarCos = sign*p;
		}

		static CONSTEXPR Float InvSqrt
		(
			const Float F
		)
		{
			const M128 fOneHalf = _mm_set_ss(0.5f);
			float temp = 0.0f;

			M128 Y0 = _mm_set_ss(F);
			M128 X0 = _mm_rsqrt_ss(Y0);
			M128 FOver2 = _mm_mul_ss(Y0, fOneHalf);

			M128	X1 = _mm_mul_ss(X0, X0);
					X1 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X1));
					X1 = _mm_add_ss(X0, _mm_mul_ss(X0, X1));

			M128	X2 = _mm_mul_ss(X1, X1);
					X2 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X2));
					X2 = _mm_add_ss(X1, _mm_mul_ss(X1, X2));

			_mm_store_ss(&temp, X2);
			return temp;
		}

		static CONSTEXPR Float InvSqrtEst
		(
			const Float F
		)
		{
			const M128 fOneHalf = _mm_set_ss(0.5f);
			float temp = 0.0f;

			M128 Y0 = _mm_set_ss(F);
			M128 X0 = _mm_rsqrt_ss(Y0);

			M128 FOver2 = _mm_mul_ss(Y0, fOneHalf);

			M128	X1 = _mm_mul_ss(X0, X0);
					X1 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X1));
					X1 = _mm_add_ss(X0, _mm_mul_ss(X0, X1));

			_mm_store_ss(&temp, X1);
			return temp;
		}

		template
		<
			class MatrixType
		>
		static CONSTEXPR M128 VectorTransformVector
		(
			const M128			& VecP,
			const MatrixType	& MatrixM
		)
		{
			const M128 *M = (const M128 *)&MatrixM;

			M128 VTempX = VectorReplicate(VecP, 0);
			M128 VTempY = VectorReplicate(VecP, 1);
			M128 VTempZ = VectorReplicate(VecP, 2);
			M128 VTempW = VectorReplicate(VecP, 3);

			VTempX = VectorMultiply(VTempX, M[0]);
			VTempY = VectorMultiply(VTempY, M[1]);
			VTempZ = VectorMultiply(VTempZ, M[2]);
			VTempW = VectorMultiply(VTempW, M[3]);

			VTempX = VectorAdd(VTempX, VTempY);
			VTempZ = VectorAdd(VTempZ, VTempW);
			VTempX = VectorAdd(VTempX, VTempZ);

			return VTempX;
		}

		CONSTEXPR M128 Lincomb_SSE
		(
			const M128	Source[4],
			const M128&	A01
		)
		{
			M128 Result;
			{
				Result = _mm_mul_ps(_mm_shuffle_ps(A01, A01, 0x00), Source[0]);
				Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(A01, A01, 0x55), Source[1]));
				Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(A01, A01, 0xaa), Source[2]));
				Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(A01, A01, 0xff), Source[3]));
			}
			return Result;
		}

#ifdef __AVX2__
		static CONSTEXPR M256 TwoLincomb_AVX_8
		(
			const M128	 Source[4],
			const M256 & A01
		)
		{
			M256 Result = _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0x00), _mm256_broadcast_ps(&Source[0]));
			{
				Result = _mm256_add_ps(Result, _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0x55), _mm256_broadcast_ps(&Source[1])));
				Result = _mm256_add_ps(Result, _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0xaa), _mm256_broadcast_ps(&Source[2])));
				Result = _mm256_add_ps(Result, _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0xff), _mm256_broadcast_ps(&Source[3])));
			}

			return Result;
		}
#endif

		static CONSTEXPR Float FMod
		(
			const Float X,
			const Float Y
		)
		{
			return fmodf(X, Y);
		}

		static CONSTEXPR Float UnwindDegrees
		(
			Float A
		)
		{
			A = FMod(A, 360.f);

			if (A > 180.f)
			{
				A -= 360.f;
			}

			if (A < -180.f)
			{
				A += 360.f;
			}

			return A;
		}

		static CONSTEXPR Float Cos
		(
			const Float F
		)
		{
			return cosf(F);
		}

		static CONSTEXPR Float Sin
		(
			const Float F
		)
		{
			return sinf(F);
		}

		static CONSTEXPR Float Acos
		(
			const Float Value
		) 
		{ 
			return acosf((Value<-1.f) ? -1.f : ((Value<1.f) ? Value : 1.f)); 
		}

		static CONSTEXPR float Tan
		(
			const float Value
		) 
		{ 
			return tanf(Value); 
		}

		static CONSTEXPR float Atan
		(
			const float Value
		) 
		{ 
			return atanf(Value); 
		}

		static constexpr float Atan2_LUT[7] =
		{
			+7.2128853633444123e-03f,
			-3.5059680836411644e-02f,
			+8.1675882859940430e-02f,
			-1.3374657325451267e-01f,
			+1.9856563505717162e-01f,
			-3.3324998579202170e-01f,
			+1.0f
		};

		static constexpr float Atan2
		(
			const float Y,
			const float X
		)
		{
			const float AbsX = Abs(X);
			const float AbsY = Abs(Y);

			const bool YAbsBigger = (AbsY > AbsX);

			float T0 = YAbsBigger ? AbsY : AbsX;
			float T1 = YAbsBigger ? AbsX : AbsY;

			if (T0 == 0.0f)
			{
				return 0.0f;
			}

			float T3 = T1 / T0;
			float T4 = T3 * T3;

			T0 = Atan2_LUT[0];
			T0 = T0 * T4 + Atan2_LUT[1];
			T0 = T0 * T4 + Atan2_LUT[2];
			T0 = T0 * T4 + Atan2_LUT[3];
			T0 = T0 * T4 + Atan2_LUT[4];
			T0 = T0 * T4 + Atan2_LUT[5];
			T0 = T0 * T4 + Atan2_LUT[6];
			T3 = T0 * T3;

			T3 = YAbsBigger ? (0.5f * PI) - T3 : T3;
			T3 = (X < 0.0f) ? PI - T3 : T3;
			T3 = (Y < 0.0f) ? -T3 : T3;

			return T3;
		}

		static float FastAsin(float Value)
		{
			float AbsX = Abs(Value);
			float InvX = 1.0f - AbsX;

			if (InvX < 0.0f)
			{
				InvX = 0.0f;
			}

			float Result = ((((((
				-0.0012624911f * 
				AbsX + 0.0066700901f) * 
					AbsX - 0.0170881256f) * 
						AbsX + 0.0308918810f) * 
							AbsX - 0.0501743046f) * 
								AbsX + 0.0889789874f) * 
									AbsX - 0.2145988016f) * AbsX + FASTASIN_HALF_PI;

			Result *= Sqrt(InvX);

			return (Value >= 0.0f ? FASTASIN_HALF_PI - Result : Result - FASTASIN_HALF_PI);
		}

		template 
		<
			class T
		>
			static CONSTEXPR T DivideAndRoundUp(T Dividend, T Divisor)
		{
			return (Dividend + Divisor - 1) / Divisor;
		}

		static constexpr double FastPow
		(
			const double A,
			const double B
		)
		{
			union
			{
				double F;
				int X[2];
			} U = { A };

			U.X[1] = static_cast<int>(B * (U.X[1] - 1072632447) + 1072632447);
			U.X[0] = 0;

			return U.F;
		}

		static CONSTEXPR Float Pow
		(
			const Float A,
			const Float B
		)
		{
			return powf(A, B);
		}

		static CONSTEXPR bool IsNaN
		(
			const Float F
		) 
		{ 
			return _isnan(F) != 0; 
		}

		static CONSTEXPR bool IsFinite
		(
			const Float F
		) 
		{ 
			return _finite(F) != 0; 
		}

		static CONSTEXPR Float Frac
		(
			const Float N
		)
		{
			return N - std::floor(N);
		}

		static CONSTEXPR Float Hash
		(
			const Float N
		)
		{
			return Frac(std::sin(N) * 1e4f);
		}

		template<class T, class P = Float>
		static constexpr Float Lerp
		(
			const T X,
			const T Y,
			const P S
		)
		{
			return X + S * (Y - X);
		}

		static CONSTEXPR Vector3f Floor
		(
			const Vector3f & V
		);

		static CONSTEXPR Vector3f Frac
		(
			const Vector3f & V
		);

		CONSTEXPR Float GenerateNoise
		(
			const Vector3f & Position
		);

		CONSTEXPR Float GenerateNoise2
		(
			const Vector3f & Position
		);

		template<typename T> static FORCEINLINE T Random()
		{
			thread_local size_t Seed = 1;
			T Value = static_cast<T>((Seed ^= 0x3F800000) +
				*reinterpret_cast<unsigned int*>(0x7FFE0008));

			return Value;
		}

		static FORCEINLINE Float RandomNoise()
		{
			thread_local unsigned int Seed = 1;
			Seed ^= *reinterpret_cast<unsigned int*>(0x7FFE0008) ^ 0x3F800000 + (Seed << 6) + (Seed >> 2);

			float Value;
			*reinterpret_cast<int32_t*>(&Value) = (0x7E << 23) | (Seed & 0x7FFFFF);

			return 2.0f * Value - 1.0f;
		}

		static FORCEINLINE Float RandomNoiseGtZero()
		{
			Float Noise = RandomNoise();

			if (Noise == 0)
			{
				return FLT_EPSILON;
			}

			return Noise;
		}

		template
		<
			typename T, std::enable_if_t<sizeof(T) == sizeof(float), size_t> = 0
		> 
		static CONSTEXPR T Rcp
		(
			const T F
		)
		{
			if (F == 0.0f)
			{
				return FLT_MAX;
			}
			else
			{
				if (F != 1.0)
				{
					return 1 / F;
				}
			}

			return F;
		}

		template
		<
			typename T, std::enable_if_t<sizeof(T) == sizeof(double), size_t> = 0
		> 
		static CONSTEXPR T Rcp
		(
			const T F
		)
		{
			if (F == 0.0f)
			{
				return FLT_MAX;
			}
			else
			{
				if (F != 1.0)
				{
					return 1 / F;
				}
			}

			return F;
		}

		static Vector3f ClosestPointOnLine
		(
			const Vector3f & LineStart, 
			const Vector3f & LineEnd, 
			const Vector3f & Point
		);

		static Vector3f ClosestPointOnInfiniteLine
		(
			const Vector3f & LineStart, 
			const Vector3f & LineEnd, 
			const Vector3f & Point
		);

		static Vector3f LinePlaneIntersection
		(
			const Vector3f	& Point1,
			const Vector3f	& Point2,
			const Plane		& Plane
		);

		static bool IntersectPlanes3
		(
					Vector3f& I,
			const	Plane	& P1,
			const	Plane	& P2,
			const	Plane	& P3
		);

		static bool IntersectPlanes2
		(
					Vector3f & I,
					Vector3f & D, 
			const	Plane	 & P1, 
			const	Plane	 & P2
		);

		static bool Intersects
		(
			const	Vector2f &	Object,
			const	Vector2f &	Source,
			const	Vector2f &	Destination,
			const	Float		LineSize,
			const	Float		ObjectSize,
					Vector2f *	Intersection0,
					Vector2f *	Intersection1
		);

		template
		<
			class U
		>
		FORCEINLINE Rotation Lerp
		(
			const	Rotation & A,
			const	Rotation & B,
			const	U		 & Alpha
		);

		template
		<
			class U
		>
		FORCEINLINE Rotation LerpRange
		(
			const	Rotation & A,
			const	Rotation & B,
			const	U		 & Alpha
		);

		template
		<
			class U
		>
		FORCEINLINE Quaternion Lerp
		(
			const	Quaternion  & A,
			const	Quaternion  & B,
			const	U			& Alpha
		);

		template
		<
			class U
		>
		FORCEINLINE Quaternion BiLerp
		(
			const	Quaternion & P00,
			const	Quaternion & P10,
			const	Quaternion & P01,
			const	Quaternion & P11,
			const	float		 FracX,
			const	float		 FracY
		);

		template
		<
			class U
		>
		FORCEINLINE Quaternion CubicInterp
		(
			const	Quaternion	& P0,
			const	Quaternion	& T0,
			const	Quaternion	& P1,
			const	Quaternion	& T1,
			const	U			& A
		);
	}
}