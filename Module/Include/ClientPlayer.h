#pragma once

#include <chrono>
#include <optional>

#include "stdafx.h"
#include "Offsets.h"
#include "ClassDecl.h"
#include "Eastl.h"

class WeaponZeroingEntry
{
public:
	float ZeroDistance; //0x0000
	float Angle; //0x0004
	float AngleX;
};//Size=0x0008

class ZeroingModifier
{
public:
	WeaponZeroingEntry GetZeroLevelAt(int32_t Index);

	Pad(0x18);
	WeaponZeroingEntry* ppZeroLevels; //0x0018
	float DefaultZeroingDistance; //0x0020 
};

class BulletEntityData
{
public:
	Pad(0x140);
	float Gravity; //0x0140
	float Drag;
};

class BulletSpawnData
{
public:
	Pad(0x18);
	class BulletEntityData* BulletEntityData; //0x0018
};

class ReplicatedFiring
{
private:
	virtual void F0() = 0;
};

class WeaponFiring
{
private:
	virtual void F0() = 0;
};

class FiringFunctionData
{
public:
	Pad(0xA4);
	float BulletAcceleration; //0x00A4
	float BulletVelocity; //0x00A8
	Pad(0x08);
	float BulletSpawnOffsetY; //0x00B4
	Pad(0x28);
	class BulletEntityData* BulletEntityData; //0x00E0
};

class GunSwayData
{
private:
	virtual void Function0() = 0;

public:
	Pad(0x3C0);
	float RecoilData[256];
};

class WeaponFiringData
{
private:
	virtual void Function0() = 0;

public:
	Pad(0x08);
	class FiringFunctionData* PrimaryFire; //0x0010
	Pad(0x20);
	class GunSwayData* SwayData; //0x0038
};

class WeaponZeroingData
{
public:
	WeaponZeroingEntry Entries[1];
};

class WeaponZeroing
{
public:
	Pad(0x18);
	class WeaponZeroingData* ZeroingData; //0x0018
};

class WeaponModifier
{
public:
	Pad(0x40);
	uint8_t BreathModifier; //0x0040
	Pad(0x8F);
	class WeaponZeroing* Zeroing; //0x00D0
};

class ClientWeapon
{
public:
	void SetShootspace(const Matrix4x4& Mat, InputController& Ic);

	Pad(0x18);
	class WeaponFiringData* FiringData; //0x0018
	class WeaponModifier* WeaponModifier; //0x0020
	Pad(0x08);
	Vector4f Velocity; //0x0030
	Hyper::Matrix4x4 Shootspace; //0x0040
};

class BulletConfigData
{
public:
	Pad(0x30);
	float BulletSpawnOffsetX; //0x0030
	float BulletSpawnOffsetY; //0x0034
	Pad(0x1C);
	float Acceleration; //0x0054
	float Velocity; //0x0058
};

class ClientSoldierAimingSimulation
{
public:
	float GetYaw(Vector4f&) const;
	float GetPitch(Vector4f&) const;

	Pad(0x30);
	Vector2f Angles;
	Pad(0x58);
	Matrix4x4 AimMatrix;
};

class ClientAimEntity
{
public:
	Pad(0x40);
	Vector3f* Offset;
	Pad(0x118);
	Vector3f LookAt;
	Pad(0x38);
	ClientSoldierAimingSimulation* AuthorativeAiming;
};

class BFClientSodlierPhysicsComponent
{
public:
	Vector3f& GetViewOffsetThirdPerson(Vector3f& Result);
};

class ClientSoldierWeapon
{
public:
	BulletEntityData* GetInheritedBulletEntityData() const;

	Pad(0x48A0);
	class BulletConfigData* BulletConfigData; //0x48A0
	Pad(0x08);
	class BulletSpawnData* BulletSpawnData; //0x48B0
	Pad(0x178);
	class ClientWeapon* ClientWeapon; //0x4A30
	class ReplicatedFiring* ReplicatedFiring; //0x4A38
	Pad(0x08);
	class WeaponFiring* PredictedFiring; //0x4A48
	Pad(0x5F8);
};

class ClientSoldierWeaponsComponent
{
private:
	virtual void Function0() = 0;

public:
	Pad(0x30);
	class ClientSoldierWeapon* Weapon; //0x0038	
	Pad(0x9A0);
	int32_t ZeroingLevel; //0x09E0
	Pad(0x664);
};

struct QuatTransform
{
	Vector4f TransAndScale;
	Vector4f Rotation;
};

struct SkeletonBulkData
{
	QuatTransform BoneTransforms[1];
};

class BoneCollisionComponent
{
private:
	virtual void Function0() = 0;

public:
	Pad(0x10);
	struct SkeletonBulkData* ActiveTransforms; //0x0018
	Pad(0x1C);
	bool ValidTransforms; //0x003C
};

class ClientStatusComponent
{
private:
	virtual void Function0() = 0;

public:
	constexpr bool IsDead() const
	{
		return Health < 1.f;
	}

	Pad(0x18);
	float Health; //0x0020
	float HealthMax; //0x0024
};

