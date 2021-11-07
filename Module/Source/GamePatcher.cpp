#include "stdafx.h"
#include "GamePatcher.h"
#include "GameHooks.h"
#include "Offsets.h"

GamePatcher::MemState::MemState(IMemoryInterface * MemoryInterface,
								std::vector<uint8_t>&& MemorySrc,
								void * MemoryDst)
	: Toggled(false)
	, MemoryTarget(MemoryDst)
	, MemoryAccessMutex()
	, MemoryPatch(std::move(MemorySrc))
	, MemoryInterface(MemoryInterface)
{
	MemoryBackup.resize(MemoryPatch.size());

	if (IsValidPointer(MemoryDst))
	{
		if (std::memcmp(MemoryPatch.data(), MemoryDst, MemoryPatch.size()) == 0)
		{
			Toggled = true;
		}

		memcpy(MemoryBackup.data(), MemoryDst, MemoryBackup.size());
	}
}

GamePatcher::MemState::~MemState()
{
	if (Toggled)
	{
		MemoryInterface->WriteMemory(MemoryTarget, MemoryBackup.data(), MemoryBackup.size());
	}
}

bool GamePatcher::MemState::Toggle(const ToggleState State)
{
	std::lock_guard Guard(MemoryAccessMutex);

	if (State.State != EToggleStates::Toggle_Revert && State == Toggled)
	{
		return true;
	}

	const bool Written = MemoryInterface->WriteMemory(MemoryTarget, State ? MemoryPatch.data() : MemoryBackup.data(), MemoryBackup.size()) != nullptr;

	if (Written)
	{
		switch (State.State)
		{
			case EToggleStates::Toggle_Revert:
			{
				Toggled = !Toggled;
				break;
			}
			case EToggleStates::Toggle_On:
			{
				Toggled = true;
				break;
			}
			default:
			{
				Toggled = false;
				break;
			}
		}
	}

	return Written;
}

GamePatcher::GamePatcher(IMemoryInterface& MemoryInterface)
	: NoBreath(&MemoryInterface, { 0xB0, 0x03, 0x90, 0x90 }, CastPtr(EFunctionOffsets::Offset_NoBreath))
	, NoRecoil(&MemoryInterface, { 0xE9, 0x14, 0x01, 0x00, 0x00, 0x90 }, CastPtr(EFunctionOffsets::Offset_NoRecoil))
	, NoSpread(&MemoryInterface, { 0xF3, 0x0F, 0x10, 0x30, 0x90, 0x90, 0x90, 0x90 }, CastPtr(EFunctionOffsets::Offset_NoSpread))
	, NoSS(&MemoryInterface, {}, 0)
	, Esp(&MemoryInterface, {}, 0)
	, Radar(&MemoryInterface, { 0x90, 0x90 }, CastPtr(EFunctionOffsets::Offset_Radar))
	, SpottingNoFOV(&MemoryInterface, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, CastPtr(EFunctionOffsets::Offset_SpottingFoV))
	, SpottingAlwaysActive(&MemoryInterface, { 0xC7, 0x85, 0x38, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00 }, CastPtr(EFunctionOffsets::Offset_SpottingAlways))
	, SpottingNoTeam(&MemoryInterface, { 0x90, 0x90 }, CastPtr(EFunctionOffsets::Offset_SpottingTeam))
	, SpottingNoOccluded(&MemoryInterface, { 0x90, 0x90 }, CastPtr(EFunctionOffsets::Offset_SpottingOccluded))
	, SpottingNoVisibility(&MemoryInterface, { 0xEB, 0x2B }, CastPtr(EFunctionOffsets::Offset_SpottingVisibility))
	, TerrainCulling(&MemoryInterface, { 0xEB }, CastPtr(EFunctionOffsets::Offset_CullTerrain))
	, Culling(&MemoryInterface, { 0x41, 0xBC, 0x39, 0x00, 0x00, 0x00, 0x90 }, CastPtr(EFunctionOffsets::Offset_Cull))
	, CullingCheck(&MemoryInterface, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, CastPtr(EFunctionOffsets::Offset_CullCheck))
	, BipodAlwaysSet(&MemoryInterface, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, CastPtr(EFunctionOffsets::Offset_BipodWhileMoving))
	, BipodEverywhere(&MemoryInterface, { 0x90, 0x90 }, CastPtr(EFunctionOffsets::Offset_BipodEverywhere))
	, BipodAllWeapons(&MemoryInterface, { 0x90, 0x90 }, CastPtr(EFunctionOffsets::Offset_BipodEveryWeapon))
	, ThirdPerson(&MemoryInterface, { 0xEB }, CastPtr(EFunctionOffsets::Offset_ThirdPersonBody))
	, ThirdPersonCheck(&MemoryInterface, { 0xEB }, CastPtr(0x00000001413E0392))
	, ThirdPersonRay(&MemoryInterface, { 0xEB }, CastPtr(EFunctionOffsets::Offset_LookAtRay))
	, ThirdPersonCameraTransform(&MemoryInterface, { 0xE9, 0x59, 0x01, 0x00, 0x00, 0x90 }, CastPtr(EFunctionOffsets::Offset_ThirdPersonCameraTransform))
	, NoViewShift(&MemoryInterface, { 0xEB }, CastPtr(EFunctionOffsets::Offset_BodyTransform))
	, ZoomCrosshair(&MemoryInterface, { 0x32, 0xC0, 0x90, 0x90, 0x90 }, CastPtr(EFunctionOffsets::Offset_CrosshairWhenZoomedIn))
{
	DisableInvalidMemoryAccess(MemoryInterface);
}

