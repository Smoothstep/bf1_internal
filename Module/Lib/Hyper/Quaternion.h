#pragma once

#include "Rotation.h"
#include "Matrix.h"

namespace Hyper
{
	struct Quaternion
	{
	public:
		float X;
		float Y;
		float Z;
		float W;

	public:
		static const Quaternion Identity;

	public:
		FORCEINLINE Quaternion
		(
			float InX = 0.f,
			float InY = 0.f,
			float InZ = 0.f,
			float InW = 0.f
		);

		FORCEINLINE Quaternion
		(
			const Quaternion & Q
		);

		FORCEINLINE Quaternion
		(
			const Matrix4x4 & M
		);

		FORCEINLINE Quaternion
		(
			const Rotation & R
		);

		FORCEINLINE Quaternion
		(
			const Vector3f & Axis, 
			const float		 AngleRad
		);

	public:

		FORCEINLINE Quaternion operator+
		(
			const Quaternion & Q
		)	const;

		FORCEINLINE Quaternion operator+=
		(
			const Quaternion & Q
		);

		FORCEINLINE Quaternion operator-
		(
			const Quaternion & Q
		)	const;

		FORCEINLINE bool Equals
		(
			const	Quaternion & Q, 
			const	float		 Tolerance	= KINDA_SMALL_NUMBER
		)	const;

		FORCEINLINE bool IsIdentity
		(
			const	float Tolerance			= SMALL_NUMBER
		)	const;

		FORCEINLINE Quaternion operator-=
		(
			const Quaternion & Q
		);

		FORCEINLINE Quaternion operator*
		(
			const Quaternion & Q
		)	const;

		FORCEINLINE Quaternion operator*=
		(
			const Quaternion & Q
		);

		FORCEINLINE Vector3f operator*
		(
			const Vector3f & V
		)	const;

		FORCEINLINE Matrix4x4 operator*
		(
			const Matrix4x4 & M
		)	const;

		FORCEINLINE Quaternion operator*=
		(
			const float Scale
		);
		FORCEINLINE Quaternion operator*
		(
			const float Scale
		)	const;

		FORCEINLINE Quaternion operator/=
		(
			const float Scale
		);

		FORCEINLINE Quaternion operator/
		(
			const float Scale
		)	const;

		FORCEINLINE bool operator==
		(
			const Quaternion & Q
		)	const;

		FORCEINLINE bool operator!=
		(
			const Quaternion & Q
		)	const;

		FORCEINLINE float operator|
		(
			const Quaternion & Q
		)	const;

	public:

		static Quaternion MakeFromEuler
		(
			const Vector3f & Euler
		);

		FORCEINLINE Vector3f GetEuler() const;

		FORCEINLINE void Normalize
		( 
			const float Tolerance = SMALL_NUMBER
		);

		FORCEINLINE Quaternion GetNormalized
		(
			const float Tolerance = SMALL_NUMBER
		)	const;

		FORCEINLINE bool IsNormalized() const;

		FORCEINLINE float Size()		const;
		FORCEINLINE float SizeSquared() const;

		FORCEINLINE void ToAxisAndAngle
		(
					Vector3f & Axis, 
					float	 & Angle
		)	const;

		FORCEINLINE void ToSwingTwist
		(
			const	Vector3f	& InTwistAxis, 
					Quaternion	& OutSwing, 
					Quaternion	& OutTwist
		)	const;

		FORCEINLINE Vector3f RotateVector
		(
			const	Vector3f V
		)	const;

		FORCEINLINE Vector3f UnrotateVector
		(
			const	Vector3f V
		)	const;

		FORCEINLINE void GetRotationMatrix
		(
			Matrix4x4 & M
		);

		FORCEINLINE Quaternion Log()		const;
		FORCEINLINE Quaternion Exp()		const;
		FORCEINLINE Quaternion Inverse()	const;

		FORCEINLINE void EnforceShortestArcWith
		(
			const Quaternion & OtherQuat
		);

		FORCEINLINE Vector3f GetAxisX()			const;
		FORCEINLINE Vector3f GetAxisY()			const;
		FORCEINLINE Vector3f GetAxisZ()			const;
		FORCEINLINE Vector3f GetForwardVector() const;
		FORCEINLINE Vector3f GetRightVector()	const;
		FORCEINLINE Vector3f GetUpVector()		const;
		FORCEINLINE Vector3f GetVector()		const;
					Rotation GetRotation()		const;
		FORCEINLINE Vector3f GetRotationAxis()	const;

