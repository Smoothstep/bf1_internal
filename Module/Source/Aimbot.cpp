#include "stdafx.h"
#include "Aimbot.h"
#include "Crypto.h"
#include "ClientPlayer.h"
#include "ClientGame.h"
#include "Globals.h"
#include "Game.h"
#include "GameHooks.h"
#include "ProjectileTrajectory.h"
#include "Hooks.h"
#include "InputController.h"

#include <numeric>

constexpr AimTargetWeights::AimTargetWeights(const Default&) noexcept
	: PreviousTargetWeightMultiplier{ 2.f }
	, D3DistanceImportanceWeight{ 1.f }
	, D2DistanceImportanceWeight{ 5.f }
	, HealthDifferenceWeight{ 1.f }
{}

TriggerBehavior::TriggerBehavior(const Default&)
	: MaxDistanceWorld{ 0.f, 5000.f }
{
	TriggerBones.push_back(EBones::BoneAny);
}

RaycastBehavior::RaycastBehavior(const Default&)
{
	Bones.push_back(EBones::BoneHead);
	Bones.push_back(EBones::BoneSpine);
}

constexpr NearestEntityObject::NearestEntityObject(NearestEntityObject && Other) noexcept
	: Entity{ Other.Entity }
	, Distance3D{ Other.Distance3D }
	, Distance2D{ Other.Distance2D }
	, Insignificance{ Other.Insignificance }
	, Position{ Other.Position }
	, Velocity{ Other.Velocity }
{
	Other.Entity = nullptr;
}

constexpr NearestEntityObject::NearestEntityObject(
	const PhysicsEntityBase * Entity,
	float Distance3D,
	float Distance2D,
	float Insignificance,
	Vector3f Position,
	Vector3f Velocity) noexcept
	: Entity{ Entity }
	, Distance3D{ Distance3D }
	, Distance2D{ Distance2D }
	, Insignificance{ Insignificance }
	, Position{ Position }
	, Velocity{ Velocity }
{}

constexpr bool NearestEntityObject::operator==(const NearestEntityObject & Other) const
{
	return Entity == Other.Entity;
}

constexpr bool NearestEntityObject::operator!=(const NearestEntityObject & Other) const
{
	return !(*this == Other);
}

AimResult::AimResult(
	const NearestEntityObject& Entity,
	std::shared_ptr<AimCurve> Cv,
	CurvePosition&& CvData,
	float TimeToHit) noexcept
	: Cv{ Cv }
	, CvData{ std::move(CvData) }
	, TargetObject{ Entity }
	, TimeToHit{ TimeToHit }
{}

void AimResult::Update(const float Time, const NearestEntityObject& Entity)
{
	TargetObject = Entity;
	CvData.CurrentDelta = Entity.Distance2D;
}

constexpr bool AimResult::HasTarget() const
{
	return TargetObject.Entity != nullptr;
}

AimBehavior::AimBehavior(const Default&)
	: MaxDistanceWorld{ 0.f, 5000.f }
	, MaxDistanceScreen{ 90.f }
	, UseRaycast{ true }
	, Silent{ false }
	, Constants{ GDefault }
	, Weights{ GDefault }
	, Targets{ TargetAllies | TargetVehicles }
{
	TargetBones.push_back(BoneHead);
	TargetBones.push_back(BoneSpine);
	DefaultCurve = { std::make_shared<AimCurve>(), GDefault };
}