void GamePatcher::ToggleNoRecoil(ToggleState State)
{
	NoRecoil.Toggle(State);
}

void GamePatcher::ToggleBipodAlwaysSet(ToggleState State)
{
	BipodAlwaysSet.Toggle(State);
}

void GamePatcher::ToggleBipodEverywhere(ToggleState State)
{
	BipodEverywhere.Toggle(State);
}

void GamePatcher::ToggleBipodAllWeapons(ToggleState State)
{
	BipodAllWeapons.Toggle(State);
}

void GamePatcher::ToggleThirdPerson(ToggleState State)
{
	NoViewShift.Toggle(State);
	ThirdPersonRay.Toggle(true);
	ThirdPerson.Toggle(State);
	ThirdPersonCameraTransform.Toggle(State);
	ZoomCrosshair.Toggle(State);

	if (State)
	{
		TemporaryHooks.Init<ViewOffsetHook>();
	}
	else
	{
		TemporaryHooks.Unhook<ViewOffsetHook>();
	}
}

void GamePatcher::ToggleNoSpread(ToggleState State)
{
	NoSpread.Toggle(State);
}

void GamePatcher::ToggleNoBreath(ToggleState State)
{
	NoBreath.Toggle(State);
}

void GamePatcher::Toggle3DEsp(ToggleState State)
{
	Esp.Toggle(State);
}

void GamePatcher::ToggleRadar(ToggleState State)
{
	Radar.Toggle(State);
}

void GamePatcher::ToggleNoSS(ToggleState State)
{
	NoSS.Toggle(State);
}

void GamePatcher::ToggleSpottingFOV(ToggleState State)
{
	SpottingNoFOV.Toggle(State);
}

void GamePatcher::ToggleSpottingTeam(ToggleState State)
{
	SpottingNoTeam.Toggle(State);
}

void GamePatcher::ToggleSpottingAlwaysActive(ToggleState State)
{
	SpottingAlwaysActive.Toggle(State);
}

void GamePatcher::ToggleSpottingOccluded(ToggleState State)
{
	SpottingNoOccluded.Toggle(State);
}

void GamePatcher::ToggleSpottingVisiblity(ToggleState State)
{
	SpottingNoVisibility.Toggle(State);
}

void GamePatcher::ToggleTerrainCulling(ToggleState State)
{
	Culling.Toggle(State);
	CullingCheck.Toggle(State);
	TerrainCulling.Toggle(State);
}

void GamePatcher::DisableInvalidMemoryAccess(IMemoryInterface& Interface)
{
	uint8_t PatchByte = 0xEB;
	Interface.WriteMemory(reinterpret_cast<void*>(EFunctionOffsets::Offset_TwinkleCrash), &PatchByte, 1);
}