		FORCEINLINE float AngularDistance
		(
			const Quaternion& Q
		)	const;

		bool ContainsNaN() const;

	public:
		FORCEINLINE void DiagnosticCheckNaN() const {}
		FORCEINLINE void DiagnosticCheckNaN
		(
			const TCHAR * Message
		)	const {}

	public:
		static FORCEINLINE Quaternion FindBetween
		(
			const Vector3f & Vector1, 
			const Vector3f & Vector2
		)
		{
			return FindBetweenVectors(Vector1, Vector2);
		}

		static Quaternion FindBetweenNormals
		(
			const Vector3f & Normal1, 
			const Vector3f & Normal2
		);

		static Quaternion FindBetweenVectors
		(
			const Vector3f & Vector1, 
			const Vector3f & Vector2
		);

		static FORCEINLINE float Error
		(
			const Quaternion & Q1, 
			const Quaternion & Q2
		);

		static FORCEINLINE float ErrorAutoNormalize
		(
			const Quaternion & A, const Quaternion & B
		);

		static FORCEINLINE Quaternion FastLerp
		(
			const Quaternion &	A, 
			const Quaternion &	B, 
			const float			Alpha);

		static FORCEINLINE Quaternion FastBilerp
		(
			const Quaternion &	P00, 
			const Quaternion &	P10, 
			const Quaternion &	P01, 
			const Quaternion &	P11, 
			const float			FracX,
			const float			FracY);

		static Quaternion Slerp_NotNormalized
		(
			const Quaternion &	Quat1, 
			const Quaternion &	Quat2, 
			const float			Slerp
		);

		static FORCEINLINE Quaternion Slerp
		(
			const	Quaternion & Quat1, 
			const	Quaternion & Quat2, 
			const	float		 Slerp
		)
		{
			return Slerp_NotNormalized(Quat1, Quat2, Slerp).GetNormalized();
		}

		static Quaternion SlerpFullPath_NotNormalized
		(
			const	Quaternion & Quat1,
			const	Quaternion & Quat2,
			const	float		 Alpha
		);

		static FORCEINLINE Quaternion SlerpFullPath
		(
			const	Quaternion & Quat1,
			const	Quaternion & Quat2,
			const	float		 Alpha)
		{
			return SlerpFullPath_NotNormalized(Quat1, Quat2, Alpha).GetNormalized();
		}

		static Quaternion Squad
		(
			const	Quaternion & Quat1,
			const	Quaternion & Tang1, 
			const	Quaternion & Quat2,
			const	Quaternion & Tang2, 
			const	float		 Alpha
		);

		static Quaternion SquadFullPath
		(
			const	Quaternion & Quat1,
			const	Quaternion & Tang1,
			const	Quaternion & Quat2,
			const	Quaternion & Tang2,
			const	float		 Alpha
		);

		static void CalcTangents
		(
			const	Quaternion & PrevP, 
			const	Quaternion & P, 
			const	Quaternion & NextP, 
			const	float		 Tension,
					Quaternion & OutTan);
	};


