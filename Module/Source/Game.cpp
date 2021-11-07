#include "stdafx.h"
#include "Game.h"
#include "Hooks.h"
#include "Aimbot.h"
#include "Crypto.h"
#include "ClientPlayer.h"
#include "InputController.h"

bool GameRenderer::WorldToScreen(const Vector3f& WorldPosition, Vector3f& ScreenPosition) const
{
	if (!IsValidPointer(RenderView))
	{
		return false;
	}

	const Vector4f Position3d = Vector4f(WorldPosition, 1.0f);

	const auto Viewport = DxRenderer::Instance()->Screen->GetViewport();

	const float ScreenHX = 0.5f * Viewport.X;
	const float ScreenHY = 0.5f * Viewport.Y;

	const Matrix4x4& Vpm = RenderView->ViewProjectionTransposed;

	ScreenPosition.Z = Vpm[3] | Position3d;

	if (ScreenPosition.Z < 0.1f)
	{
		return false;
	}

	const float X = (Vpm[0] | Position3d) / ScreenPosition.Z;
	const float Y = (Vpm[1] | Position3d) / ScreenPosition.Z;

	ScreenPosition.X = ScreenHX + ScreenHX * X;
	ScreenPosition.Y = ScreenHY - ScreenHY * Y;
	ScreenPosition.Y = Viewport.Y - ScreenPosition.Y;

	return true;
}

bool GameRenderer::WorldToScreen(const Vector3f& WorldPosition, Vector3f& ScreenPosition, const float Fov) const
{
	if (!IsValidPointer(RenderView))
	{
		return false;
	}

	const Vector4f Position3d = Vector4f(WorldPosition, 1.0f);

	const auto Viewport = DxRenderer::Instance()->Screen->GetViewport();

	const float ScreenHX = 0.5f * Viewport.X;
	const float ScreenHY = 0.5f * Viewport.Y;

	const Matrix4x4& Vpm = RenderView->ViewProjectionTransposed;

	ScreenPosition.Z = Vpm[3] | Position3d;

	if (ScreenPosition.Z < 0.1f)
	{
		return false;
	}

	const float X = (Vpm[0] | Position3d) / ScreenPosition.Z;
	const float Y = (Vpm[1] | Position3d) / ScreenPosition.Z;

	const float A0 = Math::Tan(Fov * 0.5f);
	const float AX = Math::Tan(RenderView->Fov() * 0.5f);
	const float AY = Math::Tan(Math::Atan(AX * RenderView->Aspect()));

	const float X0 = A0 / AX;
	const float Y0 = A0 / AY;

	if (Math::Abs(X) > X0 ||
		Math::Abs(Y) > Y0)
	{
		return false;
	}

	ScreenPosition.X = ScreenHX + ScreenHX * X;
	ScreenPosition.Y = ScreenHY - ScreenHY * Y;
	ScreenPosition.Y = Viewport.Y - ScreenPosition.Y;

	return true;
}