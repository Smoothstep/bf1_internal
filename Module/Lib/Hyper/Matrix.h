#pragma once

#include "Memory.h"
#include "Vector3.h"
#include "Plane.h"
#include "Rotation.h"

#ifdef __AVX2__
constexpr size_t MatrixAlignment = 64;
#else
constexpr size_t MatrixAlignment = 32;
#endif

namespace Hyper
{
	using namespace Memoryspace;

	struct ALIGN(MatrixAlignment) Matrix4x4 : public TCopyable<Matrix4x4>
	{
		struct Structure
		{
			Float _11, _12, _13, _14;
			Float _21, _22, _23, _24;
			Float _31, _32, _33, _34;
			Float _41, _42, _43, _44;
		};

		union
		{
			ALIGN(MatrixAlignment) Structure	Matrix;
			ALIGN(MatrixAlignment) Float		MatrixArray			[4][4];
			ALIGN(MatrixAlignment) Vector4f		MatrixVector		[4];
			ALIGN(MatrixAlignment) M128			MatrixRow			[4];
		};

		Matrix4x4() : Matrix4x4(Identity()) {};

		FORCEINLINE static Matrix4x4 Identity();
		FORCEINLINE static Matrix4x4 Zero();
		FORCEINLINE static Matrix4x4 Translation
		(
			const Vector3f & P
		);
		FORCEINLINE static Matrix4x4 Scale
		(
			const Vector3f & S
		);
		FORCEINLINE static Matrix4x4 Scale
		(
			Float Scale
		);

		FORCEINLINE Matrix4x4
		(
			IN const Float4x4 &	Matrix
		);
		FORCEINLINE Matrix4x4
		(
			IN const Structure & Matrix
		);
		FORCEINLINE Matrix4x4
		(
			IN const M128 Matrix[4]
		);
#ifdef __AVX2__
		FORCEINLINE Matrix4x4
		(
			IN const M256 Matrix[2]
		);
#endif
		FORCEINLINE Matrix4x4
		(
			IN const Vector4f & _1,
			IN const Vector4f & _2,
			IN const Vector4f & _3,
			IN const Vector4f & _4
		);
		FORCEINLINE Matrix4x4
		(
			IN const Vector4f Vector[4]
		);
		FORCEINLINE Matrix4x4
		(
			IN const Float _11, const Float _12, const Float _13, const Float _14,
			IN const Float _21, const Float _22, const Float _23, const Float _24,
			IN const Float _31, const Float _32, const Float _33, const Float _34,
			IN const Float _41, const Float _42, const Float _43, const Float _44
		);

		FORCEINLINE const Vector4f& operator[]
		(
			IN  const size_t Offset
		)		const;

		FORCEINLINE Vector4f& operator[]
		(
			IN  const size_t Offset
		);

		FORCEINLINE Matrix4x4 & operator =
		(
			IN	const Matrix4x4	& Other
		);

		FORCEINLINE Matrix4x4 operator *
		(
			IN	const Matrix4x4 & Other
		)		const;

		FORCEINLINE void operator *=
		(
			IN	const Matrix4x4	& Other
		);

		FORCEINLINE bool operator ==
		(
			IN	const Matrix4x4	& Other
		)		const;

		FORCEINLINE bool operator !=
		(
			IN	const Matrix4x4	& Other
		)		const;

		FORCEINLINE void MakeIdentity();
		FORCEINLINE void MakeZero();

		FORCEINLINE Float Determinant() const;
		
		FORCEINLINE void VectorInverse();
		FORCEINLINE void VectorInverse
		(
			OUT	Matrix4x4 & Matrix
		) const;
		FORCEINLINE Matrix4x4 GetInverse() const;

		FORCEINLINE void Transpose();
		FORCEINLINE void Transpose
		(
			OUT Matrix4x4 & Matrix
		) const;
		FORCEINLINE Matrix4x4 GetTransposed() const;

		FORCEINLINE void Translate
		(
			const Vector3f & Delta
		);

		FORCEINLINE Vector3f GetOrigin() const;
		FORCEINLINE Vector3f GetScaledAxis
		(
			const EAxis Axis
		)	const;
		FORCEINLINE void SetAxis
		(
			const EAxis		 A,
			const Vector3f & Axis
		);

		FORCEINLINE void RotateZ
		(
			const Float Angle
		);
		FORCEINLINE void RotateY
		(
			const Float Angle
		);
		FORCEINLINE void RotateX
		(
			const Float Angle
		);
		FORCEINLINE void RotateXInverse
		(
			const Float Angle
		)
		{
			return RotateY(-Angle);
		}
		FORCEINLINE void RotateYInverse
		(
			const Float Angle
		)
		{
			return RotateX(-Angle);
		}
		FORCEINLINE void RotateZInverse
		(
			const Float Angle
		)
		{
			return RotateZ(-Angle);
		}

		FORCEINLINE void Rotate
		(
			Quaternion Quaternion
		);

		FORCEINLINE void RemoveTranslation();
		FORCEINLINE void RemoveTranslation
		(
			OUT Matrix4x4 & Matrix
		);
		FORCEINLINE Matrix4x4 WithoutTranslation() const;
		FORCEINLINE Matrix4x4 WithTranslation
		(
			IN	const Vector3f & P
		)		const;
		
		FORCEINLINE Vector4f TransformVector4f
		(
			IN	const Vector4f & P
		)		const;

		FORCEINLINE Vector4f TransformVector
		(
			IN	const Vector3f & P
		)		const;

		FORCEINLINE Vector4f TransformPosition
		(
			IN	const Vector3f & P
		)		const;

		FORCEINLINE Vector3f InverseTransformPosition
		(
			IN	const Vector3f & P
		)		const;

		FORCEINLINE Vector3f ExtractScaling
		(
			IN	const Float Tolerance = SMALL_NUMBER
		);

		FORCEINLINE Vector3f ExtractTranslation() const;

		FORCEINLINE bool GetFrustumRightPlane
		(
			OUT Plane & P
		)		const;

		FORCEINLINE bool GetFrustumLeftPlane
		(
			OUT Plane & P
		)		const;

		FORCEINLINE bool GetFrustumBottomPlane
		(
			OUT Plane & P
		)		const;

		FORCEINLINE bool GetFrustumTopPlane
		(
			OUT Plane & P
		)		const;

		FORCEINLINE bool GetFrustumNearPlane
		(
			OUT Plane & P
		)		const;

		FORCEINLINE bool GetFrustumFarPlane
		(
			OUT Plane & P
		)		const;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	FORCEINLINE Matrix4x4 Matrix4x4::Identity()
	{
		constexpr Float4x4 gc_Matrix =
		{
			{ 1.0, 0.0, 0.0, 0.0 },
			{ 0.0, 1.0, 0.0, 0.0 },
			{ 0.0, 0.0, 1.0, 0.0 },
			{ 0.0, 0.0, 0.0, 1.0 }
		};

		return gc_Matrix;
	}


	FORCEINLINE Matrix4x4 Matrix4x4::Zero()
	{
		constexpr Float4x4 gc_Matrix =
		{
			{ 0.0, 0.0, 0.0, 0.0 },
			{ 0.0, 0.0, 0.0, 0.0 },
			{ 0.0, 0.0, 0.0, 0.0 },
			{ 0.0, 0.0, 0.0, 0.0 }
		};

		return gc_Matrix;
	}


	FORCEINLINE Matrix4x4 Matrix4x4::Translation
	(
		const Vector3f & P
	)
	{
		Float4x4 Matrix =
		{
			{ 1.0, 0.0, 0.0, 0.0 },
			{ 0.0, 1.0, 0.0, 0.0 },
			{ 0.0, 0.0, 1.0, 0.0 },
			{ P.X, P.Y, P.Z, 1.0 }
		};

		return Matrix;
	}


