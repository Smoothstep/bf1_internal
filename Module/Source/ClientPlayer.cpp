#include "stdafx.h"
#include "ClientPlayer.h"
#include "ClientGame.h"
#include "D3DHooks.h"
#include "InputController.h"
#include "Crypto.h"
#include "Game.h"
#include "Hooks.h"
#include "Utility.h"

constexpr float RayCastUpdateInterval = 0.5f;

WeaponZeroingEntry ZeroingModifier::GetZeroLevelAt(int32_t Index)
{
	if (Index < 0)
	{
		return { DefaultZeroingDistance, 0 };
	}

	return ppZeroLevels[Index];
}

Vector3f& BFClientSodlierPhysicsComponent::GetViewOffsetThirdPerson(Vector3f& Result)
{
	Result = { 0.0f, 3.5f, -3.f };
	return Result;
}

BulletEntityData* ClientSoldierWeapon::GetInheritedBulletEntityData() const
{
	if (IsValidPointer(BulletSpawnData) &&
		IsValidPointer(BulletSpawnData->BulletEntityData))
	{
		return BulletSpawnData->BulletEntityData;
	}

	return nullptr;
}

const FiringFunctionData* ClientSoldierEntity::GetPrimaryFire() const
{
	if (IsValidPointer(Weapon))
	{
		auto WeaponInfo = Weapon->ClientWeapon;

		if (IsValidPointer(WeaponInfo))
		{
			auto FiringData = WeaponInfo->FiringData;

			if (IsValidPointer(FiringData))
			{
				return FiringData->PrimaryFire;
			}
		}
	}

	return nullptr;
}

std::optional<Vector3f> ClientSoldierEntity::GetBonePosition(EBones Bone) const
{
	if (!IsValidPointer(Ragdoll))
	{
		return {};
	}

	const SkeletonBulkData* Transforms = Ragdoll->ActiveTransforms;

	if (!IsValidPointer(Transforms))
	{
		return {};
	}

	return std::make_optional(Transforms->BoneTransforms[Bone].TransAndScale);
}

std::optional<Vector3f> ClientSoldierEntity::GetShootSpace() const
{
	if (!IsValidPointer(Weapon))
	{
		return {};
	}

	const ClientWeapon* WeaponInfo = Weapon->ClientWeapon;

	if (!IsValidPointer(WeaponInfo))
	{
		return {};
	}

	return std::make_optional(WeaponInfo->Shootspace.MatrixVector[3]);
}

std::optional<Matrix4x4> ClientSoldierEntity::GetShootSpaceMatrix() const
{
	if (!IsValidPointer(Weapon))
	{
		return {};
	}

	const ClientWeapon* WeaponInfo = Weapon->ClientWeapon;

	if (!IsValidPointer(WeaponInfo))
	{
		return {};
	}

	return std::make_optional(WeaponInfo->Shootspace);
}

std::optional<float> ClientSoldierEntity::GetBulletDistance() const
{
	if (!IsValidPointer(Weapon))
	{
		return {};
	}

	auto ClientWeapon = Weapon->ClientWeapon;

	if (!IsValidPointer(ClientWeapon))
	{
		return {};
	}

	if (WeaponComponent &&
		WeaponComponent->ZeroingLevel != -1)
	{
		auto Modifier = ClientWeapon->WeaponModifier;

		if (Modifier &&
			Modifier->Zeroing)
		{
			if (auto Data = Modifier->Zeroing->ZeroingData)
			{
				return Data->Entries[WeaponComponent->ZeroingLevel].ZeroDistance;
			}
		}
	}

	return {};
}

std::optional<float> ClientSoldierEntity::GetBulletSpawnOffset() const
{
	if (!IsValidPointer(Weapon))
	{
		return {};
	}

	auto ClientWeapon = Weapon->ClientWeapon;

	if (!IsValidPointer(ClientWeapon))
	{
		return {};
	}

	if (WeaponComponent &&
		WeaponComponent->ZeroingLevel != -1)
	{
		auto Modifier = ClientWeapon->WeaponModifier;

		if (Modifier &&
			Modifier->Zeroing)
		{
			if (auto Data = Modifier->Zeroing->ZeroingData; Data)
			{
				return std::make_optional(Data->Entries[WeaponComponent->ZeroingLevel].Angle);
			}
		}
	}

	return {};
}

