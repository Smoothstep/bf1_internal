#include "stdafx.h"
#include "AimCurve.h"

constexpr float Smooth(float V, float Edge0, float Edge1)
{
	const float T = Math::Clamp((V - Edge0) / (Edge1 - Edge0), 0.0f, 1.0f);
	return T * T * (3.0f - 2.0f * T);
}

inline float GenerateNoise()
{
	return static_cast<float>(std::rand()) / RAND_MAX;
}

constexpr float LerpNoise(OptimizedValue& Value, float TimeDiff, float TimeMax, float TimeStep)
{
	if (TimeDiff > TimeMax + SMALL_NUMBER)
	{
		Value = { Value.Next, GenerateNoise() };
		TimeMax = Math::Ceil(TimeDiff / TimeStep) * TimeStep;
	}

	return Math::Lerp(Value.Base, Value.Next, Smooth(TimeDiff, TimeMax - TimeStep, TimeMax));
}

Vector2f CalculateAngles(
	const Vector3f& Direction,
	const Vector3f& Right,
	const Vector3f& Dir)
{
	const Vector2f Result =
	{
		-Math::Atan2(Direction.X, Direction.Z),
		 Math::FastAsin(Direction.Y)
	};

	const Vector3f Delta = (Direction - Dir);

	const float Yaw = Math::Atan2(-Dir.X, Dir.Z);
	const float Pitch = Math::FastAsin(Delta.Y);
	const float Theta = Result.X - Yaw;

	return { Theta, Pitch };

	/*
	float InitialPitch = Math::FastAsin(Dir.Y);
	float Yaw = Math::Atan2(Delta.X, Delta.Z);

	while (InitialYaw < 0)
	{
		InitialYaw += 2 * PI;
	}

	while (InitialPitch < 0)
	{
		InitialPitch += 2 * PI;
	}

	Result.X -= InitialYaw;
	Result.Y -= InitialPitch;

	if (InitialPitch > 0)
	{
		Result.Y -= InitialPitch; 
	}

	return { Result.X, Result.Y };
	*/
}

Vector2f CalculateAimDirectionDelta(const Matrix4x4& View, const Vector3f& Direction)
{
	const Vector3f Right = View[0];
	const Vector3f Up = View[1];
	const Vector3f Dir = View[2];

	const float AimZ = Dir | Direction;
	const float AimX = (Right | Direction) * AimZ;
	const float AimY = (Up | Direction) * AimZ;

	return{ AimX, AimY };
}

CurvePosition::CurvePosition(
	const float Delta,
	CurveDistribution&& DistX,
	CurveDistribution&& DistY) noexcept
	: DeltaDirection{}
	, DeltaDirectionAcc{}
	, InitialDelta{ Delta }
	, CurrentDelta{ Delta }
	, Time{ 0 }
	, ReachTime{ 0 }
	, Deviance{ 0 }
	, CurveIndex{ 0 }
	, OptimizedValues{ }
	, DistX{ std::move(DistX) }
	, DistY{ std::move(DistY) }
{}

Vector2f CurvePosition::CalculateOvershoot(float Time, float TimeMax)
{
	return Vector2f
	{
		DistX(Time, TimeMax),
		DistY(Time, TimeMax)
	};
}

CurveDistribution CurveOvershootAttributes::DistX() const
{
	return { MeanX, SigmaX, Factor, Degradation };
}

CurveDistribution CurveOvershootAttributes::DistY() const
{
	return { MeanY, SigmaY, Factor, Degradation };
}

AimCurve::AimCurve()
	: MaxDelta{ 0 }
	, MinDelta{ 0 }
{
	Points.push_back(0.f);
}

AimCurve::AimCurve(const std::vector<float>& NonNormalizedPoints, const float Avg)
{
	float Min = std::numeric_limits<float>::max();
	float Max = std::numeric_limits<float>::min();

	Points.resize(NonNormalizedPoints.size());
	{
		std::transform(NonNormalizedPoints.begin(), NonNormalizedPoints.end(), Points.begin(), [&](const float Pnt)
		{
			Min = Math::Min(Min, Pnt);
			Max = Math::Max(Max, Pnt);
			return (Pnt - Avg) / Avg;
		});
	}

	Points.push_back(0.f);

	MaxDelta = Max;
	MinDelta = Min;
}

