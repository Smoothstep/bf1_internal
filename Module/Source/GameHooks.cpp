#include "stdafx.h"
#include "GameHooks.h"
#include "RenderHooks.h"
#include "Aimbot.h"
#include "Game.h"
#include "ClientPlayer.h"
#include "Projectile.h"
#include "D3DHooks.h"
#include "InputController.h"
#include "Instruction.h"
#include "D3DHooks.h"
#include "D3DRenderer.h"
#include "Cleaner.h"

extern "C" 
{
	extern void HkSetViewAngles(); 
	extern void HkSetShootspace();
	uint64_t ViewAnglesHookCtrl;
	uint64_t ViewAnglesHookJmpBack;
	uint64_t SetShootspaceJmpBack;
	uint64_t SetShootspaceVa;
}

SetViewParametersHook::SetViewParametersHook(HookTag<>&& Tag)
	: DetourHook(
		CastPtr(EFunctionOffsets::Offset_SetViewParameters),
		CastPtr(&RenderViewLayout::CopyFrom))
{
	if (!InitHook(Tag.Unwrap(), { 14 }))
	{
		throw std::runtime_error("SetViewParametersHook: Initialization failed");
	}
}

UpdatePositionHook::UpdatePositionHook(HookTag<D3D::GameInterface&>&& Tag)
	: VMTHook(
		CastPtr(EVMTOffsets::Offset_CSE_VMT),
		CastPtr(&ClientSoldierEntity::UpdatePosition),
		EVMTFunctionsClientPlayer::Index_UpdatePosition)
{
	if (!InitHook(Tag.Unwrap()))
	{
		throw std::runtime_error("UpdatePositionHook: Initialization failed");
	}
}

SetCustomViewHook::SetCustomViewHook(HookTag<>&& Tag)
	: VMTHook(
		CastPtr(EVMTOffsets::Offset_RVP_VMT),
		CastPtr(&RenderViewParams::CustomLayout),
		EVMTFunctionsRenderViewParams::Index_GetView)
{
	if (!InitHook(Tag.Unwrap()))
	{
		throw std::runtime_error("SetCustomViewHook: Initialization failed.");
	}
}

UpdateProjectileHook::UpdateProjectileHook(HookTag<>&& Tag)
	: VMTHook(
		CastPtr(EVMTOffsets::Offset_BLT_VMT),
		CastPtr(&Projectile::ComputePosition),
		EVMTFunctionsProjectile::Index_UpdatePosition)
{
	if (!InitHook(Tag.Unwrap()))
	{
		throw std::runtime_error("UpdateProjectileHook: Initialization failed");
	}
}

InputCacheHook::InputCacheHook(HookTag<InputController&>&& Tag)
	: VMTHook(
		CastPtr(EVMTOffsets::Offset_INP_VMT),
		CastPtr(&InputController::InputCache::GetInput),
		EVMTFunctionsInput::Index_GetInput)
{
	if (!InitHook(Tag.Unwrap()))
	{
		throw std::runtime_error("InputCacheHook: Initialization failed");
	}
}

GetMessageHook::GetMessageHook(HookTag<D3D::GameInterface&>&& Tag)
	: DetourHook(
		CastPtr(GetProcAddress(GetModuleHandleA("USER32"), "GetMessageA")),
		CastPtr(&D3D::GameInterface::GetMessageHk))
{
	if (!InitHook(Tag.Unwrap(), { 15 }))
	{
		throw std::runtime_error("GetMessageHook: Initialization failed");
	}
}

PresentHook::PresentHook(HookTag<D3D::GameInterface&>&& Tag)
	: VMTHook(
		reinterpret_cast<uintptr_t**>(std::get<0>(Tag.Unwrap()).GetSwapChain())[0],
		CastPtr(&D3D::GameInterface::Present),
		EVMTFunctionsSwapchain::Index_Present)
{
	if (!InitHook(Tag.Unwrap(), {}))
	{
		throw std::runtime_error("PresentHook: Initialization failed");
	}
}

SetupRenderDynamicHook::SetupRenderDynamicHook(HookTag<D3D::GameInterface&>&& Tag)
	: DetourHook(
		CastPtr(EFunctionOffsets::Offset_InitializeEntityRenderPipeline),
		CastPtr(&D3D::MeshRenderer::PrepareDynamic))
{
	if (!InitHook(Tag.Unwrap(), { 14 }))
	{
		throw std::runtime_error("SetupRenderDynamicHook: Initialization failed");
	}
}

SetupRenderStaticHook::SetupRenderStaticHook(HookTag<D3D::GameInterface&>&& Tag)
	: DetourHook(
		CastPtr(EFunctionOffsets::Offset_InitializeVehicleRenderPipeline),
		CastPtr(&D3D::MeshRenderer::PrepareStatic))
{
	if (!InitHook(Tag.Unwrap(), { 14 }))
	{
		throw std::runtime_error("SetupRenderStaticHook: Initialization failed");
	}
}

LightingRenderPass::LightingRenderPass(HookTag<D3D::GameInterface&>&& Tag)
	: DetourHook(
		CastPtr(EFunctionOffsets::Offset_PBRLight),
		CastPtr(&D3D::FbRenderer::RenderLighting))
{
	if (!InitHook(Tag.Unwrap(), { 20 }))
	{
		throw std::runtime_error("LightingRenderPass: Initialization failed");
	}
}

