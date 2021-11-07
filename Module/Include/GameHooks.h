#pragma once

#include "Hooks.h"
#include "ClassDecl.h"
#include "RenderFunctions.h"

class InputCacheHook : public VMTHook
{
public:
	InputCacheHook(HookTag<InputController&>&&);
};

class UpdatePositionHook : public VMTHook
{
public:
	UpdatePositionHook(HookTag<D3D::GameInterface&>&&);
};

class SetCustomViewHook : public VMTHook
{
public:
	SetCustomViewHook(HookTag<>&&);
};

class UpdateProjectileHook : public VMTHook
{
public:
	UpdateProjectileHook(HookTag<>&&);
};

class SetViewParametersHook : public DetourHook
{
public:
	SetViewParametersHook(HookTag<>&&);
};

class GetMessageHook : public DetourHook
{
public:
	GetMessageHook(HookTag<D3D::GameInterface&>&& Gi);
};

class PresentHook : public VMTHook
{
public:
	PresentHook(HookTag<D3D::GameInterface&>&&);
};

class SetupRenderDynamicHook : public DetourHook
{
public:
	SetupRenderDynamicHook(HookTag<D3D::GameInterface&>&&);
};

class SetupRenderStaticHook : public DetourHook
{
public:
	SetupRenderStaticHook(HookTag<D3D::GameInterface&>&&);
};

class RenderSoldierPrimitiveHook : public VMTHook
{
public:
	RenderSoldierPrimitiveHook(HookTag<D3D::GameInterface&>&&);
};

class LightingRenderPass : public DetourHook
{
public:
	LightingRenderPass(HookTag<D3D::GameInterface&>&&);
};

class SetupEntityRendererHook : public DetourHook
{
public:
	SetupEntityRendererHook(HookTag<D3D::GameInterface&>&&);
};

class RenderFunctionDispatcherHook : public DetourHook
{
public:
	RenderFunctionDispatcherHook(HookTag<>&&);
};

class ViewAnglesHook : public IntermediateHook
{
public:
	ViewAnglesHook(HookTag<InputController&>&&);
};

class MergeRenderHook : public DetourHook
{
public:
	MergeRenderHook(HookTag<D3D::GameInterface&>&&);
};

class ShootspaceUpdateHook : public IntermediateHook
{
public:
	ShootspaceUpdateHook(HookTag<InputController&>&&);
};

class ViewOffsetHook : public VMTHook
{
public:
	ViewOffsetHook(HookTag<>&&);
};

class BitBltHook : public DetourHook
{
public:
	BitBltHook(HookTag<D3D::GameInterface&>&&);
};

class AimYawHook : public VMTHook
{
public:
	AimYawHook(HookTag<InputController&>&&);
};

class AimPitchHook : public VMTHook
{
public:
	AimPitchHook(HookTag<InputController&>&&);
};