	inline Quaternion::Quaternion
	(
		const Matrix4x4 & M
	)
	{
		if (M.GetScaledAxis(EAxis::X).IsNearlyZero() || 
			M.GetScaledAxis(EAxis::Y).IsNearlyZero() || 
			M.GetScaledAxis(EAxis::Z).IsNearlyZero())
		{
			*this = Quaternion::Identity;
			return;
		}

#ifdef _DEBUG
		if (!(
			(Math::Abs(1.f - M.GetScaledAxis(EAxis::X).SizeSquared()) <= KINDA_SMALL_NUMBER) && 
			(Math::Abs(1.f - M.GetScaledAxis(EAxis::Y).SizeSquared()) <= KINDA_SMALL_NUMBER) && 
			(Math::Abs(1.f - M.GetScaledAxis(EAxis::Z).SizeSquared()) <= KINDA_SMALL_NUMBER)))
		{
			*this = Quaternion::Identity;
			return;
		}
#endif
				float s;
		const	float tr = M.MatrixArray[0][0] + M.MatrixArray[1][1] + M.MatrixArray[2][2];

		if (tr > 0.0f)
		{
			float InvS = Math::InvSqrt(tr + 1.f);
			
			s = 0.5f * InvS;

			this->X = (M.MatrixArray[1][2] - M.MatrixArray[2][1]) * s;
			this->Y = (M.MatrixArray[2][0] - M.MatrixArray[0][2]) * s;
			this->Z = (M.MatrixArray[0][1] - M.MatrixArray[1][0]) * s;
			this->W = 0.5f * (1.f / InvS);
		}
		else
		{
			int32 i = 0;

			if (M.MatrixArray[1][1] > M.MatrixArray[0][0])
			{
				i = 1;
			}

			if (M.MatrixArray[2][2] > M.MatrixArray[i][i])
			{
				i = 2;
			}

			static const int32 nxt[3] = { 1, 2, 0 };

			const int32 j = nxt[i];
			const int32 k = nxt[j];

			s = M.MatrixArray[i][i] - M.MatrixArray[j][j] - M.MatrixArray[k][k] + 1.0f;

			float InvS = Math::InvSqrt(s);

			s = 0.5f * InvS;

			float Quat[4];
			{
				Quat[i] = 0.5f * (1.f / InvS);
				Quat[3] = (M.MatrixArray[j][k] - M.MatrixArray[k][j]) * s;
				Quat[j] = (M.MatrixArray[i][j] + M.MatrixArray[j][i]) * s;
				Quat[k] = (M.MatrixArray[i][k] + M.MatrixArray[k][i]) * s;
			}

			this->X = Quat[0];
			this->Y = Quat[1];
			this->Z = Quat[2];
			this->W = Quat[3];

			DiagnosticCheckNaN();
		}
	}


	FORCEINLINE Quaternion::Quaternion
	(
		const Rotation & R
	)
	{
		*this = R.GetQuaternion();
		DiagnosticCheckNaN();
	}


	FORCEINLINE Vector3f Quaternion::operator*
	(
		const Vector3f & V
	)	const
	{
		return RotateVector(V);
	}


	inline Matrix4x4 Quaternion::operator*
	(
		const Matrix4x4 & M
	)	const
	{
		Matrix4x4 Result;

		Quaternion VT, VR;
		Quaternion Inv = Inverse();

		for (int32 I = 0; I < 4; ++I)
		{
			Quaternion VQ
			(
				M.MatrixArray[I][0], 
				M.MatrixArray[I][1], 
				M.MatrixArray[I][2], 
				M.MatrixArray[I][3]
			);

			Math::SSE::VectorQuaternionMultiply(&VT, this, &VQ);
			Math::SSE::VectorQuaternionMultiply(&VR, &VT, &Inv);

			Result.MatrixArray[I][0] = VR.X;
			Result.MatrixArray[I][1] = VR.Y;
			Result.MatrixArray[I][2] = VR.Z;
			Result.MatrixArray[I][3] = VR.W;
		}

		return Result;
	}

	FORCEINLINE Quaternion::Quaternion
	(
		const float InX,
		const float InY,
		const float InZ,
		const float InW
	)
		: X(InX)
		, Y(InY)
		, Z(InZ)
		, W(InW)
	{
		DiagnosticCheckNaN();
	}


	FORCEINLINE Quaternion::Quaternion
	(
		const Quaternion & Q
	)
		: X(Q.X)
		, Y(Q.Y)
		, Z(Q.Z)
		, W(Q.W)
	{ }


	FORCEINLINE Quaternion::Quaternion
	(
		const Vector3f & Axis,
		const float		 AngleRad
	)
	{
		const float half_a = 0.5f * AngleRad;
		float s, c;
		Math::SinCos(&s, &c, half_a);

		X = s * Axis.X;
		Y = s * Axis.Y;
		Z = s * Axis.Z;
		W = c;

		DiagnosticCheckNaN();
	}


	FORCEINLINE Quaternion Quaternion::operator+
	(
		const Quaternion& Q
	)	const
	{
		return Quaternion(X + Q.X, Y + Q.Y, Z + Q.Z, W + Q.W);
	}