	FORCEINLINE Matrix4x4 Matrix4x4::Scale
	(
		const Vector3f & S
	)
	{
		Float4x4 Matrix =
		{
			{ S.X, 0.0, 0.0, 0.0 },
			{ 0.0, S.Y, 0.0, 0.0 },
			{ 0.0, 0.0, S.Z, 0.0 },
			{ 0.0, 0.0, 0.0, 1.0 }
		};

		return Matrix;
	}


	FORCEINLINE Matrix4x4 Matrix4x4::Scale
	(
		const Float SSS
	)
	{
		Float4x4 Matrix =
		{
			{ SSS, 0.0,	0.0, 0.0 },
			{ 0.0, SSS,	0.0, 0.0 },
			{ 0.0, 0.0,	SSS, 0.0 },
			{ 0.0, 0.0,	0.0, 1.0 }
		};

		return Matrix;
	}


	Matrix4x4::Matrix4x4
	(
		IN const Float4x4 & Matrix
	)
	{
#ifdef __AVX2__
		Matrix4X2Store(Matrix[0], &MatrixRow[0]);
		Matrix4X2Store(Matrix[2], &MatrixRow[2]);
#else
		VectorStoreAligned(VectorLoadAligned(Matrix[0]), &MatrixRow[0]);
		VectorStoreAligned(VectorLoadAligned(Matrix[1]), &MatrixRow[1]);
		VectorStoreAligned(VectorLoadAligned(Matrix[2]), &MatrixRow[2]);
		VectorStoreAligned(VectorLoadAligned(Matrix[3]), &MatrixRow[3]);
#endif
	}


	Matrix4x4::Matrix4x4
	(
		IN const Structure & Matrix
	)
	{
#ifdef __AVX2__
		Matrix4X2Store(&Matrix._11, &MatrixRow[0]);
		Matrix4X2Store(&Matrix._31, &MatrixRow[2]);
#else
		VectorStoreAligned(VectorLoadAligned(&Matrix._11), &MatrixRow[0]);
		VectorStoreAligned(VectorLoadAligned(&Matrix._21), &MatrixRow[1]);
		VectorStoreAligned(VectorLoadAligned(&Matrix._31), &MatrixRow[2]);
		VectorStoreAligned(VectorLoadAligned(&Matrix._41), &MatrixRow[3]);
#endif
	}


	Matrix4x4::Matrix4x4
	(
		IN const M128 Matrix[4]
	)
	{
#ifdef __AVX2__
		Matrix4X2Store(Matrix[0], &MatrixRow[0]);
		Matrix4X2Store(Matrix[2], &MatrixRow[2]);
#else
		VectorStoreAligned(Matrix[0], &MatrixRow[0]);
		VectorStoreAligned(Matrix[1], &MatrixRow[1]);
		VectorStoreAligned(Matrix[2], &MatrixRow[2]);
		VectorStoreAligned(Matrix[3], &MatrixRow[3]);
#endif
	}


#ifdef __AVX2__
	Matrix4x4::Matrix4x4
	(
		IN const M256 Matrix[2]
	)
	{
		Matrix4X2Store(Matrix[0], &MatrixRow[0]);
		Matrix4X2Store(Matrix[1], &MatrixRow[2]);
	}
#endif


	Matrix4x4::Matrix4x4
	(
		IN const Vector4f & _1,
		IN const Vector4f & _2,
		IN const Vector4f & _3,
		IN const Vector4f & _4
	)
	{
		VectorStoreAligned(VectorLoad(&_1), &MatrixRow[0]);
		VectorStoreAligned(VectorLoad(&_2), &MatrixRow[1]);
		VectorStoreAligned(VectorLoad(&_3), &MatrixRow[2]);
		VectorStoreAligned(VectorLoad(&_4), &MatrixRow[3]);
	}


	Matrix4x4::Matrix4x4
	(
		IN const Vector4f V[4]
	)
	{
#ifdef __AVX2__
		Matrix4X2Store(V[0], &MatrixRow[0]);
		Matrix4X2Store(V[2], &MatrixRow[2]);
#else
		VectorStoreAligned(VectorLoad(&V[0].X), &MatrixRow[0]);
		VectorStoreAligned(VectorLoad(&V[1].X), &MatrixRow[1]);
		VectorStoreAligned(VectorLoad(&V[2].X), &MatrixRow[2]);
		VectorStoreAligned(VectorLoad(&V[3].X), &MatrixRow[3]);
#endif
	}


	FORCEINLINE Matrix4x4::Matrix4x4
	(
		IN const Float _11, const Float _12, const Float _13, const Float _14,
		IN const Float _21, const Float _22, const Float _23, const Float _24,
		IN const Float _31, const Float _32, const Float _33, const Float _34,
		IN const Float _41, const Float _42, const Float _43, const Float _44
	)
	{
		Matrix._11 = _11;
		Matrix._12 = _12;
		Matrix._13 = _13;
		Matrix._14 = _14;
		Matrix._21 = _21;
		Matrix._22 = _22;
		Matrix._23 = _23;
		Matrix._24 = _24;
		Matrix._31 = _31;
		Matrix._32 = _32;
		Matrix._33 = _33;
		Matrix._34 = _34;
		Matrix._41 = _41;
		Matrix._42 = _42;
		Matrix._43 = _43;
		Matrix._44 = _44;
	}


	FORCEINLINE const Vector4f& Matrix4x4::operator[]
	(
		IN	const size_t Offset
	)		const
	{
		return MatrixVector[Offset];
	}


	FORCEINLINE Vector4f& Matrix4x4::operator[]
	(
		IN	const size_t Offset
	)
	{
		return MatrixVector[Offset];
	}


	FORCEINLINE Matrix4x4 & Matrix4x4::operator =
	(
		IN const Matrix4x4 & Other
	)
	{
#ifdef __AVX2__
		Matrix4X2Store(Other.MatrixRow[0], &MatrixRow[0]);
		Matrix4X2Store(Other.MatrixRow[2], &MatrixRow[2]);
#else
		VectorStoreAligned(Other.MatrixRow[0], &MatrixRow[0]);
		VectorStoreAligned(Other.MatrixRow[1], &MatrixRow[1]);
		VectorStoreAligned(Other.MatrixRow[2], &MatrixRow[2]);
		VectorStoreAligned(Other.MatrixRow[3], &MatrixRow[3]);
#endif

		return *this;
	}


	FORCEINLINE Matrix4x4 Matrix4x4::operator *
	(
		IN	const Matrix4x4 & Other
	)		const
	{
#ifdef __AVX2__
		Matrix4X2ResetRegisters();

		M256 A01 = Matrix4X2Load(&MatrixRow[0]);
		M256 A23 = Matrix4X2Load(&MatrixRow[2]);

		M256 Rows[2] = { Math::TwoLincomb_AVX_8(Other.MatrixRow, MatrixRow[0]), Math::TwoLincomb_AVX_8(Other.MatrixRow, MatrixRow[2]) };

		return { Rows };
#else
		M128 Rows[4] =
		{
			Math::Lincomb_SSE(Other.MatrixRow, MatrixRow[0]),
			Math::Lincomb_SSE(Other.MatrixRow, MatrixRow[1]),
			Math::Lincomb_SSE(Other.MatrixRow, MatrixRow[2]),
			Math::Lincomb_SSE(Other.MatrixRow, MatrixRow[3])
		};

		return { Rows };
#endif
	}


	FORCEINLINE void Matrix4x4::operator *=
	(
		IN const Matrix4x4 & Other
	)
	{
		*this = (*this) * Other;
	}


