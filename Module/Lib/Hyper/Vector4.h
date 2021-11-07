#pragma once

#include "Vector2.h"
#include "Vector3.h"

namespace Hyper
{
	ALIGN(16) struct Vector4f
	{
	public:

		static const Vector4f ZeroVector;

	public:

		float X;
		float Y;
		float Z;
		float W;

	public:

		CONSTEXPR Vector4f();

		explicit CONSTEXPR Vector4f
		(
			const Vector3f& InVector, 
			const float		InW = 1.0f
		);

		explicit CONSTEXPR Vector4f
		(
			const float InX,
			const float InY,
			const float InZ,
			const float InW = 1.0f
		);

		explicit CONSTEXPR Vector4f
		(
			const Vector2f InXY,
			const Vector2f InZW
		);

		explicit CONSTEXPR Vector4f
		(
			const Float F
		);

		explicit CONSTEXPR Vector4f
		(
			const Float4 F
		);

	public:

		FORCEINLINE float& operator[]
		(
			const int32 ComponentIndex
		);

		FORCEINLINE float operator[]
		(
			const int32 ComponentIndex
		)	const;

		FORCEINLINE Vector4f operator-() const;

		FORCEINLINE Vector4f operator+
		(
			const Vector4f& V
		)	const;

		FORCEINLINE Vector4f operator+=
		(
			const Vector4f& V
		);

		FORCEINLINE Vector4f operator-
		(
			const Vector4f& V
		)	const;

		FORCEINLINE Vector4f operator*
		(
			const float Scale
		)	const;

		Vector4f operator/
		(
			const float Scale
		)	const;

		Vector4f operator/
		(
			const Vector4f & V
		)	const;

		Vector4f operator*
		(
			const Vector4f & V
		)	const;

		Vector4f operator*=
		(
			const Vector4f & V
		);

		Vector4f operator/=
		(
			const Vector4f & V
		);

		Vector4f operator*=
		(
			const float S
		);

		Vector4f operator/=
		(
			const float S
		);

		Vector4f operator=
		(
			const Vector3f& V
		);

		friend FORCEINLINE float Dot3
		(
			const Vector4f & V1, 
			const Vector4f & V2
		)
		{
			return 
				V1.X * V2.X + 
				V1.Y * V2.Y + 
				V1.Z * V2.Z;
		}

		friend FORCEINLINE float Dot4
		(
			const Vector4f & V1, 
			const Vector4f & V2
		)
		{
			return 
				V1.X * V2.X + 
				V1.Y * V2.Y + 
				V1.Z * V2.Z + 
				V1.W * V2.W;
		}

		friend FORCEINLINE Vector4f operator*
		(
			const float		 Scale, 
			const Vector4f & V)
		{
			return V * Scale;
		}

		bool operator==
		(
			const Vector4f & V
		)	const;

		bool operator!=
		(
			const Vector4f & V
		)	const;

		Vector4f operator^
		(
			const Vector4f & V
		)	const;

		float operator|
		(
			const Vector4f & V
		)	const;

		void operator=
		(
			const Float4 F
		);

	public:

		float & Component
		(
			const int32 Index
		);

		bool Equals
		(
			const Vector4f & V, 
			const float		 Tolerance = KINDA_SMALL_NUMBER
		)	const;

		bool IsUnit3
		(
			const float LengthSquaredTolerance = KINDA_SMALL_NUMBER
		)	const;

		FORCEINLINE Vector4f GetSafeNormal
		(
			const float Tolerance = SMALL_NUMBER
		)	const;

		FORCEINLINE Vector4f GetUnsafeNormal3() const;
		
		Quaternion	ToOrientationQuat()		const;
		Rotation	ToOrientationRotation() const;
		Rotation	GetRotation()			const;

		FORCEINLINE void Set
		(
			const float InX,
			const float InY, 
			const float InZ,
			const float InW
		);

		CONSTEXPR float Size3()				const;
		CONSTEXPR float SizeSquared3()		const;
		CONSTEXPR float Size()				const;
		CONSTEXPR float SizeSquared()		const;

		CONSTEXPR Vector4f Max(float V)		const;
		CONSTEXPR Vector4f Min(float V)		const;

		CONSTEXPR bool ContainsNaN() const;

		CONSTEXPR bool IsNearlyZero3
		(
			const float Tolerance = KINDA_SMALL_NUMBER
		)	const;

		Vector4f Reflect3
		(
			const Vector4f & Normal
		)	const;

		void FindBestAxisVectors3
		(
			Vector4f & Axis1, 
			Vector4f & Axis2
		)	const;

		FORCEINLINE Vector3f TransformToVector3() const;

		CONSTEXPR void DiagnosticCheckNaN() { }
	};


	CONSTEXPR Vector4f::Vector4f()
		: X(0.0f)
		, Y(0.0f)
		, Z(0.0f)
		, W(1.0f)
	{ }


	CONSTEXPR Vector4f::Vector4f
	(
		const Vector3f &	InVector, 
		const float			InW
	)
		: X(InVector.X)
		, Y(InVector.Y)
		, Z(InVector.Z)
		, W(InW)
	{
		DiagnosticCheckNaN();
	}


