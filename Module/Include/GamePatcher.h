#pragma once

#include "MemoryInterface.h"
#include "Hooks.h"
#include <vector>
#include <thread>
#include <optional>

enum class EToggleStates : int8_t
{
	Toggle_Off,
	Toggle_On,
	Toggle_Revert
};

struct ToggleState
{
	constexpr ToggleState(const bool State) noexcept
		: State(State ? EToggleStates::Toggle_On : EToggleStates::Toggle_Off) {}

	constexpr ToggleState(const EToggleStates State = EToggleStates::Toggle_Off) noexcept
		: State(State) {}

	constexpr operator bool() const
	{
		return State == EToggleStates::Toggle_On;
	}

	EToggleStates State;
};

class GamePatcher
{
	class MemState
	{
	public:
		MemState(IMemoryInterface * MemoryInterface,
				 std::vector<uint8_t>&& MemorySrc,
				 void * MemoryDst);
		MemState() = delete;
		~MemState();

		bool Toggle(const ToggleState State);

	private:
		bool Toggled;
		void* MemoryTarget;
		std::mutex MemoryAccessMutex;
		std::vector<uint8_t> MemoryBackup;
		std::vector<uint8_t> MemoryPatch;
		IMemoryInterface * MemoryInterface;
	};

public:
	GamePatcher(IMemoryInterface& MemoryInterface);

	void ToggleBipodAlwaysSet(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleBipodEverywhere(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleBipodAllWeapons(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleThirdPerson(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleNoRecoil(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleNoSpread(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleNoBreath(ToggleState State = EToggleStates::Toggle_Revert);
	void Toggle3DEsp(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleRadar(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleNoSS(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleSpottingFOV(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleSpottingTeam(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleSpottingAlwaysActive(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleSpottingOccluded(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleSpottingVisiblity(ToggleState State = EToggleStates::Toggle_Revert);
	void ToggleTerrainCulling(ToggleState State = EToggleStates::Toggle_Revert);

	void DisableInvalidMemoryAccess(IMemoryInterface& MemoryInterface);

private:
	MemState NoBreath;
	MemState NoRecoil;
	MemState NoSpread;
	MemState NoSS;
	MemState Esp;
	MemState Radar;
	MemState SpottingNoFOV;
	MemState SpottingAlwaysActive;
	MemState SpottingNoTeam;
	MemState SpottingNoOccluded;
	MemState SpottingNoVisibility;
	MemState TerrainCulling;
	MemState Culling;
	MemState CullingCheck;
	MemState BipodAlwaysSet;
	MemState BipodEverywhere;
	MemState BipodAllWeapons;
	MemState ThirdPerson;
	MemState ThirdPersonCheck;
	MemState ThirdPersonRay;
	MemState ThirdPersonCameraTransform;
	MemState NoViewShift;
	MemState ZoomCrosshair;
	HookDb TemporaryHooks;
};