	FORCEINLINE bool Matrix4x4::operator ==
	(
		IN	const Matrix4x4 & Other
	)		const
	{
#ifdef __AVX2__
		M256 R0 = Matrix4X2Compare(MatrixRow[0], Other.MatrixRow[0], _CMP_EQ_OQ);
		if (Matrix4X2MoveMask(R0) > 0)
		{
			return false;
		}

		M256 R1 = Matrix4X2Compare(MatrixRow[2], Other.MatrixRow[2], _CMP_EQ_OQ);
		if (Matrix4X2MoveMask(R1) > 0)
		{
			return false;
		}
#else
		M128 R0 = VectorCompareEQ(MatrixRow[0], Other.MatrixRow[0]);
		if (VectorMoveMask(R0) > 0)
		{
			return false;
		}
		M128 R1 = VectorCompareEQ(MatrixRow[1], Other.MatrixRow[1]);
		if (VectorMoveMask(R1) > 0)
		{
			return false;
		}
		M128 R2 = VectorCompareEQ(MatrixRow[2], Other.MatrixRow[2]);
		if (VectorMoveMask(R2) > 0)
		{
			return false;
		}
		M128 R3 = VectorCompareEQ(MatrixRow[3], Other.MatrixRow[3]);
		if (VectorMoveMask(R3) > 0)
		{
			return false;
		}
#endif
		return true;
	}


	FORCEINLINE bool Matrix4x4::operator !=
	(
		IN	const Matrix4x4 & Other
	)		const
	{
		return !(*this == Other);
	}


	FORCEINLINE void Matrix4x4::MakeIdentity()
	{
		*this = Identity();
	}


	FORCEINLINE void Matrix4x4::MakeZero()
	{
		*this = Zero();
	}


	FORCEINLINE Float Matrix4x4::Determinant() const
	{
		return	MatrixArray[0][0] * (
			MatrixArray[1][1] * (MatrixArray[2][2] * MatrixArray[3][3] - MatrixArray[2][3] * MatrixArray[3][2]) -
			MatrixArray[2][1] * (MatrixArray[1][2] * MatrixArray[3][3] - MatrixArray[1][3] * MatrixArray[3][2]) +
			MatrixArray[3][1] * (MatrixArray[1][2] * MatrixArray[2][3] - MatrixArray[1][3] * MatrixArray[2][2])
			) -
			MatrixArray[1][0] * (
				MatrixArray[0][1] * (MatrixArray[2][2] * MatrixArray[3][3] - MatrixArray[2][3] * MatrixArray[3][2]) -
				MatrixArray[2][1] * (MatrixArray[0][2] * MatrixArray[3][3] - MatrixArray[0][3] * MatrixArray[3][2]) +
				MatrixArray[3][1] * (MatrixArray[0][2] * MatrixArray[2][3] - MatrixArray[0][3] * MatrixArray[2][2])
				) +
			MatrixArray[2][0] * (
				MatrixArray[0][1] * (MatrixArray[1][2] * MatrixArray[3][3] - MatrixArray[1][3] * MatrixArray[3][2]) -
				MatrixArray[1][1] * (MatrixArray[0][2] * MatrixArray[3][3] - MatrixArray[0][3] * MatrixArray[3][2]) +
				MatrixArray[3][1] * (MatrixArray[0][2] * MatrixArray[1][3] - MatrixArray[0][3] * MatrixArray[1][2])
				) -
			MatrixArray[3][0] * (
				MatrixArray[0][1] * (MatrixArray[1][2] * MatrixArray[2][3] - MatrixArray[1][3] * MatrixArray[2][2]) -
				MatrixArray[1][1] * (MatrixArray[0][2] * MatrixArray[2][3] - MatrixArray[0][3] * MatrixArray[2][2]) +
				MatrixArray[2][1] * (MatrixArray[0][2] * MatrixArray[1][3] - MatrixArray[0][3] * MatrixArray[1][2])
				);
	}


	FORCEINLINE Matrix4x4 Matrix4x4::GetInverse() const
	{
		Float4x4	Result;
		Float4x4	Tmp;
		Float4		Det;

		Tmp[0][0] = MatrixArray[2][2] * MatrixArray[3][3] - MatrixArray[2][3] * MatrixArray[3][2];
		Tmp[0][1] = MatrixArray[1][2] * MatrixArray[3][3] - MatrixArray[1][3] * MatrixArray[3][2];
		Tmp[0][2] = MatrixArray[1][2] * MatrixArray[2][3] - MatrixArray[1][3] * MatrixArray[2][2];

		Tmp[1][0] = MatrixArray[2][2] * MatrixArray[3][3] - MatrixArray[2][3] * MatrixArray[3][2];
		Tmp[1][1] = MatrixArray[0][2] * MatrixArray[3][3] - MatrixArray[0][3] * MatrixArray[3][2];
		Tmp[1][2] = MatrixArray[0][2] * MatrixArray[2][3] - MatrixArray[0][3] * MatrixArray[2][2];

		Tmp[2][0] = MatrixArray[1][2] * MatrixArray[3][3] - MatrixArray[1][3] * MatrixArray[3][2];
		Tmp[2][1] = MatrixArray[0][2] * MatrixArray[3][3] - MatrixArray[0][3] * MatrixArray[3][2];
		Tmp[2][2] = MatrixArray[0][2] * MatrixArray[1][3] - MatrixArray[0][3] * MatrixArray[1][2];

		Tmp[3][0] = MatrixArray[1][2] * MatrixArray[2][3] - MatrixArray[1][3] * MatrixArray[2][2];
		Tmp[3][1] = MatrixArray[0][2] * MatrixArray[2][3] - MatrixArray[0][3] * MatrixArray[2][2];
		Tmp[3][2] = MatrixArray[0][2] * MatrixArray[1][3] - MatrixArray[0][3] * MatrixArray[1][2];

		Det[0] = MatrixArray[1][1] * Tmp[0][0] - MatrixArray[2][1] * Tmp[0][1] + MatrixArray[3][1] * Tmp[0][2];
		Det[1] = MatrixArray[0][1] * Tmp[1][0] - MatrixArray[2][1] * Tmp[1][1] + MatrixArray[3][1] * Tmp[1][2];
		Det[2] = MatrixArray[0][1] * Tmp[2][0] - MatrixArray[1][1] * Tmp[2][1] + MatrixArray[3][1] * Tmp[2][2];
		Det[3] = MatrixArray[0][1] * Tmp[3][0] - MatrixArray[1][1] * Tmp[3][1] + MatrixArray[2][1] * Tmp[3][2];

		const Float Determinant = MatrixArray[0][0] * Det[0] - MatrixArray[1][0] * Det[1] + MatrixArray[2][0] * Det[2] - MatrixArray[3][0] * Det[3];
		const Float	RDet = 1.0f / Determinant;

		Result[0][0] = +RDet * Det[0];
		Result[0][1] = -RDet * Det[1];
		Result[0][2] = +RDet * Det[2];
		Result[0][3] = -RDet * Det[3];
		Result[1][0] = -RDet * (MatrixArray[1][0] * Tmp[0][0] - MatrixArray[2][0] * Tmp[0][1] + MatrixArray[3][0] * Tmp[0][2]);
		Result[1][1] = +RDet * (MatrixArray[0][0] * Tmp[1][0] - MatrixArray[2][0] * Tmp[1][1] + MatrixArray[3][0] * Tmp[1][2]);
		Result[1][2] = -RDet * (MatrixArray[0][0] * Tmp[2][0] - MatrixArray[1][0] * Tmp[2][1] + MatrixArray[3][0] * Tmp[2][2]);
		Result[1][3] = +RDet * (MatrixArray[0][0] * Tmp[3][0] - MatrixArray[1][0] * Tmp[3][1] + MatrixArray[2][0] * Tmp[3][2]);
		Result[2][0] = +RDet * (
			MatrixArray[1][0] * (MatrixArray[2][1] * MatrixArray[3][3] - MatrixArray[2][3] * MatrixArray[3][1]) -
			MatrixArray[2][0] * (MatrixArray[1][1] * MatrixArray[3][3] - MatrixArray[1][3] * MatrixArray[3][1]) +
			MatrixArray[3][0] * (MatrixArray[1][1] * MatrixArray[2][3] - MatrixArray[1][3] * MatrixArray[2][1])
			);
		Result[2][1] = -RDet * (
			MatrixArray[0][0] * (MatrixArray[2][1] * MatrixArray[3][3] - MatrixArray[2][3] * MatrixArray[3][1]) -
			MatrixArray[2][0] * (MatrixArray[0][1] * MatrixArray[3][3] - MatrixArray[0][3] * MatrixArray[3][1]) +
			MatrixArray[3][0] * (MatrixArray[0][1] * MatrixArray[2][3] - MatrixArray[0][3] * MatrixArray[2][1])
			);
		Result[2][2] = RDet * (
			MatrixArray[0][0] * (MatrixArray[1][1] * MatrixArray[3][3] - MatrixArray[1][3] * MatrixArray[3][1]) -
			MatrixArray[1][0] * (MatrixArray[0][1] * MatrixArray[3][3] - MatrixArray[0][3] * MatrixArray[3][1]) +
			MatrixArray[3][0] * (MatrixArray[0][1] * MatrixArray[1][3] - MatrixArray[0][3] * MatrixArray[1][1])
			);
		Result[2][3] = -RDet * (
			MatrixArray[0][0] * (MatrixArray[1][1] * MatrixArray[2][3] - MatrixArray[1][3] * MatrixArray[2][1]) -
			MatrixArray[1][0] * (MatrixArray[0][1] * MatrixArray[2][3] - MatrixArray[0][3] * MatrixArray[2][1]) +
			MatrixArray[2][0] * (MatrixArray[0][1] * MatrixArray[1][3] - MatrixArray[0][3] * MatrixArray[1][1])
			);
		Result[3][0] = -RDet * (
			MatrixArray[1][0] * (MatrixArray[2][1] * MatrixArray[3][2] - MatrixArray[2][2] * MatrixArray[3][1]) -
			MatrixArray[2][0] * (MatrixArray[1][1] * MatrixArray[3][2] - MatrixArray[1][2] * MatrixArray[3][1]) +
			MatrixArray[3][0] * (MatrixArray[1][1] * MatrixArray[2][2] - MatrixArray[1][2] * MatrixArray[2][1])
			);
		Result[3][1] = RDet * (
			MatrixArray[0][0] * (MatrixArray[2][1] * MatrixArray[3][2] - MatrixArray[2][2] * MatrixArray[3][1]) -
			MatrixArray[2][0] * (MatrixArray[0][1] * MatrixArray[3][2] - MatrixArray[0][2] * MatrixArray[3][1]) +
			MatrixArray[3][0] * (MatrixArray[0][1] * MatrixArray[2][2] - MatrixArray[0][2] * MatrixArray[2][1])
			);
		Result[3][2] = -RDet * (
			MatrixArray[0][0] * (MatrixArray[1][1] * MatrixArray[3][2] - MatrixArray[1][2] * MatrixArray[3][1]) -
			MatrixArray[1][0] * (MatrixArray[0][1] * MatrixArray[3][2] - MatrixArray[0][2] * MatrixArray[3][1]) +
			MatrixArray[3][0] * (MatrixArray[0][1] * MatrixArray[1][2] - MatrixArray[0][2] * MatrixArray[1][1])
			);
		Result[3][3] = RDet * (
			MatrixArray[0][0] * (MatrixArray[1][1] * MatrixArray[2][2] - MatrixArray[1][2] * MatrixArray[2][1]) -
			MatrixArray[1][0] * (MatrixArray[0][1] * MatrixArray[2][2] - MatrixArray[0][2] * MatrixArray[2][1]) +
			MatrixArray[2][0] * (MatrixArray[0][1] * MatrixArray[1][2] - MatrixArray[0][2] * MatrixArray[1][1])
			);

		return Result;
	}