	FORCEINLINE Quaternion Quaternion::operator+=
	(
		const Quaternion & Q
	)
	{
		this->X += Q.X;
		this->Y += Q.Y;
		this->Z += Q.Z;
		this->W += Q.W;

		DiagnosticCheckNaN();

		return *this;
	}


	FORCEINLINE Quaternion Quaternion::operator-
	(
		const Quaternion & Q
	)	const
	{
		return Quaternion
		(
			X - Q.X, 
			Y - Q.Y, 
			Z - Q.Z, 
			W - Q.W
		);
	}


	FORCEINLINE bool Quaternion::Equals
	(
		const	Quaternion & Q, 
		const	float		 Tolerance
	)	const
	{
#if PLATFORM_ENABLE_VECTORINTRINSICS
		const M128 ToleranceV = VectorLoadFloat1(&Tolerance);
		const M128 A = VectorLoadAligned(this);
		const M128 B = VectorLoadAligned(&Q);

		const M128 RotationSub = VectorAbs(VectorSubtract(A, B));
		const M128 RotationAdd = VectorAbs(VectorAdd(A, B));
		return !VectorAnyGreaterThan(RotationSub, ToleranceV) || !VectorAnyGreaterThan(RotationAdd, ToleranceV);
#else
		return (Math::Abs(X - Q.X) <= Tolerance && Math::Abs(Y - Q.Y) <= Tolerance && Math::Abs(Z - Q.Z) <= Tolerance && Math::Abs(W - Q.W) <= Tolerance)
			|| (Math::Abs(X + Q.X) <= Tolerance && Math::Abs(Y + Q.Y) <= Tolerance && Math::Abs(Z + Q.Z) <= Tolerance && Math::Abs(W + Q.W) <= Tolerance);
#endif
	}

	FORCEINLINE bool Quaternion::IsIdentity
	(
		const float Tolerance
	)	const
	{
		return Equals(Quaternion::Identity, Tolerance);
	}

	FORCEINLINE Quaternion Quaternion::operator-=
	(
		const Quaternion & Q
	)
	{
		this->X -= Q.X;
		this->Y -= Q.Y;
		this->Z -= Q.Z;
		this->W -= Q.W;

		DiagnosticCheckNaN();

		return *this;
	}


	FORCEINLINE Quaternion Quaternion::operator*
	(
		const Quaternion & Q
	)	const
	{
		Quaternion Result;
		Math::SSE::VectorQuaternionMultiply(&Result, this, &Q);

		Result.DiagnosticCheckNaN();

		return Result;
	}


	FORCEINLINE Quaternion Quaternion::operator*=
	(
		const Quaternion & Q
	)
	{
		M128 A = VectorLoadAligned(this);
		M128 B = VectorLoadAligned(&Q);
		M128 Result;
		Math::SSE::VectorQuaternionMultiply(&Result, &A, &B);
		VectorStoreAligned(Result, this);

		DiagnosticCheckNaN();

		return *this;
	}


	FORCEINLINE Quaternion Quaternion::operator*=
	(
		const float Scale
	)
	{
		X *= Scale;
		Y *= Scale;
		Z *= Scale;
		W *= Scale;

		DiagnosticCheckNaN();

		return *this;
	}


	FORCEINLINE Quaternion Quaternion::operator*
	(
		const float Scale
	)	const
	{
		return Quaternion(Scale * X, Scale * Y, Scale * Z, Scale * W);
	}


	FORCEINLINE Quaternion Quaternion::operator/=
	(
		const float Scale
	)
	{
		const float Recip = 1.0f / Scale;
		X *= Recip;
		Y *= Recip;
		Z *= Recip;
		W *= Recip;

		DiagnosticCheckNaN();

		return *this;
	}


	FORCEINLINE Quaternion Quaternion::operator/
	(
		const float Scale
	)	const
	{
		const float Recip = 1.0f / Scale;
		return Quaternion(X * Recip, Y * Recip, Z * Recip, W * Recip);
	}


	FORCEINLINE bool Quaternion::operator==
	(
		const Quaternion & Q
	)	const
	{
#if PLATFORM_ENABLE_VECTORINTRINSICS
		const VectorRegister A = VectorLoadAligned(this);
		const VectorRegister B = VectorLoadAligned(&Q);
		return VectorMaskBits(VectorCompareEQ(A, B)) == 0x0F;
#else
		return X == Q.X && Y == Q.Y && Z == Q.Z && W == Q.W;
#endif // PLATFORM_ENABLE_VECTORINTRINSICS
	}


