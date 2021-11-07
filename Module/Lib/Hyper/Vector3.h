#pragma once

#include "Vector2.h"

namespace Hyper
{
	struct Vector3f
	{
	public:
		Float X;
		Float Y;
		Float Z;

	public:
		static const Vector3f ZeroVector;
		static const Vector3f UpVector;
		static const Vector3f DownVector;
		static const Vector3f ForwardVector;
		static const Vector3f BackwardVector;
		static const Vector3f RightVector;
		static const Vector3f LeftVector;

	public:
		constexpr FORCEINLINE Vector3f();

		constexpr FORCEINLINE Vector3f
		(
			Float InF
		);

		constexpr FORCEINLINE Vector3f
		(
			Float InX, 
			Float InY, 
			Float InZ
		);

		constexpr FORCEINLINE Vector3f
		(
			const	Vector2f	V, 
			const	Float		InZ
		);

		constexpr FORCEINLINE Vector3f
		(
			const Vector4f& V
		);

		constexpr FORCEINLINE Vector3f
		(
			const Quaternion& Q
		);

		constexpr FORCEINLINE Vector3f
		(
			IntPoint A
		);

		FORCEINLINE Vector3f operator^
		(
			const Vector3f& V
		)	const;

		FORCEINLINE static Vector3f CrossProduct
		(
			const Vector3f& A, 
			const Vector3f& B
		);

		FORCEINLINE Float operator|
		(
			const Vector3f& V
		)	const;

		FORCEINLINE static Float DotProduct
		(
			const Vector3f& A, 
			const Vector3f& B
		);

		FORCEINLINE Vector3f operator+
		(
			const Vector3f& V
		)	const;

		FORCEINLINE Vector3f operator-
		(
			const Vector3f& V
		)	const;

		FORCEINLINE Vector3f operator-
		(
			Float Bias
		)	const;

		FORCEINLINE Vector3f operator+
		(
			Float Bias
		)	const;

		FORCEINLINE Vector3f operator*
		(
			Float Scale
		)	const;

		FORCEINLINE Vector3f operator/
		(
			Float Scale
		)	const;

		FORCEINLINE Vector3f operator*
		(
			const Vector3f& V
		)	const;

		FORCEINLINE Vector3f operator/
		(
			const Vector3f& V
		)	const;

		FORCEINLINE bool operator==
		(
			const Vector3f& V
		)	const;

		FORCEINLINE bool operator!=
		(
			const Vector3f& V
		)	const;

		FORCEINLINE bool Equals
		(
			const	Vector3f&	V, 
			const	Float		Tolerance = KINDA_SMALL_NUMBER
		)	const;

		FORCEINLINE bool AllComponentsEqual
		(
			const	Float Tolerance = KINDA_SMALL_NUMBER
		)	const;

		FORCEINLINE Vector3f operator-() const;

		FORCEINLINE Vector3f operator+=
		(
			const Vector3f& V
		);

		FORCEINLINE Vector3f operator-=
		(
			const Vector3f& V
		);

		FORCEINLINE Vector3f operator*=
		(
			Float Scale
		);

		FORCEINLINE Vector3f operator/=
		(
			Float V
		);

		FORCEINLINE Vector3f operator*=
		(
			const Vector3f& V
		);

		FORCEINLINE Vector3f operator/=
		(
			const Vector3f& V
		);

		FORCEINLINE Float& operator[]
		(
			int32 Index
		);

		FORCEINLINE Float operator[]
		(
			int32 Index
		)	const;

		FORCEINLINE Float& Component
		(
			int32 Index
		);

		FORCEINLINE Float Component
		(
			int32 Index
		)	const;

	public:
		FORCEINLINE void Set
		(
			Float InX, 
			Float InY, 
			Float InZ
		);

		FORCEINLINE Float GetMax()		const;
		FORCEINLINE Float GetMin()		const;
		FORCEINLINE Float GetAbsMin()	const;
		FORCEINLINE Float GetAbsMax()	const;

		FORCEINLINE Vector3f ComponentMin
		(
			const Vector3f& Other
		)	const;

		FORCEINLINE Vector3f ComponentMax
		(
			const Vector3f& Other
		)	const;

		FORCEINLINE Vector3f GetAbs() const;

		FORCEINLINE Float Size()			const;
		FORCEINLINE Float Size2D()			const;
		FORCEINLINE Float SizeSquared()		const;
		FORCEINLINE Float SizeSquared2D()	const;

		FORCEINLINE bool IsNearlyZero
		(
			Float Tolerance = KINDA_SMALL_NUMBER
		)	const;

		FORCEINLINE bool IsZero() const;

		FORCEINLINE bool Normalize
		(
			Float Tolerance = SMALL_NUMBER
		);
		FORCEINLINE bool Normalize
		(
					Vector3f &	Result,
			const	Float		Tolerance = SMALL_NUMBER
		)	const;
		FORCEINLINE Vector3f GetNormalized
		(
			const Float Tolerance = SMALL_NUMBER
		)	const;

