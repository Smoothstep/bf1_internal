#pragma once

#include "Vector4.h"

namespace Hyper
{
	ALIGN(16) 
	struct Plane : public Vector3f
	{
	public:

		Float W;

	public:

		FORCEINLINE Plane();

		FORCEINLINE Plane
		(
			const Plane & P
		);

		FORCEINLINE Plane
		(
			const Vector4f & V
		);

		FORCEINLINE Plane
		(
			const	float InX, 
			const	float InY,
			const	float InZ,
			const	float InW
		);

		FORCEINLINE Plane
		(
			const	Vector3f &  InNormal,
			const	Float		InW
		);

		FORCEINLINE Plane
		(
			const	Vector3f &	InBase,
			const	Vector3f &	InNormal
		);

		FORCEINLINE Plane
		(
			const Vector3f & A,
			const Vector3f & B,
			const Vector3f & C
		);

		FORCEINLINE float PlaneDot
		(
			const Vector3f & P
		)	const;

		Plane Flip() const;


		Plane TransformBy
		(
			const Matrix4x4 & M
		)	const;

		Plane TransformByUsingAdjointT
		(
			const Matrix4x4	&	M, 
			const float			DetM,
			const Matrix4x4	&	TA
		)	const;

		bool operator==
		(
			const Plane & V
		)	const;

		bool operator!=
		(
			const Plane & V
		)	const;

		bool Equals
		(
			const Plane & V, 
			const float Tolerance = KINDA_SMALL_NUMBER
		)	const;

		FORCEINLINE float operator|
		(
			const Plane & V
		)	const;

		Plane operator+
		(
			const Plane & V
		)	const;

		Plane operator-
		(
			const Plane& V
		)	const;

		Plane operator/
		(
			const float Scale
		)	const;

		Plane operator*
		(
			const float Scale
		)	const;

		Plane operator*
		(
			const Plane & V
		);

		Plane operator+=
		(
			const Plane & V
		);

		Plane operator-=
		(
			const Plane & V
		);

		Plane operator*=
		(
			const float Scale
		);

		Plane operator*=
		(
			const Plane & V
		);

		Plane operator/=
		(
			const float V
		);
	};


	inline Vector3f Math::LinePlaneIntersection
	(
		const Vector3f	& Point1,
		const Vector3f	& Point2,
		const Plane		& Plane
	)
	{
		return Point1 + (Point2 - Point1) * ((Plane.W - (Point1 | Plane)) / ((Point2 - Point1) | Plane));
	}

	inline bool Math::IntersectPlanes3
	(
				Vector3f & I, 
		const	Plane	 & P1, 
		const	Plane	 & P2, 
		const	Plane	 & P3
	)
	{
		const Float Det = (P1 ^ P2) | P3;

		if (Square(Det) < Square(0.001f))
		{
			I = Vector3f::ZeroVector;
			return false;
		}
		else
		{
			I = 
			(
				P1.W * (P2 ^ P3) + 
				P2.W * (P3 ^ P1) + 
				P3.W * (P1 ^ P2)
			) / Det;
		}

		return true;
	}

	inline bool Math::IntersectPlanes2
	(
				Vector3f & I,
				Vector3f & D, 
		const	Plane	 & P1, 
		const	Plane	 & P2)
	{
		D = P1 ^ P2;

		const Float DD = D.SizeSquared();

		if (DD < Square(0.001f))
		{
			D = I = Vector3f::ZeroVector;
			return false;
		}
		else
		{
			I = 
			(
				P1.W * (P2 ^ D ) + 
				P2.W * (D  ^ P1)
			) / DD;

			D.Normalize();

			return true;
		}
	}

	inline Vector3f Vector3f::MirrorByPlane
	(
		const Plane & Plane
	)	const
	{
		return *this - Plane * (2.f * Plane.PlaneDot(*this));
	}

	inline Vector3f Vector3f::PointPlaneProject
	(
		const Vector3f	& Point, 
		const Plane		& Plane
	)
	{
		return Point - Plane.PlaneDot(Point) * Plane;
	}

	inline Vector3f Vector3f::PointPlaneProject
	(
		const Vector3f & Point, 
		const Vector3f & A, 
		const Vector3f & B, 
		const Vector3f & C
	)
	{
		Plane Plane(A, B, C);
		return Point - Plane.PlaneDot(Point) * Plane;
	}