	FORCEINLINE bool Quaternion::operator!=
	(
		const Quaternion & Q
	)	const
	{
#if PLATFORM_ENABLE_VECTORINTRINSICS
		const VectorRegister A = VectorLoadAligned(this);
		const VectorRegister B = VectorLoadAligned(&Q);
		return VectorMaskBits(VectorCompareNE(A, B)) != 0x00;
#else
		return X != Q.X || Y != Q.Y || Z != Q.Z || W != Q.W;
#endif // PLATFORM_ENABLE_VECTORINTRINSICS
	}


	FORCEINLINE float Quaternion::operator|
	(
		const Quaternion & Q
	)	const
	{
		return X * Q.X + Y * Q.Y + Z * Q.Z + W * Q.W;
	}


	FORCEINLINE Vector3f Quaternion::GetEuler() const
	{
		return GetRotation().GetEuler();
	}


	FORCEINLINE void Quaternion::Normalize
	(
		const float Tolerance
	)
	{
#if PLATFORM_ENABLE_VECTORINTRINSICS
		const VectorRegister Vector = VectorLoadAligned(this);

		const VectorRegister SquareSum = VectorDot4(Vector, Vector);
		const VectorRegister NonZeroMask = VectorCompareGE(SquareSum, VectorLoadFloat1(&Tolerance));
		const VectorRegister InvLength = VectorReciprocalSqrtAccurate(SquareSum);
		const VectorRegister NormalizedVector = VectorMultiply(InvLength, Vector);
		VectorRegister Result = VectorSelect(NonZeroMask, NormalizedVector, GlobalVectorConstants::Float0001);

		VectorStoreAligned(Result, this);
#else
		const float SquareSum = X * X + Y * Y + Z * Z + W * W;

		if (SquareSum >= Tolerance)
		{
			const float Scale = Math::InvSqrt(SquareSum);

			X *= Scale;
			Y *= Scale;
			Z *= Scale;
			W *= Scale;
		}
		else
		{
			*this = Quaternion::Identity;
		}
#endif // PLATFORM_ENABLE_VECTORINTRINSICS
	}


	FORCEINLINE Quaternion Quaternion::GetNormalized
	(
		const float Tolerance
	)	const
	{
		Quaternion Result(*this);
		Result.Normalize(Tolerance);
		return Result;
	}



	FORCEINLINE bool Quaternion::IsNormalized() const
	{
		return (Math::Abs(1.f - SizeSquared()) < THRESH_QUAT_NORMALIZED);
	}


	FORCEINLINE float Quaternion::Size() const
	{
		return Math::Sqrt(X * X + Y * Y + Z * Z + W * W);
	}


	FORCEINLINE float Quaternion::SizeSquared() const
	{
		return (X * X + Y * Y + Z * Z + W * W);
	}


	FORCEINLINE void Quaternion::ToAxisAndAngle
	(
		Vector3f & Axis, 
		float	 & Angle
	)	const
	{
		Angle = 2.f * Math::Acos(W);
		Axis = GetRotationAxis();
	}

	FORCEINLINE Vector3f Quaternion::GetRotationAxis() const
	{
		// Ensure we never try to sqrt a neg number
		const float S = Math::Sqrt(Math::Max(1.f - (W * W), 0.f));

		if (S >= 0.0001f)
		{
			return Vector3f(X / S, Y / S, Z / S);
		}

		return Vector3f(1.f, 0.f, 0.f);
	}

	float Quaternion::AngularDistance
	(
		const Quaternion & Q
	)	const
	{
		float InnerProd = X*Q.X + Y*Q.Y + Z*Q.Z + W*Q.W;
		return Math::Acos((2 * InnerProd * InnerProd) - 1.f);
	}


