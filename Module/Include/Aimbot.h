#pragma once

#include "AimCurve.h"
#include "ProjectileTrajectory.h"

enum AimTargets : uint64_t
{
	TargetNone = 0,
	TargetAllies = 1 << 0,
	TargetEnemies = 1 << 1,
	TargetVehicles = 1 << 2,
	TargetSoldiers = 1 << 3,
	TargetAll = ~0ULL
};

struct TriggerBehavior
{
	TriggerBehavior() = default;
	TriggerBehavior(const Default&);

	std::array<float, 2> MaxDistanceWorld;
	std::vector<EBones> TriggerBones;
};

struct AimTargetWeights
{
	AimTargetWeights() noexcept = default;
	constexpr AimTargetWeights(const Default&) noexcept;

	float PreviousTargetWeightMultiplier;
	float D3DistanceImportanceWeight;
	float D2DistanceImportanceWeight;
	float HealthDifferenceWeight;
};

struct RaycastBehavior
{
	RaycastBehavior() = default;
	RaycastBehavior(const Default&);

	std::vector<EBones> Bones;
};

struct NearestEntityObject
{
	constexpr NearestEntityObject(const NearestEntityObject&) noexcept = default;
	constexpr NearestEntityObject(NearestEntityObject&& Other) noexcept;
	constexpr NearestEntityObject(
		const PhysicsEntityBase* Entity = nullptr,
		float Distance3D = std::numeric_limits<float>::max(),
		float Distance2D = std::numeric_limits<float>::max(),
		float Insignificance = std::numeric_limits<float>::max(),
		Vector3f Position = Vector3f::ZeroVector,
		Vector3f Velocity = Vector3f::ZeroVector) noexcept;

	constexpr NearestEntityObject& operator=(const NearestEntityObject& Other) noexcept = default;

	constexpr bool operator==(const NearestEntityObject& Other) const;
	constexpr bool operator!=(const NearestEntityObject& Other) const;

	const PhysicsEntityBase* Entity;
	float Distance3D;
	float Distance2D;
	float Insignificance;
	Vector3f Position;
	Vector3f Velocity;
};

struct AimResult
{
	AimResult() noexcept = default;
	AimResult(
		const NearestEntityObject& Entity,
		std::shared_ptr<AimCurve> Cv,
		CurvePosition&& CvData,
		float TimeToHit) noexcept;

	void Update(const float Time, const NearestEntityObject& Entity);

	constexpr bool HasTarget() const;

	std::shared_ptr<AimCurve> Cv;
	CurvePosition CvData;
	NearestEntityObject TargetObject;
	float TimeToHit;
};

struct AimBehavior
{
	struct CurveInfo
	{
		constexpr bool IsApplicable(float Distance2D) const
		{
			return
				Attr.DistanceThreshold[1] > Distance2D &&
				Attr.DistanceThreshold[0] < Distance2D;
		}

		std::shared_ptr<AimCurve> AimCurve;
		CurveAttributes Attr;
	};

	AimBehavior() noexcept = default;
	AimBehavior(const Default&);

	// Unsafe procedure. AimCurves might be accessed by another thread.
	std::tuple<std::shared_ptr<AimCurve>, CurvePosition> SelectCurve(
		const float Distance2D) const;

	float AimSpeed(const InputController& Ic) const;

	std::array<float, 2> MaxDistanceWorld;
	float MaxDistanceScreen;
	bool UseRaycast;
	bool Silent;
	AimConstants Constants;
	AimTargetWeights Weights;
	uint64_t Targets;
	std::vector<EBones> TargetBones;
	std::vector<CurveInfo> AimCurves;
	CurveInfo DefaultCurve;
};

class Aimbot
{
	static constexpr size_t TrajectoryIterationCount = 32;

public:
	Aimbot(const RaycastBehavior& RcBehavior, const AimBehavior& Behavior = { GDefault }) noexcept;

	void Execute(InputController& Ic);
	void Stop(InputController& Ic);

	inline AimBehavior& GetBehavior() { return Behavior; }
	inline const AimBehavior& GetBehavior() const { return Behavior; }

private:
	std::optional<NearestEntityObject> BestEntity(const ClientPlayer* Local, const Vector3f& Position);

	const RaycastBehavior& RcBehavior;
	AimBehavior Behavior;
	AimResult AimLast;
	AimResult AimCurr;
	float LastExecutionTime;
	ProjectileMotionSim Sim;
};

class Triggerbot
{
public:
	Triggerbot(const RaycastBehavior& RcBehavior, const TriggerBehavior& Behavior = { GDefault }) noexcept;

	void Evaluate(InputController& Ic);
	void Stop(InputController& Ic);

	inline TriggerBehavior& GetBehavior() { return Behavior; }
	inline const TriggerBehavior& GetBehavior() const { return Behavior; }

private:
	const PhysicsEntityBase* BestEntity(const ClientPlayer* Local, const Vector3f& Position, const Vector3f& LookAt);

	const RaycastBehavior& RcBehavior;
	TriggerBehavior Behavior;
	const PhysicsEntityBase* LastTarget;
};