ClientSpottingComponent* ClientSoldierEntity::GetSpottingComponent()
{
	return CallVirtual<ClientSpottingComponent*>(this, EVMTFunctionsClientPlayer::Index_GetComponent, EComponentClassOffsets::Offset_SpottingComponent);
}

static std::optional<RayCastHit> HitTest(const Vector3f& From, const Vector3f& To)
{
	GameLevel* Level = GameLevel::GetCurrentLevel();

	if (!IsValidPointer(Level))
	{
		return {};
	}

	PhysicsManager* PhysicsMgr = Level->PhysicsManager;

	if (!IsValidPointer(PhysicsMgr))
	{
		return {};
	}

	IRayCaster* RayCaster = PhysicsMgr->RayCastDispatcher;

	if (!IsValidPointer(RayCaster))
	{
		return {};
	}

	__declspec(align(16)) Vector4f F(From);
	__declspec(align(16)) Vector4f T(To);

	RayCastHit Result;

	if (!RayCaster->Query("HitTest", F, T, Result, ETraceActorFlags::NoWater | ETraceActorFlags::NoRagdoll | ETraceActorFlags::NoPhantoms, nullptr))
	{
		return {};
	}

	return std::make_optional(Result);
}

static bool CheckVisibility(const Vector3f & From, const Vector3f & To)
{
	if (auto Test = HitTest(From, To))
	{
		auto Result = *Test;

		if (Result.Material.CanPenetrate() || Vector3f::Distance(Result.Position, To) < 5.0f)
		{
			return true;
		}
	}

	return false;
}

ClientSoldierPrediction * ClientSoldierEntity::GetPrediction()
{
	return reinterpret_cast<ClientSoldierPrediction*>(
		Crypto::DecryptPointer(Prediction, reinterpret_cast<uint64_t>(this)));
}

void * ClientSoldierEntity::UpdatePosition(void * Data)
{
	auto HkRef = HookRefSpec<D3D::GameInterface*>::Current();
	void* Result = (this->*HkRef->GetOriginal<decltype(&ClientSoldierEntity::UpdatePosition)>())(Data);
	auto& [Gi] = HkRef.Tuple();

	const bool ApplyChams = 
		Gi->ShouldRenderVisuals() &&
		Gi->ShouldRenderChams();

	if (!ApplyChams)
	{
		Chams = VisDefault;
	}

	const float Now = ClientGameContext::Instance()->GameTimeMs();

	if (IsValidPointer(Owner) && 
		IsValidPointer(ClientStatusComponent) && !ClientStatusComponent->IsDead())
	{
		auto Local = Crypto::GetLocalPlayer();

		if (IsValidPointer(Local) && Local->TeamId != Owner->TeamId)
		{
			if (Local->InVehicle())
			{
				return Result;
			}

			ClientSoldierEntity * LocalSoldier = Local->Soldier;

			if (IsValidPointer(LocalSoldier) && IsValidPointer(LocalSoldier->ClientStatusComponent) && !LocalSoldier->ClientStatusComponent->IsDead())
			{
				if (LocalSoldier != this)
				{
					const bool Occluded = LocalSoldier->IsOccluded;

					if (!Occluded && ((Now < LastQuery) || (Now - LastQuery > RayCastUpdateInterval)))
					{
						if (auto Pos = LocalSoldier->GetShootSpace())
						{
							bool IsVisible = false;

							if (auto Utility = Gi->GetUtility())
							{
								VisibleBones = 0;

								const auto& Bones = Utility->GetRcBehavior().Bones;

								for (size_t N = 0; N < Bones.size(); ++N)
								{
									if (auto Target = GetBonePosition(Bones[N]))
									{
										if (CheckVisibility(*Pos, *Target))
										{
											VisibleBones |= (1 << N);
											IsVisible = true;
										}
									}
								}

								LastQuery = Now;
							}

							if (ApplyChams)
							{
								Chams = IsVisible ? VisOutline : VisOutlineAlpha;
							}
						}
					}
				}
			}
		}
	}

	return Result;
}

