#pragma once

#include "Vector3.h"

namespace Hyper
{
	struct Rotation
	{
	public:
		float Pitch;
		float Yaw;
		float Roll;

	public:
		static const Rotation ZeroRotation;

	public:
		FORCEINLINE void DiagnosticCheckNaN() const {}
		FORCEINLINE void DiagnosticCheckNaN
		(
			const TCHAR* Message
		)	const {}

		FORCEINLINE Rotation
		(
			const float InF = 0.f
		);

		FORCEINLINE Rotation
		(
			const float InPitch, 
			const float InYaw,
			const float InRoll
		);

		FORCEINLINE Rotation
		(
			const Quaternion & Quat
		);

	public:
		FORCEINLINE Rotation operator+
		(
			const Rotation & R
		)	const;

		FORCEINLINE Rotation operator-
		(
			const Rotation & R
		)	const;

		FORCEINLINE Rotation operator-() const;

		FORCEINLINE Rotation operator*
		(
			const float Scale
		)	const;

		FORCEINLINE Rotation operator*=
		(
			const float Scale
		);

		FORCEINLINE bool operator==
		(
			const Rotation & R
		)	const;

		FORCEINLINE bool operator!=
		(
			const Rotation & V
		)	const;

		FORCEINLINE Rotation operator+=
		(
			const Rotation & R
		);

		FORCEINLINE Rotation operator-=
		(
			const Rotation & R
		);

	public:
		FORCEINLINE bool IsNearlyZero
		(
			const	float Tolerance = KINDA_SMALL_NUMBER
		)	const;

		FORCEINLINE bool IsZero() const;

		FORCEINLINE bool Equals
		(
			const	Rotation &  R, 
			const	float		Tolerance = KINDA_SMALL_NUMBER
		)	const;

		FORCEINLINE Rotation Add
		(
			const	float DeltaPitch,
			const	float DeltaYaw,
			const	float DeltaRoll
		);

		FORCEINLINE Rotation GetInverse() const;

		FORCEINLINE Rotation GridSnap
		(
			const Rotation& RotGrid
		)	const;

		Vector3f	GetVector()		const;
		Vector3f	GetEuler()		const;
		Quaternion	GetQuaternion() const;

		Vector3f RotateVector
		(
			const Vector3f & V
		)	const;

		Vector3f UnrotateVector
		(
			const Vector3f & V
		)	const;

		FORCEINLINE Rotation Clamp()			const;
		FORCEINLINE Rotation GetNormalized()	const;
		FORCEINLINE Rotation GetDenormalized()	const;

		FORCEINLINE void Normalize();

		void GetWindingAndRemainder
		(
					Rotation & Winding, 
					Rotation & Remainder
		)	const;

		FORCEINLINE bool ContainsNaN() const;

	public:
		static float ClampAxis
		(
			const float Angle
		);

		static float NormalizeAxis
		(
			const float Angle
		);

		static uint8 CompressAxisToByte
		(
			const float Angle
		);

		static float DecompressAxisFromByte
		(
			const uint16 Angle
		);

		static uint16 CompressAxisToShort
		(
			const float Angle
		);

		static float DecompressAxisFromShort
		(
			const uint16 Angle
		);

		static Rotation MakeFromEuler
		(
			const Vector3f & Euler
		);
	};

	FORCEINLINE Rotation operator*
	(
		const	float		Scale,
		const	Rotation &  R)
	{
		return R * Scale;
	}


	FORCEINLINE Rotation::Rotation
	(
		const float InF
	)
		: Pitch(InF), Yaw(InF), Roll(InF)
	{
		DiagnosticCheckNaN();
	}


	FORCEINLINE Rotation::Rotation
	(
		const float InPitch,
		const float InYaw,
		const float InRoll
	)
		: Pitch(InPitch), Yaw(InYaw), Roll(InRoll)
	{
		DiagnosticCheckNaN();
	}