		FORCEINLINE bool IsNormalized() const;

		FORCEINLINE void RotateByQuaternion
		(
			const Quaternion & Quaternion
		);

		FORCEINLINE void RotateByMatrix
		(
			const Matrix4x4 & RotationMatrix
		);

		FORCEINLINE void ToDirectionAndLength
		(
			Vector3f	&	OutDir, 
			Float		&	OutLength
		)	const;

		FORCEINLINE Vector3f Projection()		const;
		FORCEINLINE Vector3f Squared()			const;
		FORCEINLINE Vector3f GetSignVector()	const;
		FORCEINLINE Vector3f GetUnsafeNormal()	const;
		FORCEINLINE Vector3f UnsafeNormal()		const;

		FORCEINLINE Vector3f GridSnap
		(
			const Float& GridSz
		)	const;

		FORCEINLINE Vector3f BoundToCube
		(
			Float Radius
		)	const;

		FORCEINLINE Vector3f GetClampedToSize
		(
			Float Min,
			Float Max
		)	const;

		FORCEINLINE Vector3f GetClampedToSize2D
		(
			Float Min, 
			Float Max
		)	const;

		FORCEINLINE Vector3f GetClampedToMaxSize
		(
			Float MaxSize
		)	const;

		FORCEINLINE Vector3f GetClampedToMaxSize2D
		(
			Float MaxSize
		)	const;

		FORCEINLINE void AddBounded
		(
			const	Vector3f &	V, 
			const	Float		Radius = UINT16_MAX
		);

		FORCEINLINE Vector3f Reciprocal() const;

		FORCEINLINE bool IsUniform
		(
			const	Float Tolerance = KINDA_SMALL_NUMBER
		)	const;

		FORCEINLINE Vector3f MirrorByVector
		(
			const	Vector3f & MirrorNormal
		)	const;

		FORCEINLINE Vector3f MirrorByPlane
		(
			const	Plane& Plane
		)	const;

		FORCEINLINE Vector3f RotateAngleAxis
		(
			const	Float		AngleDeg, 
			const	Vector3f &	Axis
		)	const;

		FORCEINLINE Vector3f GetSafeNormal
		(
			const	Float Tolerance = SMALL_NUMBER
		)	const;

		FORCEINLINE Vector3f GetSafeNormal2D
		(
			const	Float Tolerance = SMALL_NUMBER
		)	const;

		FORCEINLINE Float CosineAngle2D
		(
			Vector3f B
		)	const;

		FORCEINLINE Vector3f ProjectOnTo
		(
			const Vector3f & A
		)	const;

		FORCEINLINE Vector3f ProjectOnToNormal
		(
			const Vector3f & Normal
		)	const;

		void FindBestAxisVectors
		(
			Vector3f & Axis1, 
			Vector3f & Axis2
		)	const;

		void UnwindEuler();

		Quaternion	ToOrientationQuat()		const;
		Rotation	ToOrientationRotation() const;

		FORCEINLINE bool ContainsNaN() const;

		FORCEINLINE bool IsUnit
		(
			const	Float LengthSquaredTolerance = KINDA_SMALL_NUMBER
		)	const;

		FORCEINLINE Float HeadingAngle() const;

		static void CreateOrthonormalBasis
		(
			Vector3f & XAxis, 
			Vector3f & YAxis, 
			Vector3f & ZAxis
		);

		static bool PointsAreSame
		(
			const Vector3f & P, 
			const Vector3f & Q
		);

		static bool PointsAreNear
		(
			const	Vector3f &	Point1, 
			const	Vector3f &	Point2, 
			const	Float		Dist
		);

		static Float PointPlaneDist
		(
			const Vector3f & Point, 
			const Vector3f & PlaneBase, 
			const Vector3f & PlaneNormal
		);

		static Vector3f PointPlaneProject
		(
			const Vector3f	& Point, 
			const Plane		& Plane
		);

		static Vector3f PointPlaneProject
		(
			const Vector3f & Point, 
			const Vector3f & A, 
			const Vector3f & B, 
			const Vector3f & C
		);

		static Vector3f PointPlaneProject
		(
			const Vector3f & Point, 
			const Vector3f & PlaneBase, 
			const Vector3f & PlaneNormal
		);

		static Vector3f VectorPlaneProject
		(
			const Vector3f & V, 
			const Vector3f & PlaneNormal
		);

		static FORCEINLINE Float Dist
		(
			const Vector3f & V1, 
			const Vector3f & V2
		);

		static FORCEINLINE Float Distance
		(
			const Vector3f & V1, 
			const Vector3f & V2
		) 
		{ 
			return Dist(V1, V2); 
		}

		static FORCEINLINE Float DistXY
		(
			const Vector3f & V1, 
			const Vector3f & V2
		);

