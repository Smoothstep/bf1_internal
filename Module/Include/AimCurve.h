#pragma once

#include <vector>
#include <cmath>
#include <utility>
#include <algorithm>
#include <limits>
#include <random>

#include "ClassDecl.h"

class AimCurve;

struct AimConstants
{
	constexpr AimConstants(const Default & = {}) noexcept;

	float AimVariance;
	float AimVelocity;
	float AimAccuracy;
	float AimSmoothness;
};

struct OptimizedValue
{
	constexpr OptimizedValue(float Base = 0.f, float Next = 0.f) noexcept;

	float Base;
	float Next;
};

class CurveDistribution
{
public:
	inline CurveDistribution(float Mean = 1.f, float Sigma = 1.f, float Factor = 0.f, float Degradation = 0.5f) noexcept;
	inline float operator()(float Time, float TimeMax) noexcept;

private:
	std::unique_ptr<std::default_random_engine> RndEngine[2];
	std::normal_distribution<float> Dist;
	float Factor;
	float Degradation;
	float RotationTime;
};

struct CurvePosition
{
	CurvePosition(
		const float Delta = 0.f,
		CurveDistribution&& DistX = {},
		CurveDistribution&& DistY = {}) noexcept;

	Vector2f CalculateOvershoot(float Time, float TimeMax);

	Vector2f DeltaDirection;
	Vector2f DeltaDirectionSum;
	Vector2f DeltaDirectionAcc;
	float InitialDelta;
	float CurrentDelta;
	float Time;
	float ReachTime;
	float Deviance;
	size_t CurveIndex;
	std::array<OptimizedValue, 64> OptimizedValues;
	CurveDistribution DistX;
	CurveDistribution DistY;
};

struct CurveOvershootAttributes
{
	constexpr CurveOvershootAttributes(const CurveOvershootAttributes&) noexcept = default;
	constexpr CurveOvershootAttributes(const Default & = {}) noexcept;
	constexpr CurveOvershootAttributes(float Frequency, float Factor, float Degradation, float MeanX, float SigmaX, float MeanY, float SigmaY) noexcept;

	CurveDistribution DistX() const;
	CurveDistribution DistY() const;

	float Frequency;
	float Factor;
	float Degradation;
	float MeanX;
	float SigmaX;
	float MeanY;
	float SigmaY;
};

struct CurveAttributes
{
	constexpr CurveAttributes(const CurveAttributes&) = default;
	constexpr CurveAttributes(const Default & = {});
	constexpr CurveAttributes(std::array<float, 2> DistanceThreshold, float UsageProbability, const CurveOvershootAttributes& OvershootAttr);

	std::array<float, 2> DistanceThreshold;
	float UsageProbability;
	CurveOvershootAttributes Overshoot;
};

struct CurveTransform
{
	constexpr CurveTransform(float Velocity, float Power, float RotationStep = 0.45f, float RotationStepMicro = 0.1f,
		float OvershootTimeMax = 0.3f, float OvershootThreshold = 30.0f) noexcept;

	constexpr float Intensity(float DeltaX) const;
	constexpr Vector2f Leverage(Vector2f Value) const;

	float Velocity;
	float Power;
	float RotationStep;
	float RotationStepMicro;
	float OvershootTimeMax;
	float OvershootThreshold;
};

class AimCurve
{
public:
	AimCurve();
	AimCurve(const std::vector<float>& NonNormalizedPoints, const float Avg);
	AimCurve(const std::vector<float>& NonNormalizedPoints);

	CurvePosition Evaluate(CurvePosition&& PrevAimPosition, const AimConstants& Constants, Vector3f TargetPosition,
		const Matrix4x4& View, const float Time, const CurveTransform& Transform) const;

private:
	std::vector<float> Points;
	std::vector<float> PointsOvershoot;
	float MaxDelta;
	float MinDelta;
};

#include "AimCurve.inl"