	CONSTEXPR Vector4f::Vector4f
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

	CONSTEXPR Vector4f::Vector4f
	(
		const Vector2f InXY,
		const Vector2f InZW
	)
		: X(InXY.X)
		, Y(InXY.Y)
		, Z(InZW.X)
		, W(InZW.Y)
	{
		DiagnosticCheckNaN();
	}

	CONSTEXPR Vector4f::Vector4f
	(
		const Float F
	)
		: X(F)
		, Y(F)
		, Z(F)
		, W(F)
	{
		DiagnosticCheckNaN();
	}

	CONSTEXPR Vector4f::Vector4f(const Float4 F)
		: X(F[0])
		, Y(F[1])
		, Z(F[2])
		, W(F[3])
	{
		DiagnosticCheckNaN();
	}


	FORCEINLINE float& Vector4f::operator[]
	(
		const int32 ComponentIndex
	)
	{
		return (&X)[ComponentIndex];
	}


	FORCEINLINE float Vector4f::operator[]
	(
		const int32 ComponentIndex
	)	const
	{
		return (&X)[ComponentIndex];
	}


	FORCEINLINE void Vector4f::Set
	(
		const float InX,
		const float InY,
		const float InZ,
		const float InW
	)
	{
		X = InX;
		Y = InY;
		Z = InZ;
		W = InW;

		DiagnosticCheckNaN();
	}


	FORCEINLINE Vector4f Vector4f::operator-() const
	{
		return Vector4f
		(
			-X, 
			-Y,
			-Z, 
			-W
		);
	}


	FORCEINLINE Vector4f Vector4f::operator+
	(
		const Vector4f & V
	)	const
	{
		return Vector4f
		(
			X + V.X, 
			Y + V.Y, 
			Z + V.Z, 
			W + V.W
		);
	}


	FORCEINLINE Vector4f Vector4f::operator+=
	(
		const Vector4f & V
	)
	{
		X += V.X; 
		Y += V.Y; 
		Z += V.Z; 
		W += V.W;
		DiagnosticCheckNaN();
		return *this;
	}


	FORCEINLINE Vector4f Vector4f::operator-
	(
		const Vector4f & V
	)	const
	{
		return Vector4f
		(
			X - V.X, 
			Y - V.Y, 
			Z - V.Z, 
			W - V.W
		);
	}


	FORCEINLINE Vector4f Vector4f::operator*
	(
		const float Scale
	)	const
	{
		return Vector4f
		(
			X * Scale, 
			Y * Scale, 
			Z * Scale, 
			W * Scale
		);
	}


	FORCEINLINE Vector4f Vector4f::operator/
	(
		const float Scale
	)	const
	{
		const float RScale = 1.f / Scale;

		return Vector4f
		(
			X * RScale, 
			Y * RScale, 
			Z * RScale, 
			W * RScale
		);
	}


	FORCEINLINE Vector4f Vector4f::operator*
	(
		const Vector4f & V
	)	const
	{
		return Vector4f
		(
			X * V.X, 
			Y * V.Y, 
			Z * V.Z, 
			W * V.W
		);
	}


	FORCEINLINE float Vector4f::operator|
	(
		const Vector4f & V
	)	const
	{
		return X * V.X + Y * V.Y + Z * V.Z + W * V.W;
	}


	FORCEINLINE Vector4f Vector4f::operator^
	(
		const Vector4f & V
	)	const
	{
		return Vector4f
		(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X,
			0.0f
		);
	}


	FORCEINLINE float & Vector4f::Component
	(
		const int32 Index
	)
	{
		return (&X)[Index];
	}


	FORCEINLINE void Vector4f::operator=
	(
		const Float4 F	
	)
	{
		X = F[0];
		Y = F[1];
		Z = F[2];
		W = F[3];
	}


	FORCEINLINE bool Vector4f::operator==
	(
		const Vector4f & V
	)	const
	{
		return (
			(X == V.X) && 
			(Y == V.Y) && 
			(Z == V.Z) && 
			(W == V.W));
	}


	FORCEINLINE bool Vector4f::operator!=
	(
		const Vector4f & V
	)	const
	{
		return (
			(X != V.X) || 
			(Y != V.Y) || 
			(Z != V.Z) || 
			(W != V.W));
	}


	FORCEINLINE bool Vector4f::Equals
	(
		const Vector4f & V, 
		const float		 Tolerance
	)	const
	{
		return 
			Math::Abs(X - V.X) <= Tolerance && 
			Math::Abs(Y - V.Y) <= Tolerance && 
			Math::Abs(Z - V.Z) <= Tolerance && 
			Math::Abs(W - V.W) <= Tolerance;
	}


	FORCEINLINE Vector4f Vector4f::GetSafeNormal
	(
		const float Tolerance
	)	const
	{
		const float SquareSum = 
			X * X + 
			Y * Y + 
			Z * Z;

		if (SquareSum > Tolerance)
		{
			const float Scale = Math::InvSqrt(SquareSum);

			return Vector4f
			(
				X * Scale, 
				Y * Scale, 
				Z * Scale, 
				0.0f);
		}

		return Vector4f();
	}