		static FORCEINLINE Float Dist2D
		(
			const Vector3f & V1, 
			const Vector3f & V2
		) 
		{ 
			return DistXY(V1, V2); 
		}

		static FORCEINLINE Float DistSquared
		(
			const Vector3f & V1, 
			const Vector3f & V2
		);

		static FORCEINLINE Float DistSquaredXZ
		(
			const Vector3f & V1, 
			const Vector3f & V2
		);

		static FORCEINLINE Float DistSquared2D
		(
			const Vector3f & V1, 
			const Vector3f & V2
		) 
		{ 
			return DistSquaredXZ(V1, V2); 
		}

		static FORCEINLINE Float BoxPushOut
		(
			const Vector3f & Normal, 
			const Vector3f & Size
		);

		static bool Parallel
		(
			const Vector3f &	Normal1, 
			const Vector3f &	Normal2, 
			const Float			ParallelCosineThreshold = THRESH_NORMALS_ARE_PARALLEL
		);

		static bool Coincident
		(
			const Vector3f &	Normal1, 
			const Vector3f &	Normal2, 
			const Float			ParallelCosineThreshold = THRESH_NORMALS_ARE_PARALLEL
		);

		static bool Orthogonal
		(
			const Vector3f &	Normal1, 
			const Vector3f &	Normal2, 
			const Float			OrthogonalCosineThreshold = THRESH_NORMALS_ARE_ORTHOGONAL
		);

		static bool Coplanar
		(
			const Vector3f &	Base1, 
			const Vector3f &	Normal1, 
			const Vector3f &	Base2, 
			const Vector3f &	Normal2, 
			const Float			ParallelCosineThreshold = THRESH_NORMALS_ARE_PARALLEL
		);

		static Float Triple
		(
			const Vector3f & X, 
			const Vector3f & Y, 
			const Vector3f & Z
		);

		static  Float EvaluateBezier
		(
			const	Vector3f			*	ControlPoints, 
			const	int32					NumPoints, 
					std::vector<Vector3f> &	OutPoints
		);

		static Vector3f RadiansToDegrees
		(
			const Vector3f& RadVector
		);

		static Vector3f DegreesToRadians
		(
			const Vector3f& DegVector
		);

		static  void GenerateClusterCenters
		(
					std::vector<Vector3f> & Clusters, 
			const	std::vector<Vector3f> & Points, 
			const	int32					NumIterations, 
			const	int32					NumConnectionsToBeValid
		);
	};

	FORCEINLINE Vector3f operator*
	(
		const	Float		Scale, 
		const	Vector3f &	V
	)
	{
		return V * Scale;
	}

#define INTEL_ORDER_VECTOR(x) (x)

	FORCEINLINE Float ComputeSquaredDistanceFromBoxToPoint
	(
		const Vector3f & Mins, 
		const Vector3f & Maxs, 
		const Vector3f & Point
	)
	{
		Float DistSquared = 0.f;

		if (Point.X < Mins.X)
		{
			DistSquared += Math::Square(Point.X - Mins.X);
		}
		else if (Point.X > Maxs.X)
		{
			DistSquared += Math::Square(Point.X - Maxs.X);
		}

		if (Point.Y < Mins.Y)
		{
			DistSquared += Math::Square(Point.Y - Mins.Y);
		}
		else if (Point.Y > Maxs.Y)
		{
			DistSquared += Math::Square(Point.Y - Maxs.Y);
		}

		if (Point.Z < Mins.Z)
		{
			DistSquared += Math::Square(Point.Z - Mins.Z);
		}
		else if (Point.Z > Maxs.Z)
		{
			DistSquared += Math::Square(Point.Z - Maxs.Z);
		}

		return DistSquared;
	}

	constexpr FORCEINLINE Vector3f::Vector3f
	(
		const	Vector2f	V, 
				Float		InZ)
		: X(V.X), Y(V.Y), Z(InZ)
	{}

	inline Vector3f Vector3f::RotateAngleAxis
	(
		const Float			AngleDeg, 
		const Vector3f &	Axis
	)	const
	{
		Float S, C;
		Math::SinCos(&S, &C, Math::DegreesToRadians(AngleDeg));

		const Float XX = Axis.X * Axis.X;
		const Float YY = Axis.Y * Axis.Y;
		const Float ZZ = Axis.Z * Axis.Z;

		const Float XY = Axis.X * Axis.Y;
		const Float YZ = Axis.Y * Axis.Z;
		const Float ZX = Axis.Z * Axis.X;

		const Float XS = Axis.X * S;
		const Float YS = Axis.Y * S;
		const Float ZS = Axis.Z * S;

		const Float OMC = 1.f - C;

		return Vector3f(
			(OMC * XX + C) * X + (OMC * XY - ZS) * Y + (OMC * ZX + YS) * Z,
			(OMC * XY + ZS) * X + (OMC * YY + C) * Y + (OMC * YZ - XS) * Z,
			(OMC * ZX - YS) * X + (OMC * YZ + XS) * Y + (OMC * ZZ + C) * Z
		);
	}

