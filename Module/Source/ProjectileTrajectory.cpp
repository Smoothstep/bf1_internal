#include "stdafx.h"
#include "ProjectileTrajectory.h"
#include "ClientPlayer.h"

constexpr ProjectileAttr::ProjectileAttr(float Gravity, float Drag, float Velocity, float Acceleration, float SpawnOffset) noexcept
	: Gravity{ Gravity }
	, Drag{ Drag }
	, Velocity{ Velocity }
	, Acceleration{ Acceleration }
	, SpawnOffset{ SpawnOffset }
{}

std::optional<ProjectileAttr> ProjectileAttr::FromEntity(const ClientSoldierEntity* Soldier)
{
	const FiringFunctionData* PFire = Soldier->GetPrimaryFire();

	if (!IsValidPointer(PFire))
	{
		return {};
	}
	
	const auto* BulletData = Soldier->Weapon->GetInheritedBulletEntityData();

	if (!IsValidPointer(BulletData))
	{
		BulletData = PFire->BulletEntityData;
	}

	if (!IsValidPointer(BulletData))
	{
		return {};
	}

	auto Offset = Soldier->GetBulletSpawnOffset();

	if (const auto* ConfigData = Soldier->Weapon->BulletConfigData; ConfigData)
	{
		return std::make_optional<ProjectileAttr>(BulletData->Gravity, BulletData->Drag, ConfigData->Velocity, 
			ConfigData->Acceleration, Offset ? *Offset : ConfigData->BulletSpawnOffsetY);
	}
	else if (const auto* PrimaryFire = Soldier->GetPrimaryFire())
	{
		return std::make_optional<ProjectileAttr>(BulletData->Gravity, BulletData->Drag, PrimaryFire->BulletVelocity, 
			PrimaryFire->BulletAcceleration, Offset ? *Offset : PrimaryFire->BulletSpawnOffsetY);
	}

	return {};
}

std::optional<ProjectileAttr> ProjectileAttr::FromEntity(const ClientVehicleEntity* Vehicle)
{
	return {};
}

Vector3f ProjectileTrajectory::ApplyVelocity(const Vector3f& Position, const Vector3f& Velocity, const float Time) const
{
	return Position + Velocity * Time + Vector3f::UpVector * Attr.Gravity * Time * Time * 0.5f;
}

Vector3f ProjectileTrajectory::ProcessVelocity(const Vector3f& CurVelocity, const float Time) const
{
	const Vector3f NewVelocity = CurVelocity + Vector3f::UpVector * Attr.Gravity * Time;

	const float Sz = CurVelocity.SizeSquared();
	const float Sq = Math::Sqrt(Sz);

	if (Sq > SMALL_NUMBER)
	{
		return NewVelocity - (((CurVelocity / Sq) * Sz) * Attr.Drag) * Time;
	}

	return NewVelocity;
}

ProjectileTrajectory::ProjectileTrajectory(const ProjectileAttr& Attr) noexcept
	: Attr(Attr)
{}

ProjectileState ProjectileTrajectory::Begin(const Vector3f& InitialPosition, const Vector3f& Target) const
{
	const Vector3f Forward = (Target - InitialPosition).GetSafeNormal();
	const Vector3f Right = (Forward ^ Vector3f::UpVector).GetSafeNormal();
	const Vector3f Up = (Right ^ Forward).GetSafeNormal();

	return
	{
		0.0f,
		InitialPosition + Vector3f::UpVector * Attr.SpawnOffset,
		Forward * Attr.Velocity + Up * Attr.Acceleration
	};
}

ProjectileState ProjectileTrajectory::Step(const ProjectileState& PreviousState, const float Time) const
{
	return
	{
		PreviousState.FlightTime + Time,
		ApplyVelocity(PreviousState.Position, PreviousState.Velocity, Time),
		ProcessVelocity(PreviousState.Velocity, Time)
	};
}

ProjectileMotionSim::ProjectileMotionSim(
	const float MinDistance,
	const float MaxTime,
	const float TimeStep,
	const float TimeBias) noexcept
	: MinDistance{ MinDistance }
	, MaxTime{ MaxTime }
	, TimeStep{ TimeStep }
	, TimeBias{ TimeBias }
{}

ProjectileMotionSim::HeightOffsetEstimation ProjectileMotionSim::CalculateHeightOffset(const ProjectileTrajectory& Trajectory, 
	const Vector3f& Origin, const Vector3f& Target, const Vector3f& TargetVelocity, size_t Iterations)
{
	HeightOffsetEstimation Estimation{};

	for (size_t Iter = 0; Iter < Iterations; ++Iter)
	{
		const Vector3f TargetPosition = Target + Estimation.Time * TargetVelocity + Estimation.HeightOffset * Vector3f::UpVector;
		const Vector3f RayDir = (TargetPosition - Origin).GetSafeNormal();

		ProjectileState StatePrev = Trajectory.Begin(Origin, TargetPosition);

		while (StatePrev.FlightTime < MaxTime)
		{
			ProjectileState State = Trajectory.Step(StatePrev, TimeStep);

			if ((TargetPosition.X - State.Position.X) * RayDir.X < SMALL_NUMBER &&
				(TargetPosition.Z - State.Position.Z) * RayDir.Z < SMALL_NUMBER)
			{
				const float Error = static_cast<float>(Iterations - Iter) / Iterations;

				const Vector3f A = State.Position - StatePrev.Position;
				const Vector3f B = TargetPosition - StatePrev.Position;
				const Vector3f P = StatePrev.Position + A.GetSafeNormal() * B.Size2D();

				const float TA = (State.Position - StatePrev.Position).Size2D();
				const float TB = (State.Position - P).Size2D();
				const float TT = ((TA - TB) / TA) * TimeStep;

				State = Trajectory.Step(StatePrev, TT);

				Estimation =
				{
					Target.Y - State.Position.Y + Estimation.HeightOffset + Error,
					State.FlightTime + TimeBias
				};

				break;
			}

			StatePrev = State;
		}
	}

	return Estimation;
}