	FORCEINLINE void Matrix4x4::VectorInverse
	(
		OUT Matrix4x4 & Matrix
	) const
	{
		Float4x4&	Result = Matrix.MatrixArray;
		Float4x4	Tmp;
		Float4		Det;

		Tmp[0][0] = MatrixArray[2][2] * MatrixArray[3][3] - MatrixArray[2][3] * MatrixArray[3][2];
		Tmp[0][1] = MatrixArray[1][2] * MatrixArray[3][3] - MatrixArray[1][3] * MatrixArray[3][2];
		Tmp[0][2] = MatrixArray[1][2] * MatrixArray[2][3] - MatrixArray[1][3] * MatrixArray[2][2];

		Tmp[1][0] = MatrixArray[2][2] * MatrixArray[3][3] - MatrixArray[2][3] * MatrixArray[3][2];
		Tmp[1][1] = MatrixArray[0][2] * MatrixArray[3][3] - MatrixArray[0][3] * MatrixArray[3][2];
		Tmp[1][2] = MatrixArray[0][2] * MatrixArray[2][3] - MatrixArray[0][3] * MatrixArray[2][2];

		Tmp[2][0] = MatrixArray[1][2] * MatrixArray[3][3] - MatrixArray[1][3] * MatrixArray[3][2];
		Tmp[2][1] = MatrixArray[0][2] * MatrixArray[3][3] - MatrixArray[0][3] * MatrixArray[3][2];
		Tmp[2][2] = MatrixArray[0][2] * MatrixArray[1][3] - MatrixArray[0][3] * MatrixArray[1][2];

		Tmp[3][0] = MatrixArray[1][2] * MatrixArray[2][3] - MatrixArray[1][3] * MatrixArray[2][2];
		Tmp[3][1] = MatrixArray[0][2] * MatrixArray[2][3] - MatrixArray[0][3] * MatrixArray[2][2];
		Tmp[3][2] = MatrixArray[0][2] * MatrixArray[1][3] - MatrixArray[0][3] * MatrixArray[1][2];

		Det[0] = MatrixArray[1][1] * Tmp[0][0] - MatrixArray[2][1] * Tmp[0][1] + MatrixArray[3][1] * Tmp[0][2];
		Det[1] = MatrixArray[0][1] * Tmp[1][0] - MatrixArray[2][1] * Tmp[1][1] + MatrixArray[3][1] * Tmp[1][2];
		Det[2] = MatrixArray[0][1] * Tmp[2][0] - MatrixArray[1][1] * Tmp[2][1] + MatrixArray[3][1] * Tmp[2][2];
		Det[3] = MatrixArray[0][1] * Tmp[3][0] - MatrixArray[1][1] * Tmp[3][1] + MatrixArray[2][1] * Tmp[3][2];

		const Float Determinant = MatrixArray[0][0] * Det[0] - MatrixArray[1][0] * Det[1] + MatrixArray[2][0] * Det[2] - MatrixArray[3][0] * Det[3];
		const Float	RDet = 1.0f / Determinant;

		Result[0][0] = +RDet * Det[0];
		Result[0][1] = -RDet * Det[1];
		Result[0][2] = +RDet * Det[2];
		Result[0][3] = -RDet * Det[3];
		Result[1][0] = -RDet * (MatrixArray[1][0] * Tmp[0][0] - MatrixArray[2][0] * Tmp[0][1] + MatrixArray[3][0] * Tmp[0][2]);
		Result[1][1] = +RDet * (MatrixArray[0][0] * Tmp[1][0] - MatrixArray[2][0] * Tmp[1][1] + MatrixArray[3][0] * Tmp[1][2]);
		Result[1][2] = -RDet * (MatrixArray[0][0] * Tmp[2][0] - MatrixArray[1][0] * Tmp[2][1] + MatrixArray[3][0] * Tmp[2][2]);
		Result[1][3] = +RDet * (MatrixArray[0][0] * Tmp[3][0] - MatrixArray[1][0] * Tmp[3][1] + MatrixArray[2][0] * Tmp[3][2]);
		Result[2][0] = +RDet * (
			MatrixArray[1][0] * (MatrixArray[2][1] * MatrixArray[3][3] - MatrixArray[2][3] * MatrixArray[3][1]) -
			MatrixArray[2][0] * (MatrixArray[1][1] * MatrixArray[3][3] - MatrixArray[1][3] * MatrixArray[3][1]) +
			MatrixArray[3][0] * (MatrixArray[1][1] * MatrixArray[2][3] - MatrixArray[1][3] * MatrixArray[2][1])
			);
		Result[2][1] = -RDet * (
			MatrixArray[0][0] * (MatrixArray[2][1] * MatrixArray[3][3] - MatrixArray[2][3] * MatrixArray[3][1]) -
			MatrixArray[2][0] * (MatrixArray[0][1] * MatrixArray[3][3] - MatrixArray[0][3] * MatrixArray[3][1]) +
			MatrixArray[3][0] * (MatrixArray[0][1] * MatrixArray[2][3] - MatrixArray[0][3] * MatrixArray[2][1])
			);
		Result[2][2] = RDet * (
			MatrixArray[0][0] * (MatrixArray[1][1] * MatrixArray[3][3] - MatrixArray[1][3] * MatrixArray[3][1]) -
			MatrixArray[1][0] * (MatrixArray[0][1] * MatrixArray[3][3] - MatrixArray[0][3] * MatrixArray[3][1]) +
			MatrixArray[3][0] * (MatrixArray[0][1] * MatrixArray[1][3] - MatrixArray[0][3] * MatrixArray[1][1])
			);
		Result[2][3] = -RDet * (
			MatrixArray[0][0] * (MatrixArray[1][1] * MatrixArray[2][3] - MatrixArray[1][3] * MatrixArray[2][1]) -
			MatrixArray[1][0] * (MatrixArray[0][1] * MatrixArray[2][3] - MatrixArray[0][3] * MatrixArray[2][1]) +
			MatrixArray[2][0] * (MatrixArray[0][1] * MatrixArray[1][3] - MatrixArray[0][3] * MatrixArray[1][1])
			);
		Result[3][0] = -RDet * (
			MatrixArray[1][0] * (MatrixArray[2][1] * MatrixArray[3][2] - MatrixArray[2][2] * MatrixArray[3][1]) -
			MatrixArray[2][0] * (MatrixArray[1][1] * MatrixArray[3][2] - MatrixArray[1][2] * MatrixArray[3][1]) +
			MatrixArray[3][0] * (MatrixArray[1][1] * MatrixArray[2][2] - MatrixArray[1][2] * MatrixArray[2][1])
			);
		Result[3][1] = RDet * (
			MatrixArray[0][0] * (MatrixArray[2][1] * MatrixArray[3][2] - MatrixArray[2][2] * MatrixArray[3][1]) -
			MatrixArray[2][0] * (MatrixArray[0][1] * MatrixArray[3][2] - MatrixArray[0][2] * MatrixArray[3][1]) +
			MatrixArray[3][0] * (MatrixArray[0][1] * MatrixArray[2][2] - MatrixArray[0][2] * MatrixArray[2][1])
			);
		Result[3][2] = -RDet * (
			MatrixArray[0][0] * (MatrixArray[1][1] * MatrixArray[3][2] - MatrixArray[1][2] * MatrixArray[3][1]) -
			MatrixArray[1][0] * (MatrixArray[0][1] * MatrixArray[3][2] - MatrixArray[0][2] * MatrixArray[3][1]) +
			MatrixArray[3][0] * (MatrixArray[0][1] * MatrixArray[1][2] - MatrixArray[0][2] * MatrixArray[1][1])
			);
		Result[3][3] = RDet * (
			MatrixArray[0][0] * (MatrixArray[1][1] * MatrixArray[2][2] - MatrixArray[1][2] * MatrixArray[2][1]) -
			MatrixArray[1][0] * (MatrixArray[0][1] * MatrixArray[2][2] - MatrixArray[0][2] * MatrixArray[2][1]) +
			MatrixArray[2][0] * (MatrixArray[0][1] * MatrixArray[1][2] - MatrixArray[0][2] * MatrixArray[1][1])
			);
	}