	inline bool Vector3f::PointsAreSame(const Vector3f &P, const Vector3f &Q)
	{
		Float Temp;
		Temp = P.X - Q.X;
		if ((Temp > -THRESH_POINTS_ARE_SAME) && (Temp < THRESH_POINTS_ARE_SAME))
		{
			Temp = P.Y - Q.Y;
			if ((Temp > -THRESH_POINTS_ARE_SAME) && (Temp < THRESH_POINTS_ARE_SAME))
			{
				Temp = P.Z - Q.Z;
				if ((Temp > -THRESH_POINTS_ARE_SAME) && (Temp < THRESH_POINTS_ARE_SAME))
				{
					return true;
				}
			}
		}
		return false;
	}

	inline bool Vector3f::PointsAreNear(const Vector3f &Point1, const Vector3f &Point2, Float Dist)
	{
		Float Temp;
		Temp = (Point1.X - Point2.X); if (Math::Abs(Temp) >= Dist) return false;
		Temp = (Point1.Y - Point2.Y); if (Math::Abs(Temp) >= Dist) return false;
		Temp = (Point1.Z - Point2.Z); if (Math::Abs(Temp) >= Dist) return false;
		return true;
	}

	inline Float Vector3f::PointPlaneDist
	(
		const Vector3f &Point,
		const Vector3f &PlaneBase,
		const Vector3f &PlaneNormal
	)
	{
		return (Point - PlaneBase) | PlaneNormal;
	}


	inline Vector3f Vector3f::PointPlaneProject
	(
		const Vector3f & Point, 
		const Vector3f & PlaneBase, 
		const Vector3f & PlaneNorm
	)
	{
		//Find the distance of X from the plane
		//Add the distance back along the normal from the point
		return Point - Vector3f::PointPlaneDist(Point, PlaneBase, PlaneNorm) * PlaneNorm;
	}

	inline Vector3f Vector3f::VectorPlaneProject
	(
		const Vector3f & V, 
		const Vector3f & PlaneNormal
	)
	{
		return V - V.ProjectOnToNormal(PlaneNormal);
	}

	inline bool Vector3f::Parallel
	(
		const Vector3f & Normal1, 
		const Vector3f & Normal2, 
		const Float		 ParallelCosineThreshold
	)
	{
		const Float NormalDot = Normal1 | Normal2;
		return Math::Abs(NormalDot) >= ParallelCosineThreshold;
	}

	inline bool Vector3f::Coincident
	(
		const Vector3f & Normal1, 
		const Vector3f & Normal2, 
		const Float		 ParallelCosineThreshold
	)
	{
		const Float NormalDot = Normal1 | Normal2;
		return NormalDot >= ParallelCosineThreshold;
	}

	inline bool Vector3f::Orthogonal
	(
		const Vector3f & Normal1, 
		const Vector3f & Normal2, 
		const Float		 OrthogonalCosineThreshold
	)
	{
		const Float NormalDot = Normal1 | Normal2;
		return Math::Abs(NormalDot) <= OrthogonalCosineThreshold;
	}

	inline bool Vector3f::Coplanar
	(
		const Vector3f & Base1, 
		const Vector3f & Normal1, 
		const Vector3f & Base2, 
		const Vector3f & Normal2, 
		const Float		 ParallelCosineThreshold
	)
	{
		if (!Vector3f::Parallel(Normal1, Normal2, ParallelCosineThreshold))
		{
			return false;
		}
		
		if (Vector3f::PointPlaneDist(Base2, Base1, Normal1) > THRESH_POINT_ON_PLANE)
		{
			return false;
		}

		return true;
	}

	inline Float Vector3f::Triple
	(
		const Vector3f & X, 
		const Vector3f & Y, 
		const Vector3f & Z
	)
	{
		return
			(
				(X.X * (Y.Y * Z.Z - Y.Z * Z.Y)) + 
				(X.Y * (Y.Z * Z.X - Y.X * Z.Z)) + 
				(X.Z * (Y.X * Z.Y - Y.Y * Z.X))
			);
	}

	inline Vector3f Vector3f::RadiansToDegrees
	(
		const Vector3f& RadVector
	)
	{
		return RadVector * (180.f / PI);
	}

	inline Vector3f Vector3f::DegreesToRadians
	(
		const Vector3f& DegVector
	)
	{
		return DegVector * (PI / 180.f);
	}

	constexpr FORCEINLINE Vector3f::Vector3f()
		: X(0.0f)
		, Y(0.0f)
		, Z(0.0f)
	{}

	constexpr FORCEINLINE Vector3f::Vector3f
	(
		Float InF
	)
		: X(InF)
		, Y(InF)
		, Z(InF)
	{}

	constexpr FORCEINLINE Vector3f::Vector3f
	(
		Float InX, 
		Float InY, 
		Float InZ
	)
		: X(InX)
		, Y(InY)
		, Z(InZ)
	{}

