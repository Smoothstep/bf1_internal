#pragma once

#include "Math.h"

namespace Hyper
{
	struct Vector2f
	{
		float X;
		float Y;

	public:
		static const Vector2f ZeroVector;
		static const Vector2f UnitVector;

	public:
		constexpr Vector2f();
		constexpr Vector2f
		(
			float InX, 
			float InY
		);

		constexpr Vector2f
		(
			IntPoint InPos
		);

		constexpr Vector2f
		(
			const Vector3f & V
		);

	public:
		constexpr Vector2f operator+
		(
			const Vector2f & V
		)	const;

		constexpr Vector2f operator-
		(
			const Vector2f & V
		)	const;

		constexpr Vector2f operator*
		(
			float Scale
		)	const;

		constexpr Vector2f operator/
		(
			float Scale
		)	const;

		constexpr Vector2f operator+
		(
			float A
		)	const;

		constexpr Vector2f operator-
		(
			float A
		)	const;

		constexpr Vector2f operator*
		(
			const Vector2f& V
		)	const;


		constexpr Vector2f operator/
		(
			const Vector2f& V
		)	const;

		constexpr float operator|
		(
			const Vector2f& V
		)	const;


		constexpr float operator^
		(
			const Vector2f& V
		)	const;

	public:
		constexpr bool operator==
		(
			const Vector2f& V
		)	const;

		constexpr bool operator!=
		(
			const Vector2f& V
		)	const;

		constexpr bool operator<
		(
			const Vector2f& Other
		)	const;

		constexpr bool operator>
		(
			const Vector2f& Other
		)	const;

		constexpr bool operator<=
		(
			const Vector2f& Other
		)	const;

		constexpr bool operator>=
		(	
			const Vector2f& Other
		)	const;

		constexpr Vector2f operator-() const;

		constexpr Vector2f operator+=
		(
			const Vector2f& V
		);

		constexpr Vector2f operator-=
		(
			const Vector2f& V
		);

		constexpr Vector2f operator*=
		(
			float Scale
		);

		constexpr Vector2f operator/=
		(
			float V
		);

		constexpr Vector2f operator*=
		(
			const Vector2f& V
		);

		constexpr Vector2f operator/=
		(
			const Vector2f& V
		);

		constexpr float& operator[]
		(
			int32 Index
		);

		constexpr float operator[]
		(
			int32 Index
		)	const;

		constexpr float& Component
		(
			int32 Index
		);

		constexpr float Component
		(
			int32 Index
		)	const;

	public:
		constexpr static float DotProduct
		(
			const Vector2f& A, 
			const Vector2f& B
		);

		constexpr static float DistSquared
		(
			const Vector2f& V1, 
			const Vector2f& V2
		);

		inline static float Distance
		(
			const Vector2f& V1, 
			const Vector2f& V2
		);

		constexpr static float CrossProduct
		(
			const Vector2f& A, 
			const Vector2f& B
		);

		constexpr bool Equals
		(
			const Vector2f& V, 
			float Tolerance = FLT_EPSILON
		)	const;

		constexpr void Set
		(
			float InX, 
			float InY
		);

		constexpr float GetMax()		const;
		constexpr float GetAbsMax()		const;
		constexpr float GetMin()		const;
		constexpr float SizeSquared()	const;

		inline float Size()	const;

		constexpr Vector2f GetRotated
		(
			float AngleDeg
		)	const;

		constexpr Vector2f GetSafeNormal
		(
			float Tolerance = FLT_EPSILON
		)	const;

		constexpr void Normalize
		(
			float Tolerance = FLT_EPSILON
		);

		constexpr bool IsNearlyZero
		(
			float Tolerance = FLT_EPSILON
		)	const;

		inline void ToDirectionAndLength
		(
			Vector2f	&OutDir, 
			float		&OutLength
		)	const;

		constexpr bool IsZero() const;

		inline IntPoint AsIntPoint() const;

		constexpr Vector2f ClampAxes
		(
			float MinAxisVal, 
			float MaxAxisVal
		)	const;


		constexpr Vector2f GetSignVector() const;
		constexpr Vector2f GetAbs() const;

		constexpr void DiagnosticCheckNaN() {}
		inline Vector3f SphericalToUnitCartesian() const;
	};


	constexpr Vector2f operator*
	(
		const float		 Scale,
		const Vector2f & V
	)
	{
		return V * Scale;
	}


	constexpr Vector2f::Vector2f
	(
		const float InX, 
		const float InY
	)
		: X(InX)
		, Y(InY)
	{ }


	constexpr Vector2f::Vector2f()
		: X(0.0f)
		, Y(0.0f)
	{ }


