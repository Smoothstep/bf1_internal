#include "stdafx.h"
#include "AutoSpot.h"
#include "Crypto.h"
#include "ClientGame.h"

struct SpottingDetail
{
	float LastSpottingTime;
	uint32_t SpottingCycle;
	uint32_t LastPlayerIdx;

	constexpr SpottingDetail() noexcept
		: LastSpottingTime{ 0 }
		, SpottingCycle{ 0 }
		, LastPlayerIdx{ 0 }
	{}
};

AutoSpot::AutoSpot()
	: Detail{ std::make_unique<SpottingDetail>() }
{}

AutoSpot::~AutoSpot()
{}

void AutoSpot::Execute(const SpottingBehavior& Behavior)
{
	float Now = ClientGameContext::Instance()->GameTimeMs();

	if (Now - Detail->LastSpottingTime < Behavior.Delay)
	{
		return;
	}

	auto* Local = Crypto::GetLocalPlayer();
	
	if (!IsValidPointer(Local) ||
		!IsValidPointer(Local->Soldier) || 
		Local->Soldier->ClientStatusComponent->IsDead())
	{
		return;
	}

	auto*SpottingComponent = Local->Soldier->GetSpottingComponent();
	if (!SpottingComponent)
	{
		return;
	}

	eastl::FixedVector<PhysicsEntityBase*, 8> Entities {};
	uint32_t Counter = 0;

	Detail->LastPlayerIdx += Crypto::IteratePlayers([&](ClientPlayer* Player)
	{
		if (++Counter < Detail->LastPlayerIdx)
		{
			return true;
		}

		if (Player->TeamId != Local->TeamId && !Player->InVehicle())
		{
			if (IsValidPointer(Player->Soldier) && !Player->Soldier->ClientStatusComponent->IsDead())
			{
				Entities.Push(Player->Soldier);
			}
		}

		return Entities.Size() != 8;
	}, Detail->LastPlayerIdx);

	if (Entities.Size() < 8)
	{
		Detail->LastPlayerIdx = 0;
		Detail->SpottingCycle++;
	}

	if (Entities.Size() > 0)
	{
		SpottingComponent->Spot(Local, 1, &Entities, nullptr);
	}

	Detail->LastSpottingTime = Now;
}