	FORCEINLINE Plane::Plane()
		: Vector3f()
		, W(0)
	{}


	FORCEINLINE Plane::Plane
	(
		const Plane & P
	)
		: Vector3f(P)
		, W(P.W)
	{}


	FORCEINLINE Plane::Plane
	(
		const Vector4f & V
	)
		: Vector3f(V)
		, W(V.W)
	{}


	FORCEINLINE Plane::Plane
	(
		float InX, 
		float InY, 
		float InZ, 
		float InW
	)
		: Vector3f(InX, InY, InZ)
		, W(InW)
	{}


	FORCEINLINE Plane::Plane
	(
		const Vector3f &	InNormal, 
		const float			InW
	)
		: Vector3f(InNormal), W(InW)
	{}


	FORCEINLINE Plane::Plane
	(
		const Vector3f & InBase, 
		const Vector3f & InNormal
	)
		: Vector3f(InNormal)
		, W(InBase | InNormal)
	{}


	FORCEINLINE Plane::Plane
	(
		const Vector3f & A, 
		const Vector3f & B,
		const Vector3f & C
	)
		: Vector3f(((B - A) ^ (C - A)).GetSafeNormal())
	{
		W = A | (Vector3f)(*this);
	}

	FORCEINLINE float Plane::PlaneDot(const Vector3f &P) const
	{
		return X * P.X + Y * P.Y + Z * P.Z - W;
	}


	FORCEINLINE Plane Plane::Flip() const
	{
		return Plane(-X, -Y, -Z, -W);
	}

	FORCEINLINE bool Plane::operator==(const Plane& V) const
	{
		return (X == V.X) && (Y == V.Y) && (Z == V.Z) && (W == V.W);
	}


	FORCEINLINE bool Plane::operator!=(const Plane& V) const
	{
		return (X != V.X) || (Y != V.Y) || (Z != V.Z) || (W != V.W);
	}


	FORCEINLINE bool Plane::Equals(const Plane& V, float Tolerance) const
	{
		return 
			(Math::Abs(X - V.X) < Tolerance) && 
			(Math::Abs(Y - V.Y) < Tolerance) && 
			(Math::Abs(Z - V.Z) < Tolerance) && 
			(Math::Abs(W - V.W) < Tolerance);
	}


	FORCEINLINE float Plane::operator|(const Plane& V) const
	{
		return X * V.X + Y * V.Y + Z * V.Z + W * V.W;
	}


	FORCEINLINE Plane Plane::operator+(const Plane& V) const
	{
		return Plane(X + V.X, Y + V.Y, Z + V.Z, W + V.W);
	}


	FORCEINLINE Plane Plane::operator-(const Plane& V) const
	{
		return Plane(X - V.X, Y - V.Y, Z - V.Z, W - V.W);
	}


	FORCEINLINE Plane Plane::operator/(float Scale) const
	{
		const float RScale = 1.f / Scale;
		return Plane(X * RScale, Y * RScale, Z * RScale, W * RScale);
	}


	FORCEINLINE Plane Plane::operator*(float Scale) const
	{
		return Plane(X * Scale, Y * Scale, Z * Scale, W * Scale);
	}


	FORCEINLINE Plane Plane::operator*(const Plane& V)
	{
		return Plane(X * V.X, Y * V.Y, Z * V.Z, W * V.W);
	}


	FORCEINLINE Plane Plane::operator+=(const Plane& V)
	{
		X += V.X; Y += V.Y; Z += V.Z; W += V.W;
		return *this;
	}


	FORCEINLINE Plane Plane::operator-=(const Plane& V)
	{
		X -= V.X; Y -= V.Y; Z -= V.Z; W -= V.W;
		return *this;
	}


	FORCEINLINE Plane Plane::operator*=(float Scale)
	{
		X *= Scale; Y *= Scale; Z *= Scale; W *= Scale;
		return *this;
	}


	FORCEINLINE Plane Plane::operator*=(const Plane& V)
	{
		X *= V.X; Y *= V.Y; Z *= V.Z; W *= V.W;
		return *this;
	}


	FORCEINLINE Plane Plane::operator/=(float V)
	{
		const float RV = 1.f / V;
		X *= RV; Y *= RV; Z *= RV; W *= RV;
		return *this;
	}
}