	constexpr FORCEINLINE Vector3f::Vector3f(const IntPoint A)
		: X(static_cast<float>(A.X))
		, Y(static_cast<float>(A.Y))
		, Z(0.f)
	{}


#ifdef IMPLEMENT_ASSIGNMENT_OPERATOR_MANUALLY
	FORCEINLINE Vector3f& Vector3f::operator=(const Vector3f& Other)
	{
		this->X = Other.X;
		this->Y = Other.Y;
		this->Z = Other.Z;

		

		return *this;
	}
#endif

	FORCEINLINE Vector3f Vector3f::operator^(const Vector3f& V) const
	{
		return Vector3f
		(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X
		);
	}

	FORCEINLINE Vector3f Vector3f::CrossProduct(const Vector3f& A, const Vector3f& B)
	{
		return A ^ B;
	}

	FORCEINLINE Float Vector3f::operator|(const Vector3f& V) const
	{
		return X*V.X + Y*V.Y + Z*V.Z;
	}

	FORCEINLINE Float Vector3f::DotProduct(const Vector3f& A, const Vector3f& B)
	{
		return A | B;
	}

	FORCEINLINE Vector3f Vector3f::operator+(const Vector3f& V) const
	{
		return Vector3f(X + V.X, Y + V.Y, Z + V.Z);
	}

	FORCEINLINE Vector3f Vector3f::operator-(const Vector3f& V) const
	{
		return Vector3f(X - V.X, Y - V.Y, Z - V.Z);
	}

	FORCEINLINE Vector3f Vector3f::operator-(Float Bias) const
	{
		return Vector3f(X - Bias, Y - Bias, Z - Bias);
	}

	FORCEINLINE Vector3f Vector3f::operator+(Float Bias) const
	{
		return Vector3f(X + Bias, Y + Bias, Z + Bias);
	}

	FORCEINLINE Vector3f Vector3f::operator*(Float Scale) const
	{
		return Vector3f(X * Scale, Y * Scale, Z * Scale);
	}

	FORCEINLINE Vector3f Vector3f::operator/(Float Scale) const
	{
		const Float RScale = 1.f / Scale;
		return Vector3f(X * RScale, Y * RScale, Z * RScale);
	}

	FORCEINLINE Vector3f Vector3f::operator*(const Vector3f& V) const
	{
		return Vector3f(X * V.X, Y * V.Y, Z * V.Z);
	}

	FORCEINLINE Vector3f Vector3f::operator/(const Vector3f& V) const
	{
		return Vector3f(X / V.X, Y / V.Y, Z / V.Z);
	}

	FORCEINLINE bool Vector3f::operator==(const Vector3f& V) const
	{
		return X == V.X && Y == V.Y && Z == V.Z;
	}

	FORCEINLINE bool Vector3f::operator!=(const Vector3f& V) const
	{
		return X != V.X || Y != V.Y || Z != V.Z;
	}

	FORCEINLINE bool Vector3f::Equals(const Vector3f& V, Float Tolerance) const
	{
		return Math::Abs(X - V.X) <= Tolerance && Math::Abs(Y - V.Y) <= Tolerance && Math::Abs(Z - V.Z) <= Tolerance;
	}

	FORCEINLINE bool Vector3f::AllComponentsEqual(Float Tolerance) const
	{
		return Math::Abs(X - Y) <= Tolerance && Math::Abs(X - Z) <= Tolerance && Math::Abs(Y - Z) <= Tolerance;
	}


	FORCEINLINE Vector3f Vector3f::operator-() const
	{
		return Vector3f(-X, -Y, -Z);
	}


	FORCEINLINE Vector3f Vector3f::operator+=(const Vector3f& V)
	{
		X += V.X; Y += V.Y; Z += V.Z;
		
		return *this;
	}

	FORCEINLINE Vector3f Vector3f::operator-=(const Vector3f& V)
	{
		X -= V.X; Y -= V.Y; Z -= V.Z;
		
		return *this;
	}

	FORCEINLINE Vector3f Vector3f::operator*=(Float Scale)
	{
		X *= Scale; Y *= Scale; Z *= Scale;
		
		return *this;
	}

	FORCEINLINE Vector3f Vector3f::operator/=(Float V)
	{
		const Float RV = 1.f / V;
		X *= RV; Y *= RV; Z *= RV;
		
		return *this;
	}

	FORCEINLINE Vector3f Vector3f::operator*=(const Vector3f& V)
	{
		X *= V.X; Y *= V.Y; Z *= V.Z;
		
		return *this;
	}

	FORCEINLINE Vector3f Vector3f::operator/=(const Vector3f& V)
	{
		X /= V.X; Y /= V.Y; Z /= V.Z;
		
		return *this;
	}

	FORCEINLINE Float& Vector3f::operator[](int32 Index)
	{
		if (Index == 0)
		{
			return X;
		}
		else if (Index == 1)
		{
			return Y;
		}
		else
		{
			return Z;
		}
	}