	constexpr Vector2f::Vector2f
	(
		const IntPoint InPos
	) 
		: X(static_cast<float>(InPos.X))
		, Y(static_cast<float>(InPos.Y))
	{ }

	constexpr Vector2f Vector2f::operator+
	(
		const Vector2f & V
	)	const
	{
		return Vector2f
		(
			X + V.X, 
			Y + V.Y
		);
	}


	constexpr Vector2f Vector2f::operator-
	(
		const Vector2f & V
	)	const
	{
		return Vector2f
		(
			X - V.X, 
			Y - V.Y
		);
	}


	constexpr Vector2f Vector2f::operator*
	(
		const float Scale
	)	const
	{
		return Vector2f
		(
			X * Scale, 
			Y * Scale
		);
	}


	constexpr Vector2f Vector2f::operator/
	(
		const float Scale
	)	const
	{
		const float RScale = 1.f / Scale;

		return Vector2f
		(
			X * RScale, 
			Y * RScale
		);
	}


	constexpr Vector2f Vector2f::operator+
	(
		const float A
	)	const
	{
		return Vector2f
		(
			X + A, 
			Y + A
		);
	}


	constexpr Vector2f Vector2f::operator-
	(
		const float A
	)	const
	{
		return Vector2f
		(
			X - A, 
			Y - A
		);
	}


	constexpr Vector2f Vector2f::operator*
	(
		const Vector2f & V
	)	const
	{
		return Vector2f
		(
			X * V.X, 
			Y * V.Y
		);
	}


	constexpr Vector2f Vector2f::operator/
	(
		const Vector2f & V
	)	const
	{
		return Vector2f
		(
			X / V.X, 
			Y / V.Y
		);
	}


	constexpr float Vector2f::operator|
	(
		const Vector2f & V
	)	const
	{
		return 
			X * V.X + 
			Y * V.Y;
	}


	constexpr float Vector2f::operator^
	(
		const Vector2f & V
	)	const
	{
		return 
			X * V.Y - 
			Y * V.X;
	}


	constexpr float Vector2f::DotProduct
	(
		const Vector2f & A, 
		const Vector2f & B
	)
	{
		return A | B;
	}


	constexpr float Vector2f::DistSquared
	(
		const Vector2f & V1, 
		const Vector2f & V2
	)
	{
		return 
			Math::Square(V2.X - V1.X) + 
			Math::Square(V2.Y - V1.Y);
	}


	inline float Vector2f::Distance
	(
		const Vector2f & V1, 
		const Vector2f & V2
	)
	{
		return Math::Sqrt(Vector2f::DistSquared(V1, V2));
	}


	constexpr float Vector2f::CrossProduct
	(
		const Vector2f & A, 
		const Vector2f & B
	)
	{
		return A ^ B;
	}


	constexpr bool Vector2f::operator==
	(
		const Vector2f & V
	)	const
	{
		return 
			X == V.X && 
			Y == V.Y;
	}


	constexpr bool Vector2f::operator!=
	(
		const Vector2f & V
	)	const
	{
		return 
			X != V.X || 
			Y != V.Y;
	}


	constexpr bool Vector2f::operator<
	(
		const Vector2f & Other
	)	const
	{
		return 
			X < Other.X && 
			Y < Other.Y;
	}


	constexpr bool Vector2f::operator>
	(
		const Vector2f & Other
	)	const
	{
		return 
			X > Other.X && 
			Y > Other.Y;
	}


	constexpr bool Vector2f::operator<=
	(
		const Vector2f & Other
	)	const
	{
		return 
			X <= Other.X && 
			Y <= Other.Y;
	}


	constexpr bool Vector2f::operator>=
	(
		const Vector2f & Other
	)	const
	{
		return 
			X >= Other.X && 
			Y >= Other.Y;
	}


	constexpr bool Vector2f::Equals
	(
		const	Vector2f & V, 
		const	float	   Tolerance
	)	const
	{
		return 
			Math::Abs(X - V.X) <= Tolerance && 
			Math::Abs(Y - V.Y) <= Tolerance;
	}


	constexpr Vector2f Vector2f::operator-() const
	{
		return Vector2f
		(
			-X, 
			-Y
		);
	}


	constexpr Vector2f Vector2f::operator+=
	(
		const Vector2f & V
	)
	{
		X += V.X; 
		Y += V.Y;
		return *this;
	}


	constexpr Vector2f Vector2f::operator-=
	(
		const Vector2f & V
	)
	{
		X -= V.X; 
		Y -= V.Y;
		return *this;
	}


	constexpr Vector2f Vector2f::operator*=
	(
		const float Scale
	)
	{
		X *= Scale; 
		Y *= Scale;
		return *this;
	}