	FORCEINLINE Vector3f Quaternion::RotateVector
	(
		const Vector3f V
	)	const
	{
#if WITH_DIRECTXMATH
		Vector3f Result;
		VectorQuaternionVector3Rotate(&Result, &V, this);
		return Result;

#else

		// http://people.csail.mit.edu/bkph/articles/Quaternions.pdf
		// V' = V + 2w(Q x V) + (2Q x (Q x V))
		// refactor:
		// V' = V + w(2(Q x V)) + (Q x (2(Q x V)))
		// T = 2(Q x V);
		// V' = V + w*(T) + (Q x T)

		const Vector3f Q(X, Y, Z);
		const Vector3f T = 2.f * Vector3f::CrossProduct(Q, V);
		const Vector3f Result = V + (W * T) + Vector3f::CrossProduct(Q, T);
		return Result;
#endif
	}

	FORCEINLINE Vector3f Quaternion::UnrotateVector
	(
		const Vector3f V
	)	const
	{
#if WITH_DIRECTXMATH
		Vector3f Result;
		VectorQuaternionVector3InverseRotate(&Result, &V, this);
		return Result;
#else
		//return Inverse().RotateVector(V);

		const Vector3f Q(-X, -Y, -Z); // Inverse
		const Vector3f T = 2.f * Vector3f::CrossProduct(Q, V);
		const Vector3f Result = V + (W * T) + Vector3f::CrossProduct(Q, T);
		return Result;
#endif
	}


	FORCEINLINE void Quaternion::GetRotationMatrix
	(
		Matrix4x4 & Result
	)
	{
		Quaternion VT, VR;
		Quaternion Inv = Inverse();

		for (int32 I = 0; I < 4; ++I)
		{
			Quaternion VQ
			(
				Result.MatrixArray[I][0],
				Result.MatrixArray[I][1],
				Result.MatrixArray[I][2],
				Result.MatrixArray[I][3]
			);

			Math::SSE::VectorQuaternionMultiply(&VT, this, &VQ);
			Math::SSE::VectorQuaternionMultiply(&VR, &VT, &Inv);

			Result.MatrixArray[I][0] = VR.X;
			Result.MatrixArray[I][1] = VR.Y;
			Result.MatrixArray[I][2] = VR.Z;
			Result.MatrixArray[I][3] = VR.W;
		}
	}


	FORCEINLINE Quaternion Quaternion::Inverse() const
	{
		return Quaternion(-X, -Y, -Z, W);
	}


	FORCEINLINE void Quaternion::EnforceShortestArcWith
	(
		const Quaternion & OtherQuat
	)
	{
		const float DotResult = (OtherQuat | *this);
		const float Bias = Math::FloatSelect(DotResult, 1.0f, -1.0f);

		X *= Bias;
		Y *= Bias;
		Z *= Bias;
		W *= Bias;
	}


	FORCEINLINE Vector3f Quaternion::GetAxisX() const
	{
		return RotateVector(Vector3f(1.f, 0.f, 0.f));
	}


	FORCEINLINE Vector3f Quaternion::GetAxisY() const
	{
		return RotateVector(Vector3f(0.f, 1.f, 0.f));
	}


	FORCEINLINE Vector3f Quaternion::GetAxisZ() const
	{
		return RotateVector(Vector3f(0.f, 0.f, 1.f));
	}


	FORCEINLINE Vector3f Quaternion::GetForwardVector() const
	{
		return GetAxisX();
	}

	FORCEINLINE Vector3f Quaternion::GetRightVector() const
	{
		return GetAxisY();
	}

	FORCEINLINE Vector3f Quaternion::GetUpVector() const
	{
		return GetAxisZ();
	}

	FORCEINLINE Vector3f Quaternion::GetVector() const
	{
		return GetAxisX();
	}


	FORCEINLINE float Quaternion::Error
	(
		const Quaternion & Q1, 
		const Quaternion & Q2
	)
	{
		const float cosom = Math::Abs(Q1.X * Q2.X + Q1.Y * Q2.Y + Q1.Z * Q2.Z + Q1.W * Q2.W);
		return (Math::Abs(cosom) < 0.9999999f) ? Math::Acos(cosom)*(1.f / PI) : 0.0f;
	}


	FORCEINLINE float Quaternion::ErrorAutoNormalize
	(
		const Quaternion & A, 
		const Quaternion & B
	)
	{
		Quaternion Q1 = A;
		Q1.Normalize();

		Quaternion Q2 = B;
		Q2.Normalize();

		return Quaternion::Error(Q1, Q2);
	}