	FORCEINLINE Float Vector3f::operator[](int32 Index)const
	{
		if (Index == 0)
		{
			return X;
		}
		else if (Index == 1)
		{
			return Y;
		}
		else
		{
			return Z;
		}
	}

	FORCEINLINE void Vector3f::Set(Float InX, Float InY, Float InZ)
	{
		X = InX;
		Y = InY;
		Z = InZ;
		
	}

	FORCEINLINE Float Vector3f::GetMax() const
	{
		return Math::Max(Math::Max(X, Y), Z);
	}

	FORCEINLINE Float Vector3f::GetAbsMax() const
	{
		return Math::Max(Math::Max(Math::Abs(X), Math::Abs(Y)), Math::Abs(Z));
	}

	FORCEINLINE Float Vector3f::GetMin() const
	{
		return Math::Min(Math::Min(X, Y), Z);
	}

	FORCEINLINE Float Vector3f::GetAbsMin() const
	{
		return Math::Min(Math::Min(Math::Abs(X), Math::Abs(Y)), Math::Abs(Z));
	}

	FORCEINLINE Vector3f Vector3f::ComponentMin(const Vector3f& Other) const
	{
		return Vector3f(Math::Min(X, Other.X), Math::Min(Y, Other.Y), Math::Min(Z, Other.Z));
	}

	FORCEINLINE Vector3f Vector3f::ComponentMax(const Vector3f& Other) const
	{
		return Vector3f(Math::Max(X, Other.X), Math::Max(Y, Other.Y), Math::Max(Z, Other.Z));
	}

	FORCEINLINE Vector3f Vector3f::GetAbs() const
	{
		return Vector3f(Math::Abs(X), Math::Abs(Y), Math::Abs(Z));
	}

	FORCEINLINE Float Vector3f::Size() const
	{
		return Math::Sqrt(X * X + Y * Y + Z * Z);
	}

	FORCEINLINE Float Vector3f::SizeSquared() const
	{
		return 
			X * X + 
			Y * Y + 
			Z * Z;
	}

	FORCEINLINE Float Vector3f::Size2D() const
	{
		return Math::Sqrt(X * X + Z * Z);
	}

	FORCEINLINE Float Vector3f::SizeSquared2D() const
	{
		return 
			X * X + 
			Z * Z;
	}

	FORCEINLINE bool Vector3f::IsNearlyZero(Float Tolerance) const
	{
		return
				Math::Abs(X) <= Tolerance
			&&	Math::Abs(Y) <= Tolerance
			&&	Math::Abs(Z) <= Tolerance;
	}

	FORCEINLINE bool Vector3f::IsZero() const
	{
		return 
			X == 0.f && 
			Y == 0.f && 
			Z == 0.f;
	}

	FORCEINLINE bool Vector3f::Normalize
	(
				Vector3f  &	Result,
		const	Float		Tolerance
	)	const
	{
		const Float SquareSum = X * X + Y * Y + Z * Z;

		if (SquareSum > Tolerance)
		{
			const Float Scale = Math::InvSqrt(SquareSum);

			Result.X *= Scale;
			Result.Y *= Scale;
			Result.Z *= Scale;

			return true;
		}

		return false;
	}

	FORCEINLINE Vector3f Vector3f::GetNormalized
	(
		const Float Tolerance
	)	const
	{
		Vector3f Result(*this);

		const Float SquareSum = X * X + Y * Y + Z * Z;

		if (SquareSum > Tolerance)
		{
			const Float Scale = Math::InvSqrt(SquareSum);

			Result.X *= Scale;
			Result.Y *= Scale;
			Result.Z *= Scale;
		}

		return Result;;
	}

	FORCEINLINE bool Vector3f::Normalize(Float Tolerance)
	{
		const Float SquareSum = X * X + Y * Y + Z * Z;

		if (SquareSum > Tolerance)
		{
			const Float Scale = Math::InvSqrt(SquareSum);

			X *= Scale; 
			Y *= Scale; 
			Z *= Scale;

			return true;
		}

		return false;
	}

	FORCEINLINE bool Vector3f::IsNormalized() const
	{
		return (Math::Abs(1.f - SizeSquared()) < THRESH_VECTOR_NORMALIZED);
	}

	FORCEINLINE void Vector3f::ToDirectionAndLength(Vector3f &OutDir, Float &OutLength) const
	{
		OutLength = Size();
		if (OutLength > SMALL_NUMBER)
		{
			Float OneOverLength = 1.0f / OutLength;
			OutDir = Vector3f(X*OneOverLength, Y*OneOverLength,
				Z*OneOverLength);
		}
		else
		{
			OutDir = Vector3f::ZeroVector;
		}
	}

	FORCEINLINE Vector3f Vector3f::Squared() const
	{
		return Vector3f
		(
			X * X,
			Y * Y,
			Z * Z
		);
	}

