#pragma once

#include <d3d11.h>

#include "ClassDecl.h"
#include "Offsets.h"

class RenderView
{
public:
	inline float Fov() const
	{
		return 2.0f * Math::Atan(1.0f / Projection[1][1]);
	}

	inline float Aspect() const
	{
		return Projection[1][1] / Projection[0][0];
	}

	Pad(0x2A0);
	Matrix4x4 ViewInv;
	Matrix4x4 ViewInvTransposed;
	Matrix4x4 View;
	Matrix4x4 Projection;
	Matrix4x4 Unk;
	Matrix4x4 ProjectionX;
	Matrix4x4 ProjectionInverse;
	Matrix4x4 ViewProjection; //0x0460
	Matrix4x4 ViewProjectionTransposed; //
	Matrix4x4 ViewProjectionInv; //
	Matrix4x4 ViewProjectionInvTransposed; //
}; //Size: 0x0880

struct RenderViewLayout // TwinkleOverlayManager
{
	void CopyFrom(void * Data);

	Vector4f unk0;
	Hyper::Matrix4x4 Transform;
	Hyper::Matrix4x4 Vpm;
	Vector4f unk1;
};

class RenderViewParams
{
	virtual void F0() = 0;
	virtual void F1() = 0;
	virtual void F2() = 0;
	virtual void F3() = 0;
	virtual void F4() = 0;

public:
	virtual RenderViewLayout * GetView() = 0;
	virtual RenderViewLayout * GetView_C(uint32_t, uint32_t) = 0;

	RenderViewLayout * CustomLayout(uint32_t, uint32_t);

	static void StartTracking(const Vector3f& Target);
	static void SetLookAt(const Vector3f& Position);
	static void SetViewMatrix(const Matrix4x4& Mat);
	static void StopTracking();

	Pad(0x208);
	RenderViewLayout* Layout;
};

class GameRenderer
{
	virtual void F0() = 0;

public:
	static GameRenderer * Instance()
	{
		return *reinterpret_cast<GameRenderer**>(EInstanceOffsets::Offset_GameRenderer);
	}

	bool WorldToScreen(const Vector3f& WorldPosition, Vector3f& ScreenPosition) const;
	bool WorldToScreen(const Vector3f& WorldPosition, Vector3f& ScreenPosition, const float Fov) const;

	Pad(0x58);
	class RenderView* RenderView; //0x0060
	Pad(0xEE24); //0x0068
	uint32_t GameState;
	Pad(0x1B0); //0x0068
	class Dx11RenderTarget* FinalRtGame;
	class Dx11Texture* FinalTextureGame;
	int32_t FinalTextureWidth;
	int32_t FinalTextureHeight;
	class Dx11RenderTarget* FinalRtUi;
	class Dx11Texture* FinalTextureUi;
}; //Size: 0x0458

class Screen
{
public:
	inline IntPoint GetViewport() const
	{
		return { Width, Height };
	}

	inline D3D11_VIEWPORT GetD3D11Viewport() const
	{
		return { 0.0f, 0.0f, static_cast<const float>(Width), static_cast<const float>(Height), 0.0f, 1.0f };
	}

	Pad(0x68); //0x0000
	int32_t Width; //0x0068
	int32_t Height; //0x006C
	Pad(0x210); //0x0070
	struct IDXGISwapChain * SwapChain;
	Pad(0x20);
	struct ID3D11RenderTargetView * BackBuffer[3];
};

class DxRenderer
{
public:
	static DxRenderer * Instance()
	{
		return *reinterpret_cast<DxRenderer**>(EInstanceOffsets::Offset_DxRenderer);
	}

	Pad(0x820);
	class Screen* Screen; //0x0820
	Pad(0x78);
	struct ID3D11Device * Device;
	struct ID3D11DeviceContext4 * Context;
};