RenderSoldierPrimitiveHook::RenderSoldierPrimitiveHook(HookTag<D3D::GameInterface&>&& Tag)
	: VMTHook(
		CastPtr(EVMTOffsets::Offset_RCE_VMT),
		CastPtr(&D3D::SoldierRenderer::SoldierEntityRenderPrimitive),
		EVMTFunctionsSoldierEntityRenderer::Index_RenderPrimitive
	)
{
	if (!InitHook(Tag.Unwrap(), {}))
	{
		throw std::runtime_error("RenderSoldierPrimitiveHook: Initialization failed");
	}
}

SetupEntityRendererHook::SetupEntityRendererHook(HookTag< D3D::GameInterface&>&& Tag)
	: DetourHook(
		CastPtr(EFunctionOffsets::Offset_PrepareEntityRenderer),
		CastPtr(&D3D::EntityRenderer::AddRenderTask)
	)
{
	if (!InitHook(Tag.Unwrap(), { 15 }))
	{
		throw std::runtime_error("SetupEntityRendererHook: Initialization failed");
	}
}

RenderFunctionDispatcherHook::RenderFunctionDispatcherHook(HookTag<>&& Tag)
	: DetourHook(
		CastPtr(EFunctionOffsets::Offset_DispatchFunctions),
		CastPtr(&RenderFunctionDispatcher::DispatchAll))
{
	if (!InitHook(Tag.Unwrap(), { 15 }))
	{
		throw std::runtime_error("RenderFunctionDispatcherHook: Initialization failed");
	}
}

ViewAnglesHook::ViewAnglesHook(HookTag<InputController&>&& Tag)
	: IntermediateHook(
		CastPtr(EFunctionOffsets::Offset_ViewAngles),
		CastPtr(&HkSetViewAngles))
{
	auto Params = Tag.Unwrap();

	ViewAnglesHookCtrl = reinterpret_cast<uint64_t>(&std::get<0>(Params));
	ViewAnglesHookJmpBack = reinterpret_cast<uint64_t>(GetOriginalPtr()) + 16;

	if (!InitHook(std::move(Params), { 16 }))
	{
		throw std::runtime_error("ViewAnglesHook: Initialization failed");
	}
}

MergeRenderHook::MergeRenderHook(HookTag<D3D::GameInterface&>&& Tag)
	: DetourHook(
		CastPtr(EFunctionOffsets::Offset_RenderMerge),
		CastPtr(&D3D::FbRenderer::RenderMerge))
{
	if (!InitHook(Tag.Unwrap(), { 15 }))
	{
		throw std::runtime_error("MergeRenderHook: Initialization failed");
	}
}

ShootspaceUpdateHook::ShootspaceUpdateHook(HookTag<InputController&>&& Tag)
	: IntermediateHook(
		CastPtr(EFunctionOffsets::Offset_UpdateShootspace),
		CastPtr(&HkSetShootspace))
{
	auto Params = Tag.Unwrap();

	ViewAnglesHookCtrl = reinterpret_cast<uint64_t>(&std::get<0>(Params));

	auto SetShootspace = &ClientWeapon::SetShootspace;
	SetShootspaceVa = *reinterpret_cast<uint64_t*>(&SetShootspace);
	SetShootspaceJmpBack = reinterpret_cast<uint64_t>(CastPtr(EFunctionOffsets::Offset_UpdateShootspace)) + 15;

	if (!InitHook(std::move(Params), { 15 }))
	{
		throw std::runtime_error("ShootspaceUpdateHook: Initialization failed");
	}
}

ViewOffsetHook::ViewOffsetHook(HookTag<>&& Tag)
	: VMTHook(
		CastPtr(EVMTOffsets::Offset_PHS_VMT),
		CastPtr(&BFClientSodlierPhysicsComponent::GetViewOffsetThirdPerson),
		EVMTFunctionsSoldierPhysics::Index_ViewOffset
	)
{
	if (!InitHook(Tag.Unwrap()))
	{
		throw std::runtime_error("ViewOffsetHook: Initialization failed");
	}
}

BitBltHook::BitBltHook(HookTag<D3D::GameInterface&>&& Tag)
	: DetourHook(
		CastPtr(&BitBlt),
		CastPtr(&ScreenshotCleaner::HkBitBlt))
{
	if (!InitHook(Tag.Unwrap(), { 15 }))
	{
		throw std::runtime_error("BitBltHook: Initialization failed");
	}
}

AimYawHook::AimYawHook(HookTag<InputController&>&& Tag)
	: VMTHook(
		CastPtr(EVMTOffsets::Offset_AIM_VMT),
		CastPtr(&ClientSoldierAimingSimulation::GetYaw),
		EVMTAiming::Offset_GetYaw
	)
{
	if (!InitHook(Tag.Unwrap()))
	{
		throw std::runtime_error("AimYawHook: Initialization failed");
	}
}

AimPitchHook::AimPitchHook(HookTag<InputController&>&& Tag)
	: VMTHook(
		CastPtr(EVMTOffsets::Offset_AIM_VMT),
		CastPtr(&ClientSoldierAimingSimulation::GetPitch),
		EVMTAiming::Offset_GetPitch
	)
{
	if (!InitHook(Tag.Unwrap()))
	{
		throw std::runtime_error("AimPitchHook: Initialization failed");
	}
}