std::tuple<std::shared_ptr<AimCurve>, CurvePosition> AimBehavior::SelectCurve(const float Distance2D) const
{
	std::vector<CurveInfo> OrderedCurves;

	for (const auto& Info : AimCurves)
	{
		if (Info.IsApplicable(Distance2D))
		{
			OrderedCurves.push_back(Info);
		}
	}

	std::sort(OrderedCurves.begin(), OrderedCurves.end(), [](const auto& Lhs, const auto& Rhs)
	{
		return Lhs.Attr.UsageProbability > Rhs.Attr.UsageProbability;
	});

	const auto& Selected = [&]() -> const CurveInfo&
	{
		if (auto Iter = std::lower_bound(OrderedCurves.begin(), OrderedCurves.end(), 
			Math::Abs(static_cast<float>(std::rand()) / RAND_MAX) * 100.f,
		[](const auto& Tpl, float Chn)
		{
			return Tpl.Attr.UsageProbability < Chn;
		}); Iter != OrderedCurves.end())
		{
			return *Iter;
		}
		else
		{
			return OrderedCurves.size() > 0 ? *OrderedCurves.begin() : DefaultCurve;
		}
	}();

	return std::make_tuple(Selected.AimCurve, CurvePosition { Distance2D, 
		Selected.Attr.Overshoot.DistX(), Selected.Attr.Overshoot.DistY() });
}

float AimBehavior::AimSpeed(const InputController& Ic) const
{
	const float Factor = Ic.GetInputType() == EInputType::TypeDirect ? 0.1f : 50.0f;
	return Factor * (1.0f + 1.0f / GameRenderer::Instance()->RenderView->Fov());
}

Aimbot::Aimbot(const RaycastBehavior& RcBehavior, const AimBehavior& Behavior) noexcept
	: RcBehavior{ RcBehavior }
	, Behavior{ Behavior }
	, AimLast{}
	, AimCurr{}
	, LastExecutionTime{ 0.f }
	, Sim{  }
{}

std::optional<NearestEntityObject> Aimbot::BestEntity(const ClientPlayer* Local, const Vector3f& Position)
{
	auto* Screen = DxRenderer::Instance()->Screen;

	if (!IsValidPointer(Screen))
	{
		return {};
	}

	ClientSoldierEntity* LocalSoldier = Local->Soldier;

	if (!IsValidPointer(LocalSoldier))
	{
		return {};
	}

	GameRenderer* Renderer = GameRenderer::Instance();

	if (!IsValidPointer(Renderer))
	{
		return {};
	}

	auto Mgr = CryptoManager::Instance()->PlayerManager();
	if (!Mgr)
	{
		return {};
	}

	auto Result = std::reduce(Mgr->begin(), Mgr->end(), NearestEntityObject{}, [&, Viewport{ Screen->GetViewport() / 2 }](
		NearestEntityObject Obj, const ClientPlayer* Player)->NearestEntityObject
	{
		if (Player == Local)
		{
			return Obj;
		}

		if (Player->TeamId != Local->TeamId)
		{
			if ((Behavior.Targets & TargetEnemies) != TargetEnemies)
			{
				return Obj;
			}
		}
		else
		{
			if ((Behavior.Targets & TargetAllies) != TargetAllies)
			{
				return Obj;
			}
		}

		ClientSoldierEntity* Soldier = Player->Soldier;

		if (!IsValidPointer(Soldier) || Soldier->IsOccluded)
		{
			return Obj;
		}

		if ((Behavior.Targets & TargetSoldiers) != TargetSoldiers)
		{
			return Obj;
		}

		ClientStatusComponent* Status = Soldier->ClientStatusComponent;

		if (!IsValidPointer(Status) || Status->IsDead())
		{
			return Obj;
		}

		if (!Soldier->VisibleBones)
		{
			return Obj;
		}

		auto Position3D = [&]() -> std::optional<Vector3f>
		{
			for (size_t N = 0; N < Behavior.TargetBones.size(); ++N)
			{
				if (auto Iter = std::find(RcBehavior.Bones.begin(), RcBehavior.Bones.end(), Behavior.TargetBones[N]); Iter != RcBehavior.Bones.end())
				{
					if ((Soldier->VisibleBones & (1 << std::distance(RcBehavior.Bones.begin(), Iter))) != 0)
					{
						if (auto Pos = Soldier->GetBonePosition(Behavior.TargetBones[N]))
						{
							return Pos;
						}
					}
				}
			}

			return {};
		}();

		if (!Position3D)
		{
			return Obj;
		}

		Vector3f Position2D;

		if (!Renderer->WorldToScreen(*Position3D, Position2D, Behavior.MaxDistanceScreen * PI / 90.f))
		{
			return Obj;
		}

		const float Distance2D = Math::Abs(Viewport.X - Position2D.X);
		const float Distance3D = Vector3f::Distance(Position, *Position3D);

		if (Distance3D > Behavior.MaxDistanceWorld[1] ||
			Distance3D < Behavior.MaxDistanceWorld[0])
		{
			return Obj;
		}

		const auto* LastEntity = AimCurr.TargetObject.Entity ? AimCurr.TargetObject.Entity : AimLast.TargetObject.Entity;

		const float Insignificance =
			(Distance3D * Behavior.Weights.D3DistanceImportanceWeight * 0.1f +
				Distance2D * Behavior.Weights.D2DistanceImportanceWeight) *
			(Soldier == LastEntity ? 1.f / Math::Max(Behavior.Weights.PreviousTargetWeightMultiplier, SMALL_NUMBER) : 1.f);

		if (Obj.Insignificance > Insignificance)
		{
			const auto* Pred = Soldier->GetPrediction();

			if (IsValidPointer(Pred))
			{
				return { Soldier, Distance3D, Math::Abs(Position2D.X - Viewport.X), Insignificance, *Position3D, Pred->Velocity };
			}
			else
			{
				return { Soldier, Distance3D, Math::Abs(Position2D.X - Viewport.X), Insignificance, *Position3D };
			}
		}

		return Obj;
	});
	
	if (Result.Entity)
	{
		return std::make_optional(Result);
	}
	
	return {};
}