	FORCEINLINE Rotation Rotation::operator+
	(
		const Rotation & R
	)	const
	{
		return Rotation(Pitch + R.Pitch, Yaw + R.Yaw, Roll + R.Roll);
	}


	FORCEINLINE Rotation Rotation::operator-
	(
		const Rotation & R
	)	const
	{
		return Rotation(Pitch - R.Pitch, Yaw - R.Yaw, Roll - R.Roll);
	}


	FORCEINLINE Rotation Rotation::operator*
	(
		const float Scale
	)	const
	{
		return Rotation(Pitch*Scale, Yaw*Scale, Roll*Scale);
	}


	FORCEINLINE Rotation Rotation::operator*=
	(
		const float Scale
	)
	{
		Pitch = Pitch*Scale; Yaw = Yaw*Scale; Roll = Roll*Scale;
		DiagnosticCheckNaN();
		return *this;
	}


	FORCEINLINE Rotation Rotation::operator-() const
	{
		return Rotation(-Pitch, -Yaw, -Roll);
	}


	FORCEINLINE bool Rotation::operator==
	(
		const Rotation & R
	)	const
	{
		return Pitch == R.Pitch && Yaw == R.Yaw && Roll == R.Roll;
	}


	FORCEINLINE bool Rotation::operator!=
	(
		const Rotation & V
	)	const
	{
		return Pitch != V.Pitch || Yaw != V.Yaw || Roll != V.Roll;
	}


	FORCEINLINE Rotation Rotation::operator+=
	(
		const Rotation & R
	)
	{
		Pitch += R.Pitch; Yaw += R.Yaw; Roll += R.Roll;
		DiagnosticCheckNaN();
		return *this;
	}


	FORCEINLINE Rotation Rotation::operator-=
	(
		const Rotation & R
	)
	{
		Pitch -= R.Pitch; Yaw -= R.Yaw; Roll -= R.Roll;
		DiagnosticCheckNaN();
		return *this;
	}


	FORCEINLINE bool Rotation::IsNearlyZero
	(
		const float Tolerance
	)	const
	{
#if PLATFORM_ENABLE_VECTORINTRINSICS
		const VectorRegister RegA = VectorLoadFloat3_W0(this);
		const VectorRegister Norm = VectorNormalizeRotator(RegA);
		const VectorRegister AbsNorm = VectorAbs(Norm);
		return !VectorAnyGreaterThan(AbsNorm, VectorLoadFloat1(&Tolerance));
#else
		return
			Math::Abs(NormalizeAxis(Pitch)) <= Tolerance
			&&	Math::Abs(NormalizeAxis(Yaw)) <= Tolerance
			&&	Math::Abs(NormalizeAxis(Roll)) <= Tolerance;
#endif
	}


	FORCEINLINE bool Rotation::IsZero() const
	{
		return (ClampAxis(Pitch) == 0.f) && (ClampAxis(Yaw) == 0.f) && (ClampAxis(Roll) == 0.f);
	}


	FORCEINLINE bool Rotation::Equals
	(
		const Rotation& R, 
		const float Tolerance
	)	const
	{
#if PLATFORM_ENABLE_VECTORINTRINSICS
		const VectorRegister RegA = VectorLoadFloat3_W0(this);
		const VectorRegister RegB = VectorLoadFloat3_W0(&R);
		const VectorRegister NormDelta = VectorNormalizeRotator(VectorSubtract(RegA, RegB));
		const VectorRegister AbsNormDelta = VectorAbs(NormDelta);
		return !VectorAnyGreaterThan(AbsNormDelta, VectorLoadFloat1(&Tolerance));
#else
		return (Math::Abs(NormalizeAxis(Pitch - R.Pitch)) <= Tolerance)
			&& (Math::Abs(NormalizeAxis(Yaw - R.Yaw)) <= Tolerance)
			&& (Math::Abs(NormalizeAxis(Roll - R.Roll)) <= Tolerance);
#endif
	}


