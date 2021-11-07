#pragma once

constexpr AimConstants::AimConstants(const Default&) noexcept
	: AimVariance{ 0.5f }
	, AimVelocity{ 1.f }
	, AimAccuracy{ 1.f }
	, AimSmoothness{ 1.f }
{}

constexpr OptimizedValue::OptimizedValue(float Base, float Next) noexcept
	: Base{ Base }
	, Next{ Next }
{}

inline CurveDistribution::CurveDistribution(float Mean, float Sigma, float Factor, float Degradation) noexcept
	: RndEngine{ std::make_unique<std::default_random_engine>(), std::make_unique<std::default_random_engine>() }
	, Dist{ Mean, Sigma }
	, Factor{ Factor }
	, Degradation{ Degradation }
	, RotationTime{ 0.15f }
{}

inline float CurveDistribution::operator()(float Time, float TimeMax) noexcept
{
	const float Position = Time / RotationTime;
	const float PositionNext = Math::Ceil(Position + SMALL_NUMBER);

	RndEngine[0]->seed(static_cast<uint32_t>(PositionNext - 1));
	RndEngine[1]->seed(static_cast<uint32_t>(PositionNext));

	float Ta = Dist(*RndEngine[0]);
	float Tb = Dist(*RndEngine[1]);
	Ta = 1.0f;
	Tb = 1.0f;
	const float TimeDelta = Math::Max(TimeMax - Time, 0.0f);
	const float Power = Math::Lerp(Tb, Ta, PositionNext - Position);
	const float T = TimeDelta * TimeDelta * 0.5f;
	const float R = Power * Math::Max(0.1f, Math::Pow(T, 1.0f + Degradation));

	return R * Factor;
}

constexpr CurveAttributes::CurveAttributes(const Default&)
	: DistanceThreshold{ 0.f, 5000.f }
	, UsageProbability{ 0.5f }
	, Overshoot{ GDefault }
{}

constexpr CurveAttributes::CurveAttributes(std::array<float, 2> DistanceThreshold, float UsageProbability, const CurveOvershootAttributes& OvershootAttr)
	: DistanceThreshold{ DistanceThreshold }
	, UsageProbability{ UsageProbability }
	, Overshoot{ OvershootAttr }
{}

constexpr CurveOvershootAttributes::CurveOvershootAttributes(const Default&) noexcept
	: CurveOvershootAttributes(0.2f, 0.2f, 0.5f, 2.0f, 0.5f, 2.0f, 0.5f)
{}

constexpr CurveOvershootAttributes::CurveOvershootAttributes(float Frequency, float Factor, float Degradation, float MeanX, float SigmaX, float MeanY, float SigmaY) noexcept
	: Frequency{ Frequency }
	, Factor{ Factor }
	, Degradation{ Degradation }
	, MeanX{ MeanX }
	, SigmaX{ std::max(SigmaX, SMALL_NUMBER) }
	, MeanY{ MeanY }
	, SigmaY{ std::max(SigmaY, SMALL_NUMBER) }
{}

constexpr CurveTransform::CurveTransform(float Velocity, float Power, float RotationStep,
	float RotationStepMicro, float OvershootTimeMax, float OvershootThreshold) noexcept
	: Velocity{ Velocity }
	, Power{ Power }
	, RotationStep{ RotationStep }
	, RotationStepMicro{ RotationStepMicro }
	, OvershootTimeMax{ OvershootTimeMax }
	, OvershootThreshold{ OvershootThreshold }
{}

constexpr float CurveTransform::Intensity(float DeltaX) const
{
	return (10.0f / static_cast<float>(Math::FastPow(50.0f / Math::Max(1.0f, DeltaX - 100.0f), 5.0f / (DeltaX * 0.01f)))) * Power;
}

constexpr Vector2f CurveTransform::Leverage(Vector2f Value) const
{
	return Value + Value * 0.5f * Vector2f(0.5f, 0.5f) / (Value.GetAbs() + 0.1f);
}