	constexpr Vector2f Vector2f::operator/=
	(
		const float V
	)
	{
		const float RV = 1.f / V;
		X *= RV; 
		Y *= RV;
		return *this;
	}


	constexpr Vector2f Vector2f::operator*=
	(
		const Vector2f & V
	)
	{
		X *= V.X; 
		Y *= V.Y;
		return *this;
	}


	constexpr Vector2f Vector2f::operator/=
	(
		const Vector2f & V
	)
	{
		X /= V.X; 
		Y /= V.Y;
		return *this;
	}


	constexpr float& Vector2f::operator[]
	(
		const int32 Index
	)
	{
		return ((Index == 0) ? X : Y);
	}


	constexpr float Vector2f::operator[]
	(
		const int32 Index
	)	const
	{
		return ((Index == 0) ? X : Y);
	}


	constexpr void Vector2f::Set
	(
		const float InX,
		const float InY
	)
	{
		X = InX;
		Y = InY;
	}


	constexpr float Vector2f::GetMax() const
	{
		return Math::Max(X, Y);
	}


	constexpr float Vector2f::GetAbsMax() const
	{
		return Math::Max(
			Math::Abs(X), 
			Math::Abs(Y));
	}


	constexpr float Vector2f::GetMin() const
	{
		return Math::Min(X, Y);
	}


	inline float Vector2f::Size() const
	{
		return Math::Sqrt(
			X * X + 
			Y * Y);
	}


	constexpr float Vector2f::SizeSquared() const
	{
		return 
			X * X + 
			Y * Y;
	}


	constexpr Vector2f Vector2f::GetRotated
	(
		const float AngleDeg
	)	const
	{
		float S = 0.f;
		float C = 0.f;
		{
			Math::SinCos(&S, &C, Math::DegreesToRadians(AngleDeg));
		}

		return Vector2f
		(
			C * X - S * Y,
			S * X + C * Y
		);
	}


	constexpr Vector2f Vector2f::GetSafeNormal
	(
		const float Tolerance
	)	const
	{
		const float SquareSum = X * X + Y * Y;

		if (SquareSum > Tolerance)
		{
			const float Scale = Math::InvSqrt(SquareSum);

			return Vector2f
			(
				X * Scale, 
				Y * Scale
			);
		}

		return Vector2f(0.f, 0.f);
	}

	constexpr void Vector2f::Normalize
	(
		const float Tolerance
	)
	{
		const float SquareSum = X * X + Y * Y;

		if (SquareSum > Tolerance)
		{
			const float Scale = Math::InvSqrt(SquareSum);

			X *= Scale;
			Y *= Scale;
		}
		else
		{
			X = 0.0f;
			Y = 0.0f;
		}
	}

	inline void Vector2f::ToDirectionAndLength
	(
		Vector2f & OutDir, 
		float	 & OutLength
	)	const
	{
		OutLength = Size();

		if (OutLength > FLT_EPSILON)
		{
			float OneOverLength = 1.0f / OutLength;

			OutDir = Vector2f
			( 
				X * OneOverLength, 
				Y * OneOverLength
			);
		}
		else
		{
			OutDir = Vector2f::ZeroVector;
		}
	}


	constexpr bool Vector2f::IsNearlyZero
	(
		const float Tolerance
	)	const
	{
		return	Math::Abs(X) <= Tolerance
			&&	Math::Abs(Y) <= Tolerance;
	}


	constexpr bool Vector2f::IsZero() const
	{
		return 
			X == 0.f && 
			Y == 0.f;
	}


	constexpr float& Vector2f::Component
	(
		const int32 Index
	)
	{
		return (&X)[Index];
	}


	constexpr float Vector2f::Component
	(
		const int32 Index
	)	const
	{
		return (&X)[Index];
	}


	inline Hyper::IntPoint Vector2f::AsIntPoint() const
	{
		return Hyper::IntPoint
		(
			Math::RoundToInt(X), 
			Math::RoundToInt(Y)
		);
	}


	constexpr Vector2f Vector2f::ClampAxes
	(
		const float MinAxisVal, 
		const float MaxAxisVal
	)	const
	{
		return Vector2f
		(
			Math::Clamp(X, MinAxisVal, MaxAxisVal), 
			Math::Clamp(Y, MinAxisVal, MaxAxisVal)
		);
	}


	constexpr Vector2f Vector2f::GetSignVector() const
	{
		return Vector2f
		(
			Math::FloatSelect(X, 1.f, -1.f),
			Math::FloatSelect(Y, 1.f, -1.f)
		);
	}

	constexpr Vector2f Vector2f::GetAbs() const
	{
		return Vector2f
		(
			Math::Abs(X), 
			Math::Abs(Y)
		);
	}
}