	FORCEINLINE Vector3f Vector3f::GetSignVector() const
	{
		return Vector3f
		(
			Math::FloatSelect(X, 1.f, -1.f),
			Math::FloatSelect(Y, 1.f, -1.f),
			Math::FloatSelect(Z, 1.f, -1.f)
		);
	}

	FORCEINLINE Vector3f Vector3f::Projection() const
	{
		const Float RZ = 1.f / Z;
		return Vector3f(X*RZ, Y*RZ, 1);
	}

	FORCEINLINE Vector3f Vector3f::GetUnsafeNormal() const
	{
		const Float Scale = Math::InvSqrt(X*X + Y*Y + Z*Z);
		return Vector3f(X*Scale, Y*Scale, Z*Scale);
	}

	FORCEINLINE Vector3f Vector3f::UnsafeNormal() const
	{
		return GetUnsafeNormal();
	}

	FORCEINLINE Vector3f Vector3f::GridSnap(const Float& GridSz) const
	{
		return Vector3f
		(
			Math::GridSnap(X, GridSz), 
			Math::GridSnap(Y, GridSz), 
			Math::GridSnap(Z, GridSz)
		);
	}

	FORCEINLINE Vector3f Vector3f::BoundToCube(Float Radius) const
	{
		return Vector3f
		(
			Math::Clamp(X, -Radius, Radius),
			Math::Clamp(Y, -Radius, Radius),
			Math::Clamp(Z, -Radius, Radius)
		);
	}

	FORCEINLINE Vector3f Vector3f::GetClampedToSize(Float Min, Float Max) const
	{
		Float VecSize = Size();
		const Vector3f VecDir = (VecSize > SMALL_NUMBER) ? (*this / VecSize) : Vector3f::ZeroVector;

		VecSize = Math::Clamp(VecSize, Min, Max);

		return VecSize * VecDir;
	}

	FORCEINLINE Vector3f Vector3f::GetClampedToSize2D(Float Min, Float Max) const
	{
		Float VecSize2D = Size2D();
		const Vector3f VecDir = (VecSize2D > SMALL_NUMBER) ? (*this / VecSize2D) : Vector3f::ZeroVector;

		VecSize2D = Math::Clamp(VecSize2D, Min, Max);

		return Vector3f(VecSize2D * VecDir.X, VecSize2D * VecDir.Y, Z);
	}

	FORCEINLINE Vector3f Vector3f::GetClampedToMaxSize(Float MaxSize) const
	{
		if (MaxSize < KINDA_SMALL_NUMBER)
		{
			return Vector3f::ZeroVector;
		}

		const Float VSq = SizeSquared();
		if (VSq > Math::Square(MaxSize))
		{
			const Float Scale = MaxSize * Math::InvSqrt(VSq);
			return Vector3f(X*Scale, Y*Scale, Z*Scale);
		}
		else
		{
			return *this;
		}
	}

	FORCEINLINE Vector3f Vector3f::GetClampedToMaxSize2D(Float MaxSize) const
	{
		if (MaxSize < KINDA_SMALL_NUMBER)
		{
			return Vector3f(0.f, 0.f, Z);
		}

		const Float VSq2D = SizeSquared2D();
		if (VSq2D > Math::Square(MaxSize))
		{
			const Float Scale = MaxSize * Math::InvSqrt(VSq2D);
			return Vector3f(X*Scale, Y*Scale, Z);
		}
		else
		{
			return *this;
		}
	}

	FORCEINLINE void Vector3f::AddBounded(const Vector3f& V, Float Radius)
	{
		*this = (*this + V).BoundToCube(Radius);
	}

	FORCEINLINE Float& Vector3f::Component(int32 Index)
	{
		return (&X)[Index];
	}

	FORCEINLINE Float Vector3f::Component(int32 Index) const
	{
		return (&X)[Index];
	}

	FORCEINLINE Vector3f Vector3f::Reciprocal() const
	{
		Vector3f RecVector;

		if (X != 0.f)
		{
			RecVector.X = 1.f / X;
		}
		else
		{
			RecVector.X = BIG_NUMBER;
		}
		if (Y != 0.f)
		{
			RecVector.Y = 1.f / Y;
		}
		else
		{
			RecVector.Y = BIG_NUMBER;
		}
		if (Z != 0.f)
		{
			RecVector.Z = 1.f / Z;
		}
		else
		{
			RecVector.Z = BIG_NUMBER;
		}

		return RecVector;
	}

	FORCEINLINE bool Vector3f::IsUniform(Float Tolerance) const
	{
		return AllComponentsEqual(Tolerance);
	}

	FORCEINLINE Vector3f Vector3f::MirrorByVector(const Vector3f& MirrorNormal) const
	{
		return *this - MirrorNormal * (2.f * (*this | MirrorNormal));
	}

