#include "stdafx.h"
#include "Globals.h"
#include "Crypto.h"
#include "ClientGame.h"
#include "ClientPlayer.h"
#include "Aimbot.h"
#include "Hooks.h"
#include "Game.h"
#include "GamePatcher.h"
#include "GameHooks.h"
#include "Utility.h"
#include "InputController.h"

#include "dxgi9.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

class Stub
{
public:
	Stub(HMODULE ThisModule)
	{
		DebugConsole();

		char Path[1024];
		{
			GetModuleFileNameA(ThisModule, Path, sizeof(Path));
		}

		Globals::BaseDirectory = std::filesystem::path(Path).parent_path();

		while (
			!DxRenderer::Instance() &&
			!DxRenderer::Instance()->Context)
		{
			Logger::Log() << "Waiting for DxRenderer to finish initialization ..." << Logger::EndLine;
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		InitHooks();
	}

private:
	void InitHooks()
	{
		Hooks.Init<InputCacheHook>(GameInterface.GetInputCtrl());
		Hooks.Init<RenderFunctionDispatcherHook>();
		Hooks.Init<UpdatePositionHook>(GameInterface);
		Hooks.Init<PresentHook>(GameInterface);
		Hooks.Init<ViewAnglesHook>(GameInterface.GetInputCtrl());
		Hooks.Init<AimPitchHook>(GameInterface.GetInputCtrl());
		Hooks.Init<AimYawHook>(GameInterface.GetInputCtrl());
		Hooks.Init<GetMessageHook>(GameInterface);
		Hooks.Init<SetupEntityRendererHook>(GameInterface);
		Hooks.Init<LightingRenderPass>(GameInterface);
		Hooks.Init<SetupRenderDynamicHook>(GameInterface);
		Hooks.Init<BitBltHook>(GameInterface);
		Hooks.Init<SetupRenderStaticHook>(GameInterface);
		Hooks.Init<MergeRenderHook>(GameInterface);
		Hooks.Init<ShootspaceUpdateHook>(GameInterface.GetInputCtrl());
	}

	HookDb Hooks;
	D3D::GameInterface GameInterface;
};

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
		case DLL_PROCESS_ATTACH:
		{
			try
			{
				static Stub Main{ hModule };
			}
			catch (const std::exception& Ex)
			{
				Logger::Error() << "Failed to initialize stub " << Ex.what() << Logger::EndLine;
			}

			break;
		}
		
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}