class ClientSpottingComponent
{
	struct Spotter
	{
		void Process(ClientPlayer* Player, int Type, const eastl::FixedVector<PhysicsEntityBase*, 8>* Targets, ClientSoldierEntity* Single)
		{
			using TSpotTargets = void(__thiscall*)(Spotter* Component, ClientPlayer* Player, int Type, 
				const eastl::FixedVector<PhysicsEntityBase*, 8>* Targets, ClientSoldierEntity* Single);
			static const TSpotTargets SpotTargets = reinterpret_cast<TSpotTargets>(EFunctionOffsets::Offset_Spot);
			SpotTargets(this, Player, Type, Targets, Single);
		}
	};
	
	Pad(0x1F08);
	Spotter Impl;

	virtual void Function0() = 0;

public:
	void Spot(ClientPlayer* Player, int Type, const eastl::FixedVector<PhysicsEntityBase*, 8>* Targets, ClientSoldierEntity* Single);
};

class ClientSpottingTargetComponent
{
public:
	virtual void Function0() = 0;
};

class ClientSoldierPrediction
{
public:
	Pad(0x60);
	Vector3f Position;
	Pad(0x14);
	Vector3f Velocity;
};

class PhysicsEntityBase
{
	virtual void F0() = 0; //
	virtual void F1() = 0; //
	virtual void F2() = 0; //
	virtual void F3() = 0; //
	virtual void F4() = 0; //
	virtual void F5() = 0; //
	virtual void F6() = 0; //
	virtual void F7() = 0; //
	virtual void F8() = 0; //
	virtual void F9() = 0; //
	virtual void F10() = 0; //
	virtual void F11() = 0; //
	virtual void F12() = 0; //
	virtual void F13() = 0; //
	virtual void F14() = 0; //
	virtual void F15() = 0; //
	virtual void F16() = 0; //
	virtual void F17() = 0; //
	virtual void F18() = 0; //
	virtual void F19() = 0; //
	virtual void F20() = 0; //
	virtual void F21() = 0; //
	virtual void F22() = 0; //
	virtual void F23() = 0; // 
	virtual void F24() = 0;

public:
	virtual void GetAABB(AABBTransform& pAABB) const = 0;
	virtual void GetTransform(Matrix4x4& Transform) const = 0;

	AABBTransform AABB() const
	{
		AABBTransform Transform;
		GetAABB(Transform);
		return Transform;
	}
};

class BFClientSoldierPhysicsComponent
{
public:
	Pad(0x70);
	Vector4f ViewOffset;
};

class ClientSoldierEntity : public PhysicsEntityBase
{
public:
	std::optional<Vector3f> GetBonePosition(EBones Bone) const;
	std::optional<Vector3f> GetShootSpace() const;
	std::optional<Matrix4x4> GetShootSpaceMatrix() const;
	std::optional<float> GetBulletDistance() const;
	std::optional<float> GetBulletSpawnOffset() const;

	void * UpdatePosition(void* Data);
	void * UpdateVisibillity(void* Arg0, void* Arg1);

	const class FiringFunctionData* GetPrimaryFire() const;
	class ClientSpottingComponent* GetSpottingComponent();
	class ClientSoldierPrediction * GetPrediction();

	Pad(0x12);
	uint8_t Visibillity; //0x001A
	Pad(0x65);
	class BFClientSoldierPhysicsComponent* SoldierPhysicsComponent;
	Pad(0x148);
	class ClientStatusComponent* ClientStatusComponent; //0x01D0
	Pad(0xA0);
	class ClientPlayer* Owner; //0x0278
	Pad(0xA4);
	uint8_t Chams; //0x0324
	Pad(0x16B);
	class BoneCollisionComponent* Ragdoll; //0x0490
	Pad(0x148);
	uint64_t Prediction; //0x05E0
	Pad(0x50);
	int32_t Pose; //0x0638
	int32_t RenderFlags; //0x063C
	Pad(0x10);
	float LastQuery; // Injected
	float LastCheck; // Injected
	int VisibleBones;
	Pad(0x3C);
	class ClientSoldierWeaponsComponent* WeaponComponent; //0x0690
	Pad(0x18);
	class ClientAimEntity* AimEntity; //0x06B8
	class ClientSoldierWeapon* Weapon; //0x06C0
	Pad(0x20);
	bool IsSprinting; //0x06E8
	bool IsOccluded; //0x06E9
	Pad(0x7E);
	class ClientSpottingTargetComponent* TargetSpottingComponent;
};

class ClientVehicleEntity : public PhysicsEntityBase
{
public:
	Pad(0xC0);
	uint8_t TeamId;
};

class ClientPlayer
{
private:
	virtual void F0() = 0;

public:
	bool InVehicle() const
	{
		return IsValidPointer(Vehicle);
	}

	Pad(0x18);
	char* Name;
	Pad(0x18);
	char NameBuffer[16]; //0x0040
	Pad(0x1BE4);
	uint8_t TeamId; //0x1C34
	Pad(0x103);
	class ClientVehicleEntity* Vehicle; //0x1D38
	Pad(0x08);
	class ClientSoldierEntity* Soldier; //0x1D48
};