void * ClientSoldierEntity::UpdateVisibillity(void * Arg0, void * Arg1)
{
	const float Now = ClientGameContext::Instance()->GameTimeMs();

	if (IsValidPointer(ClientStatusComponent) && !ClientStatusComponent->IsDead())
	{
		const ClientPlayer* Local = Crypto::GetLocalPlayer();

		if (IsValidPointer(Local))
		{
			const ClientSoldierEntity * LocalSoldier = Local->Soldier;

			if (IsValidPointer(LocalSoldier) && LocalSoldier != this && !LocalSoldier->IsOccluded && Now - LocalSoldier->LastQuery > 100)
			{
				const auto Pos = LocalSoldier->GetShootSpace();
				const auto Target = GetBonePosition(BoneHead);

				if (Pos && Target)
				{
					Visibillity = !CheckVisibility(*Pos, *Target);
					LastQuery = Now;
				}
			}
		}
	}

	return CallMemberFunctionAddress<void*>(this, HookRef::Current()->GetOriginal<void*>(), Arg0, Arg1);
}

void ClientSpottingComponent::Spot(ClientPlayer* Player, int Type, const eastl::FixedVector<PhysicsEntityBase*, 8>* Targets, ClientSoldierEntity* Single)
{
	Impl.Process(Player, Type, Targets, Single);
}

void ClientWeapon::SetShootspace(const Matrix4x4& Mat, InputController& Ic)
{
	auto* Local = Crypto::GetLocalPlayer();

	if (IsValidPointer(Local))
	{
		if (!Local->InVehicle())
		{
			if (IsValidPointer(Local->Soldier) &&
				IsValidPointer(Local->Soldier->Weapon))
			{
				if (Local->Soldier->Weapon->ClientWeapon == this)
				{
					if (auto Vo = Ic.GetViewOffset())
					{
						if (Local->Soldier->AimEntity)
						{
							if (Local->Soldier->AimEntity->Offset)
							{
								*Local->Soldier->AimEntity->Offset = *Vo - Local->Soldier->SoldierPhysicsComponent->ViewOffset;
							}
						}
					}
				}
			}
		}
	}

	Shootspace = Mat;
}

float ClientSoldierAimingSimulation::GetYaw(Vector4f& Unused) const
{
	auto HkRef = HookRefSpec<InputController&>::Current();
	auto& [Ic] = HkRef.Tuple();

	auto* Local = Crypto::GetLocalPlayer();
	auto Result = (this->*HkRef->GetOriginal<decltype(&ClientSoldierAimingSimulation::GetYaw)>())(Unused);

	if (IsValidPointer(Local) &&
		IsValidPointer(Local->Soldier) &&
		!Local->InVehicle() &&
		Ic.GetViewOffset())
	{
		if (auto Pos = Local->Soldier->GetShootSpace())
		{
			Vector3f Dir;

			if (auto LookAtPos = Ic.GetLookAt())
			{
				Dir = (*LookAtPos - *Pos).GetSafeNormal();
			}
			else
			{
				Dir = (Local->Soldier->AimEntity->LookAt - *Pos).GetSafeNormal();
			}

			const float Yaw = Math::Atan2(Dir.X, Dir.Z);

			return (PI * 2.f) - Yaw;
		}
	}

	return Result;
}

float ClientSoldierAimingSimulation::GetPitch(Vector4f& Unused) const
{
	auto HkRef = HookRefSpec<InputController&>::Current();
	auto& [Ic] = HkRef.Tuple();

	auto* Local = Crypto::GetLocalPlayer();
	auto Result = (this->*HkRef->GetOriginal<decltype(&ClientSoldierAimingSimulation::GetPitch)>())(Unused);

	if (IsValidPointer(Local) &&
		IsValidPointer(Local->Soldier) &&
		!Local->InVehicle() &&
		Ic.GetViewOffset())
	{
		if (auto Pos = Local->Soldier->GetShootSpace())
		{
			Vector3f Dir;

			if (auto LookAtPos = Ic.GetLookAt())
			{
				Dir = (*LookAtPos - *Pos).GetSafeNormal();
			}
			else
			{
				Dir = (Local->Soldier->AimEntity->LookAt - *Pos).GetSafeNormal();
			}

			const float Pitch = Math::FastAsin(Dir.Y);

			return Pitch;
		}
	}

	return Result;
}