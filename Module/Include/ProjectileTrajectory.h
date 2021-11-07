#pragma once

#include <optional>
#include "ClassDecl.h"

struct ProjectileAttr
{
	static std::optional<ProjectileAttr> FromEntity(const class ClientSoldierEntity* Soldier);
	static std::optional<ProjectileAttr> FromEntity(const class ClientVehicleEntity* Vehicle);

	constexpr ProjectileAttr(float Gravity, float Drag, 
		float Velocity, float Acceleration, float SpawnOffset) noexcept;

	float Gravity;
	float Drag;
	float Velocity;
	float Acceleration;
	float SpawnOffset;
};

struct ProjectileState
{
	float FlightTime;
	Vector3f Position;
	Vector3f Velocity;
};

class ProjectileTrajectory
{
public:
	ProjectileTrajectory(const ProjectileAttr& Attr) noexcept;

	ProjectileState Begin(const Vector3f& InitialPosition, const Vector3f& Target) const;
	ProjectileState Step(const ProjectileState& PreviousState, const float Time) const;

private:
	Vector3f ApplyVelocity(const Vector3f& Position, const Vector3f& Velocity, const float Time) const;
	Vector3f ProcessVelocity(const Vector3f& CurVelocity, const float Time) const;

	const ProjectileAttr Attr;
};

class ProjectileMotionSim
{
	struct HeightOffsetEstimation
	{
		float HeightOffset;
		float Time;
	};

public:
	ProjectileMotionSim(
		const float MinDistance = SMALL_NUMBER,
		const float MaxTime = 10.0f,
		const float TimeStep = 1.0f / 60.0f,
		const float TimeBias = 0.01f) noexcept;

	HeightOffsetEstimation CalculateHeightOffset(const ProjectileTrajectory& Trajectory, const Vector3f& Origin,
		const Vector3f& Target, const Vector3f& TargetVelocity, size_t Iterations);

private:
	const float MinDistance;
	const float MaxTime;
	const float TimeStep;
	const float TimeBias;
};