	CONSTEXPR Vector4f Vector4f::GetUnsafeNormal3() const
	{
		const float Scale = Math::InvSqrt(
			X * X + 
			Y * Y + 
			Z * Z);

		return Vector4f
		(
			X * Scale, 
			Y * Scale, 
			Z * Scale, 
			0.0f
		);
	}


	CONSTEXPR float Vector4f::Size3() const
	{
		return Math::Sqrt(
			X * X + 
			Y * Y + 
			Z * Z);
	}

	CONSTEXPR float Vector4f::SizeSquared3() const
	{
		return 
			X * X + 
			Y * Y + 
			Z * Z;
	}

	CONSTEXPR float Vector4f::Size() const
	{
		return Math::Sqrt(
			X * X + 
			Y * Y + 
			Z * Z + 
			W * W);
	}

	CONSTEXPR float Vector4f::SizeSquared() const
	{
		return X * X + Y * Y + Z * Z + W * W;
	}


	CONSTEXPR Vector4f Vector4f::Max(float V) const
	{
		return Vector4f
		(
			Math::Max(X, V),
			Math::Max(Y, V),
			Math::Max(Z, V),
			Math::Max(W, V)
		);
	}


	CONSTEXPR Vector4f Vector4f::Min(float V) const
	{
		return Vector4f
		(
			Math::Min(X, V),
			Math::Min(Y, V),
			Math::Min(Z, V),
			Math::Min(W, V)
		);
	}


	FORCEINLINE bool Vector4f::IsUnit3
	(
		const float LengthSquaredTolerance
	)	const
	{
		return Math::Abs(1.0f - SizeSquared3()) < LengthSquaredTolerance;
	}


	FORCEINLINE bool Vector4f::ContainsNaN() const
	{
		return (
			!Math::IsFinite(X) ||
			!Math::IsFinite(Y) ||
			!Math::IsFinite(Z) ||
			!Math::IsFinite(W));
	}


	FORCEINLINE bool Vector4f::IsNearlyZero3
	(
		const float Tolerance
	)	const
	{
		return
				Math::Abs(X) <= Tolerance
			&&	Math::Abs(Y) <= Tolerance
			&&	Math::Abs(Z) <= Tolerance;
	}


	FORCEINLINE Vector4f Vector4f::Reflect3
	(
		const Vector4f & Normal
	)	const
	{
		return 2.0f * Dot3(*this, Normal) * Normal - *this;
	}


	FORCEINLINE void Vector4f::FindBestAxisVectors3
	(
		Vector4f & Axis1, 
		Vector4f & Axis2
	)	const
	{
		const float NX = Math::Abs(X);
		const float NY = Math::Abs(Y);
		const float NZ = Math::Abs(Z);

		if (NZ > NX && NZ > NY)
		{
			Axis1 = Vector4f(1, 0, 0);
		}
		else
		{
			Axis1 = Vector4f(0, 0, 1);
		}

		Axis1 = (Axis1 - *this * Dot3(Axis1, *this)).GetSafeNormal();
		Axis2 = (Axis1 ^ *this);
	}


	FORCEINLINE Vector4f Vector4f::operator*=
	(
		const Vector4f & V
	)
	{
		X *= V.X; 
		Y *= V.Y; 
		Z *= V.Z; 
		W *= V.W;
		DiagnosticCheckNaN();
		return *this;
	}


	FORCEINLINE Vector4f Vector4f::operator/=
	(
		const Vector4f & V
	)
	{
		X /= V.X; 
		Y /= V.Y; 
		Z /= V.Z; 
		W /= V.W;
		DiagnosticCheckNaN();
		return *this;
	}


	FORCEINLINE Vector4f Vector4f::operator*=
	(
		const float S
	)
	{
		X *= S; 
		Y *= S; 
		Z *= S; 
		W *= S;
		DiagnosticCheckNaN();
		return *this;
	}


	FORCEINLINE Vector4f Vector4f::operator=
	(
		const Vector3f& V
	)
	{
		X = V.X;
		Y = V.Y;
		Z = V.Z;
		W = 1.0;
		return *this;
	}


	FORCEINLINE Vector4f Vector4f::operator/=
	(
		const float S
	)
	{
		X /= S;
		Y /= S;
		Z /= S;
		W /= S;
		DiagnosticCheckNaN();
		return *this;
	}


	FORCEINLINE Vector4f Vector4f::operator/
	(
		const Vector4f & V
	)	const
	{
		return Vector4f
		(
			X / V.X, 
			Y / V.Y, 
			Z / V.Z, 
			W / V.W
		);
	}

	constexpr FORCEINLINE Vector3f::Vector3f
	(
		const Vector4f & V
	)
		: X(V.X)
		, Y(V.Y)
		, Z(V.Z)
	{
	}


	FORCEINLINE Vector3f Vector4f::TransformToVector3() const
	{
		Vector3f Result;
		Result.X = X / W;
		Result.Y = Y / W;
		Result.Z = Z / W;
		return Result;
	}
}