	FORCEINLINE Vector3f Vector3f::GetSafeNormal(Float Tolerance) const
	{
		const Float SquareSum = X*X + Y*Y + Z*Z;

		if (SquareSum == 1.f)
		{
			return *this;
		}
		else if (SquareSum < Tolerance)
		{
			return Vector3f::ZeroVector;
		}

		const Float Scale = Math::InvSqrt(SquareSum);
		return Vector3f(X*Scale, Y*Scale, Z*Scale);
	}

	FORCEINLINE Vector3f Vector3f::GetSafeNormal2D(Float Tolerance) const
	{
		const Float SquareSum = X*X + Z*Z;

		// Not sure if it's safe to add tolerance in there. Might introduce too many errors
		if (SquareSum == 1.f)
		{
			if (Z == 0.f)
			{
				return *this;
			}
			else
			{
				return Vector3f(X, 0, Z);
			}
		}
		else if (SquareSum < Tolerance)
		{
			return Vector3f::ZeroVector;
		}

		const Float Scale = Math::InvSqrt(SquareSum);
		return Vector3f(X*Scale, 0, Z*Scale);
	}

	FORCEINLINE Float Vector3f::CosineAngle2D
	(
		Vector3f B
	)	const
	{
		Vector3f A(*this);
		A.Z = 0.0f;
		B.Z = 0.0f;
		A.Normalize();
		B.Normalize();
		return A | B;
	}

	FORCEINLINE Vector3f Vector3f::ProjectOnTo
	(
		const Vector3f& A
	)	const
	{
		return (A * ((*this | A) / (A | A)));
	}

	FORCEINLINE Vector3f Vector3f::ProjectOnToNormal
	(
		const Vector3f& Normal
	)	const
	{
		return (Normal * (*this | Normal));
	}


	FORCEINLINE bool Vector3f::ContainsNaN() const
	{
		return (
			!Math::IsFinite(X) ||
			!Math::IsFinite(Y) ||
			!Math::IsFinite(Z));
	}

	FORCEINLINE bool Vector3f::IsUnit
	(
		const Float LengthSquaredTolerance
	)	const
	{
		return Math::Abs(1.0f - SizeSquared()) < LengthSquaredTolerance;
	}

	FORCEINLINE Float Vector3f::HeadingAngle() const
	{
		Vector3f PlaneDir = *this;
		PlaneDir.Z = 0.f;
		PlaneDir = PlaneDir.GetSafeNormal();

		Float Angle = Math::Acos(PlaneDir.X);

		if (PlaneDir.Y < 0.0f)
		{
			Angle *= -1.0f;
		}

		return Angle;
	}

	FORCEINLINE Float Vector3f::Dist
	(
		const Vector3f & V1, 
		const Vector3f & V2
	)
	{
		return Math::Sqrt(Vector3f::DistSquared(V1, V2));
	}

	FORCEINLINE Float Vector3f::DistXY
	(
		const Vector3f &V1, 
		const Vector3f &V2
	)
	{
		return Math::Sqrt(Vector3f::DistSquaredXZ(V1, V2));
	}

	FORCEINLINE Float Vector3f::DistSquared
	(
		const Vector3f & V1, 
		const Vector3f & V2
	)
	{
		return 
			Math::Square(V2.X - V1.X) + 
			Math::Square(V2.Y - V1.Y) + 
			Math::Square(V2.Z - V1.Z);
	}

	FORCEINLINE Float Vector3f::DistSquaredXZ
	(
		const Vector3f & V1, 
		const Vector3f & V2
	)
	{
		return 
			Math::Square(V2.X - V1.X) + 
			Math::Square(V2.Z - V1.Z);
	}

	FORCEINLINE Float Vector3f::BoxPushOut
	(
		const Vector3f & Normal, 
		const Vector3f & Size
	)
	{
		return Math::Abs(Normal.X*Size.X) + Math::Abs(Normal.Y*Size.Y) + Math::Abs(Normal.Z*Size.Z);
	}

	FORCEINLINE Vector3f ClampVector
	(
		const Vector3f & V, 
		const Vector3f & Min, 
		const Vector3f & Max
	)
	{
		return Vector3f(
			Math::Clamp(V.X, Min.X, Max.X),
			Math::Clamp(V.Y, Min.Y, Max.Y),
			Math::Clamp(V.Z, Min.Z, Max.Z)
		);
	}

	constexpr FORCEINLINE Vector2f::Vector2f
	(
		const Vector3f & V
	)	
		: X(V.X)
		, Y(V.Y)
	{}

	inline Vector3f Vector2f::SphericalToUnitCartesian() const
	{
		const Float SinTheta = Math::Sin(X);
		return Vector3f(Math::Cos(Y) * SinTheta, Math::Sin(Y) * SinTheta, Math::Cos(X));
	}

	Vector3f Math::Frac(const Vector3f& V)
	{
		return V - Floor(V);
	}

	Vector3f Math::Floor(const Vector3f& V)
	{
		return Vector3f
		(
			Floor(V.X),
			Floor(V.Y),
			Floor(V.Z)
		);
	}
}