void Aimbot::Execute(InputController& Ic)
{
	const ClientPlayer * Local = Crypto::GetLocalPlayer();

	if (!IsValidPointer(Local) || Local->InVehicle())
	{
		return;
	}

	const ClientSoldierEntity * LocalSoldier = Local->Soldier;

	if (!IsValidPointer(LocalSoldier))
	{
		return;
	}

	auto Sp = LocalSoldier->GetShootSpaceMatrix();
	if (!Sp)
	{
		return;
	}

	const Matrix4x4 Shootspace = *Sp;
	const Vector3f LocalPosition = Shootspace.GetOrigin();

	if (auto Entity = BestEntity(Local, LocalPosition))
	{
		auto NearestEntity = *Entity;

		if (auto ProjAttr = ProjectileAttr::FromEntity(LocalSoldier))
		{
			Vector3f ShootPosition = LocalPosition;

			if (auto Vo = Ic.GetViewOffset())
			{
				ShootPosition += *Vo - Local->Soldier->SoldierPhysicsComponent->ViewOffset;
			}

			const auto Estimation = Sim.CalculateHeightOffset({ *ProjAttr }, LocalPosition, NearestEntity.Position, NearestEntity.Velocity, 32);
			
			NearestEntity.Position += Vector3f::UpVector * Estimation.HeightOffset +
				Estimation.Time * Vector3f(NearestEntity.Velocity.X, 0.0f, NearestEntity.Velocity.Z);

			if (Behavior.Silent)
			{
				Ic.SetLookAt(std::make_optional(NearestEntity.Position));
			}
			else
			{
				const float Now = ClientGameContext::Instance()->GameTimeMs();

				if (AimCurr.TargetObject != NearestEntity)
				{
					auto [Curve, CurvePos] = Behavior.SelectCurve(NearestEntity.Distance2D);
					AimCurr = { NearestEntity, Curve, std::move(CurvePos), Estimation.Time };
					LastExecutionTime = Now;
				}
				else
				{
					AimCurr.Update(Now - LastExecutionTime, NearestEntity);
				}

				if (AimCurr.Cv)
				{
					const CurveTransform Trans = { Behavior.AimSpeed(Ic) * (1.f + NearestEntity.Velocity.Size() * 0.5f), 1.0f };

					AimCurr.CvData = AimCurr.Cv->Evaluate(
						std::move(AimCurr.CvData),
						Behavior.Constants,
						NearestEntity.Position,
						Shootspace,
						Now - LastExecutionTime,
						Trans
					);

					Ic.ApplyViewParameters(AimCurr.CvData.DeltaDirection);
				}
			}
		}
	}
	else if (AimCurr.HasTarget())
	{
		Stop(Ic);
	}

}