	FORCEINLINE void Matrix4x4::VectorInverse()
	{
		Float Det = Determinant();

		if (Det < KINDA_SMALL_NUMBER)
		{
			*this = Identity();
		}
		else
		{
			*this = GetInverse();
		}
	}


	FORCEINLINE void Matrix4x4::Transpose()
	{
		Matrix4x4 Copy(*this);

		MatrixArray[0][0] = Copy.MatrixArray[0][0];
		MatrixArray[0][1] = Copy.MatrixArray[1][0];
		MatrixArray[0][2] = Copy.MatrixArray[2][0];
		MatrixArray[0][3] = Copy.MatrixArray[3][0];

		MatrixArray[1][0] = Copy.MatrixArray[0][1];
		MatrixArray[1][1] = Copy.MatrixArray[1][1];
		MatrixArray[1][2] = Copy.MatrixArray[2][1];
		MatrixArray[1][3] = Copy.MatrixArray[3][1];

		MatrixArray[2][0] = Copy.MatrixArray[0][2];
		MatrixArray[2][1] = Copy.MatrixArray[1][2];
		MatrixArray[2][2] = Copy.MatrixArray[2][2];
		MatrixArray[2][3] = Copy.MatrixArray[3][2];

		MatrixArray[3][0] = Copy.MatrixArray[0][3];
		MatrixArray[3][1] = Copy.MatrixArray[1][3];
		MatrixArray[3][2] = Copy.MatrixArray[2][3];
		MatrixArray[3][3] = Copy.MatrixArray[3][3];
	}


	FORCEINLINE void Matrix4x4::Transpose
	(
		OUT Matrix4x4 & Matrix
	) const
	{
		Matrix.MatrixArray[0][0] = MatrixArray[0][0];
		Matrix.MatrixArray[0][1] = MatrixArray[1][0];
		Matrix.MatrixArray[0][2] = MatrixArray[2][0];
		Matrix.MatrixArray[0][3] = MatrixArray[3][0];

		Matrix.MatrixArray[1][0] = MatrixArray[0][1];
		Matrix.MatrixArray[1][1] = MatrixArray[1][1];
		Matrix.MatrixArray[1][2] = MatrixArray[2][1];
		Matrix.MatrixArray[1][3] = MatrixArray[3][1];

		Matrix.MatrixArray[2][0] = MatrixArray[0][2];
		Matrix.MatrixArray[2][1] = MatrixArray[1][2];
		Matrix.MatrixArray[2][2] = MatrixArray[2][2];
		Matrix.MatrixArray[2][3] = MatrixArray[3][2];

		Matrix.MatrixArray[3][0] = MatrixArray[0][3];
		Matrix.MatrixArray[3][1] = MatrixArray[1][3];
		Matrix.MatrixArray[3][2] = MatrixArray[2][3];
		Matrix.MatrixArray[3][3] = MatrixArray[3][3];
	}


