#pragma once

#include "Matrix.h"
#include "Quaternion.h"

namespace Hyper
{
	using namespace Memory;

	ALIGN(16) struct Transform
	{
	protected:

		M128 Rotation;
		M128 Translation;
		M128 Scale3D;

	public:

		FORCEINLINE Transform()
		{
			Rotation	= VectorSet_W1(VectorZero());
			Translation = VectorZero();
			Scale3D		= VectorSet_W0(VectorOne());
		}

		FORCEINLINE explicit Transform
		(
			const Matrix4x4 & Matrix
		)
		{
			SetFromMatrix(Matrix);
		}

		void SetFromMatrix
		(
			const Matrix4x4 & InMatrix
		)
		{
			Matrix4x4 M = InMatrix;

			Vector3f InScale = M.ExtractScaling();
			Scale3D = VectorLoadFloat3_W0(&InScale);

			if (InMatrix.Determinant() < 0.f)
			{		
				Scale3D = VectorMultiply(Scale3D, SSE::FloatMinus1_111);
				M.SetAxis(EAxis::X, -M.GetScaledAxis(EAxis::X));
			}

			Quaternion InRotation = Quaternion(M);
			Rotation = VectorLoadAligned(&InRotation);
			Vector3f InTranslation = InMatrix.GetOrigin();
			Translation = VectorLoadFloat3_W0(&InTranslation);

			Rotation = Math::SSE::VectorNormalizeQuaternion(Rotation);
		}

		FORCEINLINE Matrix4x4 ToMatrixWithScale() const
		{
			Matrix4x4	OutMatrix;
			M128		DiagonalsXYZ;
			M128		Adds;
			M128		Subtracts;

			ToMatrixInternal(DiagonalsXYZ, Adds, Subtracts);
			const M128 DiagonalsXYZ_W0 = VectorSet_W0(DiagonalsXYZ);

			const M128 AddX_DC_DiagX_DC = VectorShuffle(Adds, DiagonalsXYZ_W0, 0, 0, 0, 0);
			const M128 SubZ_DC_DiagW_DC = VectorShuffle(Subtracts, DiagonalsXYZ_W0, 2, 0, 3, 0);
			const M128 Row0 = VectorShuffle(AddX_DC_DiagX_DC, SubZ_DC_DiagW_DC, 2, 0, 0, 2);

			const M128 SubX_DC_DiagY_DC = VectorShuffle(Subtracts, DiagonalsXYZ_W0, 0, 0, 1, 0);
			const M128 AddY_DC_DiagW_DC = VectorShuffle(Adds, DiagonalsXYZ_W0, 1, 0, 3, 0);
			const M128 Row1 = VectorShuffle(SubX_DC_DiagY_DC, AddY_DC_DiagW_DC, 0, 2, 0, 2);

			const M128 AddZ_DC_SubY_DC = VectorShuffle(Adds, Subtracts, 2, 0, 1, 0);
			const M128 Row2 = VectorShuffle(AddZ_DC_SubY_DC, DiagonalsXYZ_W0, 0, 2, 2, 3);
			const M128 Row3 = VectorSet_W1(Translation);

			VectorStoreAligned(Row0, &(OutMatrix.MatrixArray[0][0]));
			VectorStoreAligned(Row1, &(OutMatrix.MatrixArray[1][0]));
			VectorStoreAligned(Row2, &(OutMatrix.MatrixArray[2][0]));
			VectorStoreAligned(Row3, &(OutMatrix.MatrixArray[3][0]));

			return OutMatrix;
		}

		FORCEINLINE Matrix4x4 ToMatrixNoScale() const
		{
			Matrix4x4	OutMatrix;
			M128		DiagonalsXYZ;
			M128		Adds;
			M128		Subtracts;

			ToMatrixInternalNoScale(DiagonalsXYZ, Adds, Subtracts);

			const M128 DiagonalsXYZ_W0	= VectorSet_W0(DiagonalsXYZ);

			const M128 AddX_DC_DiagX_DC = VectorShuffle(Adds, DiagonalsXYZ_W0, 0, 0, 0, 0);
			const M128 SubZ_DC_DiagW_DC = VectorShuffle(Subtracts, DiagonalsXYZ_W0, 2, 0, 3, 0);
			const M128 Row0				= VectorShuffle(AddX_DC_DiagX_DC, SubZ_DC_DiagW_DC, 2, 0, 0, 2);

			const M128 SubX_DC_DiagY_DC = VectorShuffle(Subtracts, DiagonalsXYZ_W0, 0, 0, 1, 0);
			const M128 AddY_DC_DiagW_DC = VectorShuffle(Adds, DiagonalsXYZ_W0, 1, 0, 3, 0);
			const M128 Row1				= VectorShuffle(SubX_DC_DiagY_DC, AddY_DC_DiagW_DC, 0, 2, 0, 2);

			const M128 AddZ_DC_SubY_DC	= VectorShuffle(Adds, Subtracts, 2, 0, 1, 0);
			const M128 Row2				= VectorShuffle(AddZ_DC_SubY_DC, DiagonalsXYZ_W0, 0, 2, 2, 3);
			const M128 Row3				= VectorSet_W1(Translation);

			VectorStoreAligned(Row0, &(OutMatrix.MatrixArray[0][0]));
			VectorStoreAligned(Row1, &(OutMatrix.MatrixArray[1][0]));
			VectorStoreAligned(Row2, &(OutMatrix.MatrixArray[2][0]));
			VectorStoreAligned(Row3, &(OutMatrix.MatrixArray[3][0]));

			return OutMatrix;
		}

	private:

		FORCEINLINE void ToMatrixInternal
		(
				M128 & OutDiagonals, 
				M128 & OutAdds, 
				M128 & OutSubtracts
		) const
		{
			const M128 RotationX2Y2Z2		= VectorAdd(Rotation, Rotation);
			const M128 RotationXX2YY2ZZ2	= VectorMultiply(RotationX2Y2Z2, Rotation);		

			const M128 yy2_xx2_xx2	= VectorSwizzle(RotationXX2YY2ZZ2, 1, 0, 0, 0);
			const M128 zz2_zz2_yy2	= VectorSwizzle(RotationXX2YY2ZZ2, 2, 2, 1, 0);
			const M128 DiagonalSum	= VectorAdd(yy2_xx2_xx2, zz2_zz2_yy2);
			const M128 Diagonals	= VectorSubtract(VectorOne(), DiagonalSum);

			OutDiagonals			= VectorMultiply(Diagonals, Scale3D);

			const M128 x_y_x		= VectorSwizzle(Rotation, 0, 1, 0, 0);
			const M128 y2_z2_z2		= VectorSwizzle(RotationX2Y2Z2, 1, 2, 2, 0);
			const M128 RotBase		= VectorMultiply(x_y_x, y2_z2_z2);

			const M128 w_w_w		= VectorReplicate(Rotation, 3);
			const M128 z2_x2_y2		= VectorSwizzle(RotationX2Y2Z2, 2, 0, 1, 0);
			const M128 RotOffset	= VectorMultiply(w_w_w, z2_x2_y2);

			const M128 Adds			= VectorAdd(RotBase, RotOffset);
			OutAdds					= VectorMultiply(Adds, Scale3D);
			const M128 Scale3DYZXW	= VectorSwizzle(Scale3D, 1, 2, 0, 3);
			const M128 Subtracts	= VectorSubtract(RotBase, RotOffset);
			OutSubtracts			= VectorMultiply(Subtracts, Scale3DYZXW);
		}

		FORCEINLINE void ToMatrixInternalNoScale
		(
				M128 & OutDiagonals, 
				M128 & OutAdds,
				M128 & OutSubtracts
		) const
		{
			const M128 RotationX2Y2Z2		= VectorAdd(Rotation, Rotation);
			const M128 RotationXX2YY2ZZ2	= VectorMultiply(RotationX2Y2Z2, Rotation);		

			const M128 yy2_xx2_xx2			= VectorSwizzle(RotationXX2YY2ZZ2, 1, 0, 0, 0);
			const M128 zz2_zz2_yy2			= VectorSwizzle(RotationXX2YY2ZZ2, 2, 2, 1, 0);
			const M128 DiagonalSum			= VectorAdd(yy2_xx2_xx2, zz2_zz2_yy2);
				
			OutDiagonals					= VectorSubtract(VectorOne(), DiagonalSum);

			const M128 x_y_x				= VectorSwizzle(Rotation, 0, 1, 0, 0);
			const M128 y2_z2_z2				= VectorSwizzle(RotationX2Y2Z2, 1, 2, 2, 0);
			const M128 RotBase				= VectorMultiply(x_y_x, y2_z2_z2);

			const M128 w_w_w				= VectorReplicate(Rotation, 3);
			const M128 z2_x2_y2				= VectorSwizzle(RotationX2Y2Z2, 2, 0, 1, 0);
			const M128 RotOffset			= VectorMultiply(w_w_w, z2_x2_y2);

			OutAdds							= VectorAdd(RotBase, RotOffset);
			OutSubtracts					= VectorSubtract(RotBase, RotOffset);
		}
	};
}