	FORCEINLINE Rotation Rotation::Add
	(
		const float DeltaPitch, 
		const float DeltaYaw, 
		const float DeltaRoll
	)
	{
		Yaw		+= DeltaYaw;
		Pitch	+= DeltaPitch;
		Roll	+= DeltaRoll;
		DiagnosticCheckNaN();
		return *this;
	}


	FORCEINLINE Rotation Rotation::GridSnap
	(
		const Rotation& RotGrid
	)	const
	{
		return Rotation
		(
			Math::GridSnap(Pitch, RotGrid.Pitch),
			Math::GridSnap(Yaw, RotGrid.Yaw),
			Math::GridSnap(Roll, RotGrid.Roll)
		);
	}


	FORCEINLINE Rotation Rotation::Clamp() const
	{
		return Rotation(ClampAxis(Pitch), ClampAxis(Yaw), ClampAxis(Roll));
	}


	FORCEINLINE float Rotation::ClampAxis
	(
		float Angle
	)
	{
		Angle = Math::FMod(Angle, 360.f);

		if (Angle < 0.f)
		{
			Angle += 360.f;
		}

		return Angle;
	}


	FORCEINLINE float Rotation::NormalizeAxis
	(
		float Angle
	)
	{
		Angle = ClampAxis(Angle);

		if (Angle > 180.f)
		{
			Angle -= 360.f;
		}

		return Angle;
	}


	FORCEINLINE uint8 Rotation::CompressAxisToByte
	(
		const float Angle
	)
	{
		return Math::RoundToInt(Angle * 256.f / 360.f) & 0xFF;
	}


	FORCEINLINE float Rotation::DecompressAxisFromByte
	(
		const uint16 Angle
	)
	{
		return (Angle * 360.f / 256.f);
	}


	FORCEINLINE uint16 Rotation::CompressAxisToShort
	(
		const float Angle
	)
	{
		return Math::RoundToInt(Angle * 65536.f / 360.f) & 0xFFFF;
	}


	FORCEINLINE float Rotation::DecompressAxisFromShort
	(
		const uint16 Angle
	)
	{
		return (Angle * 360.f / 65536.f);
	}


	FORCEINLINE Rotation Rotation::GetNormalized() const
	{
		Rotation Rot = *this;
		Rot.Normalize();
		return Rot;
	}


	FORCEINLINE Rotation Rotation::GetDenormalized() const
	{
		Rotation Rot = *this;
		Rot.Pitch = ClampAxis(Rot.Pitch);
		Rot.Yaw = ClampAxis(Rot.Yaw);
		Rot.Roll = ClampAxis(Rot.Roll);
		return Rot;
	}


	FORCEINLINE void Rotation::Normalize()
	{
#if PLATFORM_ENABLE_VECTORINTRINSICS
		VectorRegister VRotator = VectorLoadFloat3_W0(this);
		VRotator = VectorNormalizeRotator(VRotator);
		VectorStoreFloat3(VRotator, this);
#else
		Pitch = NormalizeAxis(Pitch);
		Yaw = NormalizeAxis(Yaw);
		Roll = NormalizeAxis(Roll);
#endif
		DiagnosticCheckNaN();
	}


	FORCEINLINE bool Rotation::ContainsNaN() const
	{
		return (
			!Math::IsFinite(Pitch) ||
			!Math::IsFinite(Yaw) ||
			!Math::IsFinite(Roll));
	}

	template<class U>
	FORCEINLINE Rotation Math::Lerp
	(
		const	Rotation & A, 
		const	Rotation & B, 
		const	U		 & Alpha
	)
	{
		return A + (B - A).GetNormalized() * Alpha;
	}

	template<class U>
	FORCEINLINE Rotation Math::LerpRange
	(
		const	Rotation & A,
		const	Rotation & B,
		const	U		 & Alpha
	)
	{
		// Similar to Lerp, but does not take the shortest path. Allows interpolation over more than 180 degrees.
		return (A * (1 - Alpha) + B * Alpha).GetNormalized();
	}
}