	FORCEINLINE Matrix4x4 Matrix4x4::GetTransposed() const
	{
		Matrix4x4 Result;

		Result.MatrixArray[0][0] = MatrixArray[0][0];
		Result.MatrixArray[0][1] = MatrixArray[1][0];
		Result.MatrixArray[0][2] = MatrixArray[2][0];
		Result.MatrixArray[0][3] = MatrixArray[3][0];

		Result.MatrixArray[1][0] = MatrixArray[0][1];
		Result.MatrixArray[1][1] = MatrixArray[1][1];
		Result.MatrixArray[1][2] = MatrixArray[2][1];
		Result.MatrixArray[1][3] = MatrixArray[3][1];

		Result.MatrixArray[2][0] = MatrixArray[0][2];
		Result.MatrixArray[2][1] = MatrixArray[1][2];
		Result.MatrixArray[2][2] = MatrixArray[2][2];
		Result.MatrixArray[2][3] = MatrixArray[3][2];

		Result.MatrixArray[3][0] = MatrixArray[0][3];
		Result.MatrixArray[3][1] = MatrixArray[1][3];
		Result.MatrixArray[3][2] = MatrixArray[2][3];
		Result.MatrixArray[3][3] = MatrixArray[3][3];

		return Result;
	}


	FORCEINLINE void Matrix4x4::Translate
	(
		const Vector3f & Delta
	)
	{
		MatrixVector[3] = Vector4f
		(
			Delta.X, 
			Delta.Y, 
			Delta.Z, 
			1.0f
		);
	}


	FORCEINLINE Vector4f Matrix4x4::TransformVector4f
	(
		IN	const Vector4f & P
	)		const
	{
		Vector4f Result;
		{
			M128 VecP = VectorLoadAligned(&P);
			M128 VecR = Math::VectorTransformVector(VecP, Matrix);

			VectorStoreAligned(VecR, &Result);
		}

		return Result;
	}


	FORCEINLINE Vector4f Matrix4x4::TransformVector
	(
		IN	const Vector3f & P
	)		const
	{
		return TransformVector4f(Vector4f
		(
			P.X, 
			P.Y, 
			P.Z, 
			0.0
		));
	}


	FORCEINLINE Vector3f Matrix4x4::GetOrigin() const
	{
		return Vector3f(MatrixVector[3]);
	}


	FORCEINLINE Vector3f Matrix4x4::GetScaledAxis
	(
		const EAxis Axis
	)	const
	{
		switch (Axis)
		{
		case EAxis::X:
			return Vector3f(MatrixVector[0]);

		case EAxis::Y:
			return Vector3f(MatrixVector[1]);

		case EAxis::Z:
			return Vector3f(MatrixVector[2]);

		default:
			return Vector3f::ZeroVector;
		}
	}


	FORCEINLINE Matrix4x4 Matrix4x4::WithoutTranslation() const
	{
		Matrix4x4 Result(*this);

		Result.MatrixArray[3][0] = 0.0f;
		Result.MatrixArray[3][1] = 0.0f;
		Result.MatrixArray[3][2] = 0.0f;

		return Result;
	}

	FORCEINLINE Matrix4x4 Matrix4x4::WithTranslation
	(
		IN	const Vector3f & P
	)		const
	{
		Matrix4x4 Result(*this);

		Result.MatrixArray[3][0] = P.X;
		Result.MatrixArray[3][1] = P.Y;
		Result.MatrixArray[3][2] = P.Z;

		return Result;
	}


	FORCEINLINE void Matrix4x4::RotateZ
	(
		const Float Angle
	)
	{
		Matrix4x4 RotationMatrix
		(
			+Math::Cos(Angle), -Math::Sin(Angle),	0.0,	0.0,
			+Math::Sin(Angle), +Math::Cos(Angle),	0.0,	0.0,
			0.0,				0.0,				1.0,	0.0,
			0.0,				0.0,				0.0,	1.0
		);

		*this *= RotationMatrix;
	}


	FORCEINLINE void Matrix4x4::RotateY
	(
		const Float Angle
	)
	{
		Matrix4x4 RotationMatrix
		(
			+Math::Cos(Angle),	0.0, +Math::Sin(Angle),		0.0,
			0.0,				1.0, 0.0,					0.0,
			-Math::Sin(Angle),	0.0, +Math::Cos(Angle),		0.0,
			0.0,				0.0, 0.0,					1.0
		);

		*this *= RotationMatrix;
	}


	FORCEINLINE void Matrix4x4::RotateX
	(
		const Float Angle
	)
	{
		Matrix4x4 RotationMatrix
		(
			1.0,	0.0,				0.0,				0.0,
			0.0,	+Math::Cos(Angle),	-Math::Sin(Angle),	0.0,
			0.0,	+Math::Sin(Angle),	+Math::Cos(Angle),	0.0,
			0.0,	0.0,				0.0,				1.0
		);

		*this *= RotationMatrix;
	}


	FORCEINLINE Vector3f Matrix4x4::ExtractScaling
	(
		const Float Tolerance
	)
	{
		Vector3f Scale3D = Vector3f::ZeroVector;

		const float SquareSum0 = (MatrixArray[0][0] * MatrixArray[0][0]) + (MatrixArray[0][1] * MatrixArray[0][1]) + (MatrixArray[0][2] * MatrixArray[0][2]);
		const float SquareSum1 = (MatrixArray[1][0] * MatrixArray[1][0]) + (MatrixArray[1][1] * MatrixArray[1][1]) + (MatrixArray[1][2] * MatrixArray[1][2]);
		const float SquareSum2 = (MatrixArray[2][0] * MatrixArray[2][0]) + (MatrixArray[2][1] * MatrixArray[2][1]) + (MatrixArray[2][2] * MatrixArray[2][2]);

		if (SquareSum0 > Tolerance)
		{
			float Scale0 = Math::Sqrt(SquareSum0);
			Scale3D[0] = Scale0;
			float InvScale0 = 1.f / Scale0;

			MatrixArray[0][0] *= InvScale0;
			MatrixArray[0][1] *= InvScale0;
			MatrixArray[0][2] *= InvScale0;
		}
		else
		{
			Scale3D[0] = 0;
		}

		if (SquareSum1 > Tolerance)
		{
			float Scale1 = Math::Sqrt(SquareSum1);
			Scale3D[1] = Scale1;
			float InvScale1 = 1.f / Scale1;

			MatrixArray[1][0] *= InvScale1;
			MatrixArray[1][1] *= InvScale1;
			MatrixArray[1][2] *= InvScale1;
		}
		else
		{
			Scale3D[1] = 0;
		}

		if (SquareSum2 > Tolerance)
		{
			float Scale2 = Math::Sqrt(SquareSum2);
			Scale3D[2] = Scale2;
			float InvScale2 = 1.f / Scale2;

			MatrixArray[2][0] *= InvScale2;
			MatrixArray[2][1] *= InvScale2;
			MatrixArray[2][2] *= InvScale2;
		}
		else
		{
			Scale3D[2] = 0;
		}

		return Scale3D;
	}

	FORCEINLINE Vector3f Matrix4x4::ExtractTranslation() const
	{
		return Vector3f{ MatrixVector[3] };
	}

	FORCEINLINE void Matrix4x4::RemoveTranslation()
	{
		MatrixArray[3][0] = 0.0f;
		MatrixArray[3][1] = 0.0f;
		MatrixArray[3][2] = 0.0f;
	}


	FORCEINLINE Vector4f Matrix4x4::TransformPosition
	(
		IN	const Vector3f & P
	)		const
	{
		return TransformVector4f(Vector4f
		(
			P.X, 
			P.Y, 
			P.Z, 
			1.0)
		);
	}


	FORCEINLINE void Matrix4x4::SetAxis
	(
		const EAxis		 A,
		const Vector3f & Axis
	)
	{
		MatrixArray[A][0] = Axis.X;
		MatrixArray[A][1] = Axis.Y;
		MatrixArray[A][2] = Axis.Z;
	}


	FORCEINLINE Vector3f Matrix4x4::InverseTransformPosition
	(
		IN	const Vector3f & P
	)		const
	{
		Matrix4x4 Inverse = GetInverse();
		return Inverse.TransformPosition(P);
	}