	FORCEINLINE Quaternion Quaternion::FastLerp
	(
		const	Quaternion & A, 
		const	Quaternion & B, 
		const	float		 Alpha
	)
	{
		// To ensure the 'shortest route', we make sure the dot product between the both rotations is positive.
		const float DotResult = (A | B);
		const float Bias = Math::FloatSelect(DotResult, 1.0f, -1.0f);
		return (B * Alpha) + (A * (Bias * (1.f - Alpha)));
	}


	FORCEINLINE Quaternion Quaternion::FastBilerp
	(
		const	Quaternion & P00, 
		const	Quaternion & P10, 
		const	Quaternion & P01, 
		const	Quaternion & P11, 
		const	float		 FracX,
		const	float		 FracY
	)
	{
		return 
			Quaternion::FastLerp(
			Quaternion::FastLerp(P00, P10, FracX),
			Quaternion::FastLerp(P01, P11, FracX),
			FracY
		);
	}


	FORCEINLINE bool Quaternion::ContainsNaN() const
	{
		return (
			!Math::IsFinite(X) ||
			!Math::IsFinite(Y) ||
			!Math::IsFinite(Z) ||
			!Math::IsFinite(W)
			);
	}


	template<class U>
	FORCEINLINE Quaternion Math::Lerp
	(
		const	Quaternion  & A, 
		const	Quaternion  & B, 
		const	U			& Alpha
	)
	{
		return Quaternion::Slerp(A, B, Alpha);
	}

	template<class U>
	FORCEINLINE Quaternion Math::BiLerp
	(
		const	Quaternion & P00, 
		const	Quaternion & P10, 
		const	Quaternion & P01, 
		const	Quaternion & P11, 
		const	float		 FracX,
		const	float		 FracY
	)
	{
		Quaternion Result;

		Result = Lerp(
			Quaternion::Slerp_NotNormalized(P00, P10, FracX),
			Quaternion::Slerp_NotNormalized(P01, P11, FracX),
			FracY
		);

		return Result;
	}

	template<class U>
	FORCEINLINE Quaternion Math::CubicInterp
	(
		const	Quaternion	& P0, 
		const	Quaternion	& T0, 
		const	Quaternion	& P1, 
		const	Quaternion	& T1, 
		const	U			& A
	)
	{
		return Quaternion::Squad(P0, T0, P1, T1, A);
	}


	FORCEINLINE void Matrix4x4::Rotate
	(
		Quaternion Q
	)
	{
		Q.Normalize();

		const Matrix4x4 RotationMatrix
		(
			1.0f -	2.0f * Q.Y * Q.Y - 2.0f * Q.Z * Q.Z,		2.0f * Q.X * Q.Y - 2.0f * Q.Z * Q.W,		2.0f * Q.X * Q.Z + 2.0f * Q.Y * Q.W,	0.0f,
					2.0f * Q.X * Q.Y + 2.0f * Q.Z * Q.W, 1.0f -	2.0f * Q.X * Q.X - 2.0f * Q.Z * Q.Z,		2.0f * Q.Y * Q.Z - 2.0f * Q.X * Q.W,	0.0f,
					2.0f * Q.X * Q.Z - 2.0f * Q.Y * Q.W,		2.0f * Q.Y * Q.Z + 2.0f * Q.X * Q.W, 1.0f - 2.0f * Q.X * Q.X - 2.0f * Q.Y * Q.Y,	0.0f,
					0.0f,										0.0f,										0.0f,									1.0f
		);

		*this *= RotationMatrix;
	}

	constexpr FORCEINLINE Vector3f::Vector3f
	(
		const Quaternion & Q
	)
		: X(Q.X)
		, Y(Q.Y)
		, Z(Q.Z)
	{}

	FORCEINLINE void Vector3f::RotateByQuaternion
	(
		const Quaternion & Quaternion
	)
	{
		const Vector3f &	V = *this;
		const Vector3f &	U = Quaternion;
		const Float			S = Quaternion.W;

		*this = 2.0f * (U | V) * V + (S * S - (U | U)) * V + 2.0f * S * (U ^ V);
	}

	FORCEINLINE Rotation::Rotation
	(
		const Quaternion & Quat
	)
	{
		*this = Quat.GetRotation();
		DiagnosticCheckNaN();
	}
}