AimCurve::AimCurve(const std::vector<float>& NonNormalizedPoints)
{
	float Min = std::numeric_limits<float>::max();
	float Max = std::numeric_limits<float>::min();

	for (const float Pnt : NonNormalizedPoints)
	{
		Min = Math::Min(Min, Pnt);
		Max = Math::Max(Max, Pnt);
	}

	const float Avg = Min + (Max - Min) * 0.5f;

	Points.resize(NonNormalizedPoints.size());
	{
		std::transform(NonNormalizedPoints.begin(), NonNormalizedPoints.end(), Points.begin(), [Avg](const float Point)
		{
			return (Point - Avg) / Avg;
		});
	}

	Points.push_back(0.f);

	MaxDelta = Max;
	MinDelta = Min;
}

CurvePosition AimCurve::Evaluate(CurvePosition&& PrevAimPosition, const AimConstants& Constants, Vector3f TargetPosition,
	const Matrix4x4& View, const float Time, const CurveTransform& Transform) const
{
	CurvePosition NewPosition = std::move(PrevAimPosition);

	auto OptimizedValueIterator = NewPosition.OptimizedValues.begin();

	const auto GenerateNoise = [&](const float Step, const float Factor = 1.0f)
	{
		return Factor * LerpNoise(*OptimizedValueIterator++,Time, Math::Ceil(PrevAimPosition.Time / Step) * Step, Step);
	};

	const float Intensity = Math::Max(Transform.Intensity(NewPosition.CurrentDelta), 0.0f) * 0.01f;
	const float RevAcc = Math::Clamp(1.0f - Constants.AimAccuracy, 0.0f, 1.0f) * (Intensity + 1.0f);

	TargetPosition += Vector3f
	{
		GenerateNoise(Transform.RotationStep, RevAcc),
		-Math::Abs(GenerateNoise(Transform.RotationStep, RevAcc)),
		GenerateNoise(Transform.RotationStep, RevAcc)
	};

	const float TimeDelta = Time - PrevAimPosition.Time;
	const float Acceleration = (0.5f + Math::Abs(GenerateNoise(Transform.RotationStep, Constants.AimVariance)) * Intensity) * Math::Max(0.0f, 1.0f - Time);
	const float Velocity = Acceleration + Math::Max(0.01f, (Constants.AimVelocity) * Transform.Velocity) * 0.03f;
	const float DiffPos = Math::Clamp((NewPosition.InitialDelta - NewPosition.CurrentDelta) / NewPosition.InitialDelta, 0.0f, 1.0f);

	const float Jitter = 
		GenerateNoise(Transform.RotationStep, Constants.AimVariance) +
		GenerateNoise(Transform.RotationStepMicro, Constants.AimVariance * 0.05f);

	Vector2f Interpolated = { 0.f, Jitter * Intensity };
	Vector3f Direction = (TargetPosition - View.GetOrigin()).GetSafeNormal();
	Vector2f AimDirection = CalculateAimDirectionDelta(View, Direction);

	if (Points.size() > 1)
	{
		const float Offset = DiffPos * Points.size();
		const size_t Idx = static_cast<size_t>(std::roundf(Offset));

		if (Idx < Points.size() - 1)
		{
			const float Y0 = Points[Idx];
			const float Y1 = Points[Idx + 1];

			Interpolated.Y += Math::Lerp(Y0, Y1, Offset - Idx) * (MaxDelta - MinDelta) * 0.005f;
		}
	}

	if (NewPosition.CurrentDelta < Transform.OvershootThreshold)
	{
		if (NewPosition.ReachTime <= 0.0f)
		{
			NewPosition.ReachTime = Time;
			NewPosition.DeltaDirectionAcc = AimDirection.GetSafeNormal();
		}
	}

	if (NewPosition.ReachTime > 0)
	{
		const float TimeDelta = Time - NewPosition.ReachTime;

		if (Transform.OvershootTimeMax > TimeDelta)
		{
			Interpolated += NewPosition.CalculateOvershoot(TimeDelta, Transform.OvershootTimeMax) * NewPosition.DeltaDirectionSum * Velocity;
		}
	}

	if (Time > 0)
	{
		NewPosition.DeltaDirectionSum = (NewPosition.DeltaDirectionSum * PrevAimPosition.Time + AimDirection * TimeDelta) / Time;
	}

	NewPosition.DeltaDirection = (Interpolated + (AimDirection * Velocity)) * Vector2f(-1.0f, 0.5f);
	NewPosition.Time = Time;

	return NewPosition;
}