	FORCEINLINE bool MakeFrustumPlane
	(
		const	float	A, 
		const	float	B,
		const	float	C,
		const	float	D,
				Plane & OutPlane
	)
	{
		const float	LengthSquared = A * A + B * B + C * C;

		if (LengthSquared > DELTA * DELTA)
		{
			const float	InvLength = Math::InvSqrt(LengthSquared);

			OutPlane = Plane
			(
				-A * InvLength, 
				-B * InvLength,
				-C * InvLength, 
				 D * InvLength
			);

			return true;
		}
		
		return false;
	}


	FORCEINLINE bool Matrix4x4::GetFrustumRightPlane
	(
		OUT Plane & P
	)	const
	{
		return MakeFrustumPlane
		(
			MatrixArray[0][3] - MatrixArray[0][0],
			MatrixArray[1][3] - MatrixArray[1][0],
			MatrixArray[2][3] - MatrixArray[2][0],
			MatrixArray[3][3] - MatrixArray[3][0],
			P
		);
	}

	FORCEINLINE bool Matrix4x4::GetFrustumLeftPlane
	(
		OUT Plane & P
	)	const
	{
		return MakeFrustumPlane
		(
			MatrixArray[0][3] + MatrixArray[0][0],
			MatrixArray[1][3] + MatrixArray[1][0],
			MatrixArray[2][3] + MatrixArray[2][0],
			MatrixArray[3][3] + MatrixArray[3][0],
			P
		);
	}

	FORCEINLINE bool Matrix4x4::GetFrustumBottomPlane
	(
		OUT Plane & P
	)	const
	{
		return MakeFrustumPlane
		(
			MatrixArray[0][3] + MatrixArray[0][1],
			MatrixArray[1][3] + MatrixArray[1][1],
			MatrixArray[2][3] + MatrixArray[2][1],
			MatrixArray[3][3] + MatrixArray[3][1],
			P
		);
	}

	FORCEINLINE bool Matrix4x4::GetFrustumTopPlane
	(
		OUT Plane & P
	)	const
	{
		return MakeFrustumPlane
		(
			MatrixArray[0][3] - MatrixArray[0][1],
			MatrixArray[1][3] - MatrixArray[1][1],
			MatrixArray[2][3] - MatrixArray[2][1],
			MatrixArray[3][3] - MatrixArray[3][1],
			P
		);
	}

	FORCEINLINE bool Matrix4x4::GetFrustumNearPlane
	(
		OUT Plane & P
	)	const
	{
		return MakeFrustumPlane
		(
			MatrixArray[0][2],
			MatrixArray[1][2],
			MatrixArray[2][2],
			MatrixArray[3][2],
			P
		);
	}

	FORCEINLINE bool Matrix4x4::GetFrustumFarPlane
	(
		OUT Plane & P
	)	const
	{
		return MakeFrustumPlane
		(
			MatrixArray[0][3] - MatrixArray[0][2],
			MatrixArray[1][3] - MatrixArray[1][2],
			MatrixArray[2][3] - MatrixArray[2][2],
			MatrixArray[3][3] - MatrixArray[3][2],
			P
		);
	}


	FORCEINLINE void Matrix4x4::RemoveTranslation
	(
		OUT Matrix4x4 & Matrix
	)
	{
		Matrix = *this;
		Matrix.MatrixArray[3][0] = 0.0f;
		Matrix.MatrixArray[3][1] = 0.0f;
		Matrix.MatrixArray[3][2] = 0.0f;
	}