void Aimbot::Stop(InputController& Ic)
{
	if (Behavior.Silent)
	{
		Ic.SetLookAt({});
	}

	AimLast = std::move(AimCurr);
	AimCurr.TargetObject = {};
}

Triggerbot::Triggerbot(const RaycastBehavior& RcBehavior, const TriggerBehavior& Behavior) noexcept
	: RcBehavior{ RcBehavior }
	, Behavior{ Behavior }
	, LastTarget{ nullptr }
{}

const PhysicsEntityBase* Triggerbot::BestEntity(const ClientPlayer* Local, const Vector3f& Position, const Vector3f& LookAt)
{
	const auto* Mgr = CryptoManager::Instance()->PlayerManager();

	if (!Mgr)
	{
		return nullptr;
	}

	const auto Iter = std::find_if(Mgr->begin(), Mgr->end(), [&](const ClientPlayer* Player)
	{
		if (!IsValidPointer(Player) ||
			!IsValidPointer(Player->Soldier))
		{
			return false;
		}

		if (Player->TeamId == Local->TeamId)
		{
			return false;
		}

		if (Player->Soldier->IsOccluded)
		{
			return false;
		}

		if (Player->InVehicle())
		{
			return false;
		}

		if (!IsValidPointer(Player->Soldier->ClientStatusComponent) ||
			Player->Soldier->ClientStatusComponent->IsDead())
		{
			return false;
		}

		const AABBTransform Transform = Player->Soldier->AABB();
		const Vector3f Center = Transform.Transform.GetOrigin();
		const float Distance3d = Vector3f::Distance(Position, Center);

		if (Distance3d < Behavior.MaxDistanceWorld[0] ||
			Distance3d > Behavior.MaxDistanceWorld[1])
		{
			return false;
		}

		const Vector4f Q = Vector4f((LookAt - Center).GetAbs()) - 0.5f * (Transform.AABB.Max - Transform.AABB.Min);
		const float Sd = Q.Max(0.f).Size3() + Math::Min(Math::Max(Q.X, Math::Max(Q.Y, Q.Z)), 0.f);

		if (Sd > 1.0f)
		{
			return false;
		}

		return true;
	});

	if (Iter == Mgr->end())
	{
		return nullptr;
	}

	const auto* Result = *Iter;

	if (Result->InVehicle())
	{
		return Result->Vehicle;
	}

	return Result->Soldier;
}

void Triggerbot::Evaluate(InputController& Ic)
{
	const auto* Local = Crypto::GetLocalPlayer();

	if (!IsValidPointer(Local) ||
		!IsValidPointer(Local->Soldier))
	{
		return;
	}

	const auto* Soldier = Local->Soldier;

	if (Soldier->ClientStatusComponent->IsDead())
	{
		return;
	}

	const auto LocalPos = Soldier->GetShootSpace();

	if (!LocalPos)
	{
		return;
	}

	const auto* Target = BestEntity(Local, *LocalPos, Soldier->AimEntity->LookAt);

	if (Target)
	{
		LastTarget = Target;
		Ic.SetFireState(true);
	}
	else
	{
		Stop(Ic);
	}
}

void Triggerbot::Stop(InputController& Ic)
{
	Ic.SetFireState(false);
	LastTarget = nullptr;
}