	FORCEINLINE static Matrix4x4 CreateRotationMatrix
	(
		const Rotation & Rot
	)
	{
		return Matrix4x4(
			Vector4f(-Math::Cos(Rot.Pitch * PI / 180.f), +Math::Sin(Rot.Pitch * PI / 180.f), 0.0f, 0.0f),
			Vector4f(-Math::Sin(Rot.Pitch * PI / 180.f), -Math::Cos(Rot.Pitch * PI / 180.f), 0.0f, 0.0f),
			Vector4f(0.0f, 0.0f, 1.0f, 0.0f),
			Vector4f(0.0f, 0.0f, 0.0f, 1.0f)) *
			Matrix4x4(
				Vector4f(-Math::Cos(Rot.Roll * PI / 180.f), 0.0f, +Math::Sin(Rot.Roll * PI / 180.f), 0.0f),
				Vector4f(0.0f, 1.0f, 0.0f, 0.0f),
				Vector4f(-Math::Sin(Rot.Roll * PI / 180.f), 0.0f, -Math::Cos(Rot.Roll * PI / 180.f), 0.0f),
				Vector4f(0.0f, 0.0f, 0.0f, 1.0f)) *
			Matrix4x4(
				Vector4f(1.0f, 0.0f, 0.0f, 0.0f),
				Vector4f(0.0f, -Math::Cos(Rot.Yaw * PI / 180.f), -Math::Sin(Rot.Yaw * PI / 180.f), 0.0f),
				Vector4f(0.0f, +Math::Sin(Rot.Yaw * PI / 180.f), -Math::Cos(Rot.Yaw * PI / 180.f), 0.0f),
				Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	}


	FORCEINLINE static Matrix4x4 CreateInversedRotationMatrix
	(
		const Rotation & Rot
	)
	{
		return Matrix4x4(
			Vector4f(+Math::Cos(Rot.Yaw * PI / 180.f), -Math::Sin(Rot.Yaw * PI / 180.f), 0.0f, 0.0f),
			Vector4f(+Math::Sin(Rot.Yaw * PI / 180.f), +Math::Cos(Rot.Yaw * PI / 180.f), 0.0f, 0.0f),
			Vector4f(0.0f, 0.0f, 1.0f, 0.0f),
			Vector4f(0.0f, 0.0f, 0.0f, 1.0f)) *
			Matrix4x4(
				Vector4f(+Math::Cos(Rot.Pitch * PI / 180.f), 0.0f, -Math::Sin(Rot.Pitch * PI / 180.f), 0.0f),
				Vector4f(0.0f, 1.0f, 0.0f, 0.0f),
				Vector4f(+Math::Sin(Rot.Pitch * PI / 180.f), 0.0f, +Math::Cos(Rot.Pitch * PI / 180.f), 0.0f),
				Vector4f(0.0f, 0.0f, 0.0f, 1.0f)) *
			Matrix4x4(
				Vector4f(1.0f, 0.0f, 0.0f, 0.0f),
				Vector4f(0.0f, +Math::Cos(Rot.Roll * PI / 180.f), +Math::Sin(Rot.Roll * PI / 180.f), 0.0f),
				Vector4f(0.0f, -Math::Sin(Rot.Roll * PI / 180.f), +Math::Cos(Rot.Roll * PI / 180.f), 0.0f),
				Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	}


	FORCEINLINE static Matrix4x4 CreateOrthographicMatrix
	(
		IN Float Width,
		IN Float Height,
		IN Float NearZ,
		IN Float FarZ
	)
	{
		const float ZScale = 1.0f / (FarZ - NearZ);
		const float ZOffset = -NearZ;

		return Matrix4x4(
			Vector4f((Width) ? (1.0f / Width) : 1.0f, 0.0f, 0.0f, 0.0f),
			Vector4f(0.0f, (Height) ? (1.0f / Height) : 1.f, 0.0f, 0.0f),
			Vector4f(0.0f, 0.0f, ZScale, 0.0f),
			Vector4f(0.0f, 0.0f, ZOffset * ZScale, 1.0f)
		);

		return Matrix4x4
		(
			Vector4f(2.0f / Width,	0.0f,			0.0f,						0.0f),
			Vector4f(0.0f,			2.0f / Height,	0.0f,						0.0f),
			Vector4f(0.0f,			0.0f,			1.0f / (FarZ - NearZ),		0.0f),
			Vector4f(0.0f,			0.0f,			NearZ / (FarZ - NearZ),		1.0f)
		);
	}


	FORCEINLINE static Matrix4x4 CreatePerspectiveMatrixLH
	(
		IN Float FOV,
		IN Float Aspect,
		IN Float MinZ,
		IN Float MaxZ
	)
	{
		Matrix4x4 M(Matrix4x4::Zero());

		Float CotFOV = Math::Cos(FOV) / Math::Sin(FOV);

		M[0][0] = CotFOV / Aspect;
		M[1][1] = CotFOV;
		M[2][2] = MaxZ / (MaxZ - MinZ);
		M[2][3] = -MinZ * MaxZ / (MaxZ - MinZ);
		M[3][2] = 1.0;

		return M;
	}


	FORCEINLINE static Matrix4x4 CreatePerspectiveMatrix
	(
		IN Float HalfFOVX,
		IN Float HalfFOVY,
		IN Float MultFOVX,
		IN Float MultFOVY,
		IN Float MinZ,
		IN Float MaxZ
	)
	{
		return Matrix4x4
		(
			Vector4f(MultFOVX / Math::Tan(HalfFOVX),	0.0f,							0.0f,													0.0f),
			Vector4f(0.0f,								MultFOVY / Math::Tan(HalfFOVY),	0.0f,													0.0f),
			Vector4f(0.0f,								0.0f,							((MinZ == MaxZ) ? 1.0f : MaxZ / (MaxZ - MinZ)),			1.0f),
			Vector4f(0.0f,								0.0f,							-MinZ * ((MinZ == MaxZ) ? 1.0f : MaxZ / (MaxZ - MinZ)), 0.0f)
		);
	}


	FORCEINLINE static Matrix4x4 CreateReversedPerspectiveMatrix
	(
		IN Float HalfFOVX,
		IN Float HalfFOVY,
		IN Float MultFOVX,
		IN Float MultFOVY,
		IN Float MinZ,
		IN Float MaxZ
	)
	{
		return Matrix4x4
		(
			Vector4f(MultFOVX / Math::Tan(HalfFOVX),	0.0f,							0.0f,													0.0f),
			Vector4f(0.0f,								MultFOVY / Math::Tan(HalfFOVY),	0.0f,													0.0f),
			Vector4f(0.0f,								0.0f,							((MinZ == MaxZ) ? 0.0f : MinZ / (MinZ - MaxZ)),			1.0f),
			Vector4f(0.0f,								0.0f,							((MinZ == MaxZ) ? MinZ : -MaxZ * MinZ / (MinZ - MaxZ)),	0.0f)

		);
	}


	FORCEINLINE Matrix4x4 LookAt
	(
		const Vector3f & EyePosition, 
		const Vector3f & LookAtPosition,
		const Vector3f & UpVector
	)
	{
		Matrix4x4 M;

		const Vector3f ZAxis = (LookAtPosition - EyePosition).GetSafeNormal();
		const Vector3f XAxis = (UpVector ^ ZAxis).GetSafeNormal();
		const Vector3f YAxis = (XAxis ^ ZAxis).GetSafeNormal();

		for (uint32 ColIndex = 0; ColIndex < 3; ColIndex++)
		{
			M[0][ColIndex] = (&XAxis.X)[ColIndex];
			M[1][ColIndex] = (&YAxis.X)[ColIndex];
			M[2][ColIndex] = (&ZAxis.X)[ColIndex];
			M[3][ColIndex] = 0.0f;
		}

		return M.WithTranslation(EyePosition);
	}


	FORCEINLINE void Inverse4x4
	(
		double * Result, 
		double * Source
	)
	{
		const double s0  = (double)(Source[ 0]); const double s1  = (double)(Source[ 1]); const double s2  = (double)(Source[ 2]); const double s3  = (double)(Source[ 3]);
		const double s4  = (double)(Source[ 4]); const double s5  = (double)(Source[ 5]); const double s6  = (double)(Source[ 6]); const double s7  = (double)(Source[ 7]);
		const double s8  = (double)(Source[ 8]); const double s9  = (double)(Source[ 9]); const double s10 = (double)(Source[10]); const double s11 = (double)(Source[11]);
		const double s12 = (double)(Source[12]); const double s13 = (double)(Source[13]); const double s14 = (double)(Source[14]); const double s15 = (double)(Source[15]);

		double inv[16];
		inv[0]  =  s5 * s10 * s15 - s5 * s11 * s14 - s9 * s6 * s15 + s9 * s7 * s14 + s13 * s6 * s11 - s13 * s7 * s10;
		inv[1]  = -s1 * s10 * s15 + s1 * s11 * s14 + s9 * s2 * s15 - s9 * s3 * s14 - s13 * s2 * s11 + s13 * s3 * s10;
		inv[2]  =  s1 * s6  * s15 - s1 * s7  * s14 - s5 * s2 * s15 + s5 * s3 * s14 + s13 * s2 * s7  - s13 * s3 * s6;
		inv[3]  = -s1 * s6  * s11 + s1 * s7  * s10 + s5 * s2 * s11 - s5 * s3 * s10 - s9  * s2 * s7  + s9  * s3 * s6;
		inv[4]  = -s4 * s10 * s15 + s4 * s11 * s14 + s8 * s6 * s15 - s8 * s7 * s14 - s12 * s6 * s11 + s12 * s7 * s10;
		inv[5]  =  s0 * s10 * s15 - s0 * s11 * s14 - s8 * s2 * s15 + s8 * s3 * s14 + s12 * s2 * s11 - s12 * s3 * s10;
		inv[6]  = -s0 * s6  * s15 + s0 * s7  * s14 + s4 * s2 * s15 - s4 * s3 * s14 - s12 * s2 * s7  + s12 * s3 * s6;
		inv[7]  =  s0 * s6  * s11 - s0 * s7  * s10 - s4 * s2 * s11 + s4 * s3 * s10 + s8  * s2 * s7  - s8  * s3 * s6;
		inv[8]  =  s4 * s9  * s15 - s4 * s11 * s13 - s8 * s5 * s15 + s8 * s7 * s13 + s12 * s5 * s11 - s12 * s7 * s9;
		inv[9]  = -s0 * s9  * s15 + s0 * s11 * s13 + s8 * s1 * s15 - s8 * s3 * s13 - s12 * s1 * s11 + s12 * s3 * s9;
		inv[10] =  s0 * s5  * s15 - s0 * s7  * s13 - s4 * s1 * s15 + s4 * s3 * s13 + s12 * s1 * s7  - s12 * s3 * s5;
		inv[11] = -s0 * s5  * s11 + s0 * s7  * s9  + s4 * s1 * s11 - s4 * s3 * s9  - s8  * s1 * s7  + s8  * s3 * s5;
		inv[12] = -s4 * s9  * s14 + s4 * s10 * s13 + s8 * s5 * s14 - s8 * s6 * s13 - s12 * s5 * s10 + s12 * s6 * s9;
		inv[13] =  s0 * s9  * s14 - s0 * s10 * s13 - s8 * s1 * s14 + s8 * s2 * s13 + s12 * s1 * s10 - s12 * s2 * s9;
		inv[14] = -s0 * s5  * s14 + s0 * s6  * s13 + s4 * s1 * s14 - s4 * s2 * s13 - s12 * s1 * s6  + s12 * s2 * s5;
		inv[15] =  s0 * s5  * s10 - s0 * s6  * s9  - s4 * s1 * s10 + s4 * s2 * s9  + s8  * s1 * s6  - s8  * s2 * s5;

		double det = s0 * inv[0] + s1 * inv[4] + s2 * inv[8] + s3 * inv[12];

		if( det != 0.0 )
		{
			det = 1.0 / det;
		}

		for( int i = 0; i < 16; i++ )
		{
			Result[i] = inv[i] * det;
		}
	}

	FORCEINLINE void Vector3f::RotateByMatrix
	(
		const Matrix4x4 & RotationMatrix
	)
	{

	}
}