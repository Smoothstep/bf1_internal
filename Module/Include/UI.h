#pragma once

#include <memory.h>
#include <Framework/JSHelpers.h>

#include "Shader.h"
#include "Lib/Ultralight.h"
#include "D3DHooks.h"

class GPUContext : public framework::PlatformGPUContextD3D11
{
public:
	GPUContext(D3D::GameInterface* Gi)
		: D3DRenderer(Gi)
	{}

	virtual ID3D11Device* device()
	{
		return D3DRenderer->GetDevice();
	}

	virtual ID3D11Device* device_original()
	{
		return D3DRenderer->GetDeviceOriginal();
	}

	virtual ID3D11DeviceContext* immediate_context()
	{
		return D3DRenderer->GetDeviceContext();
	}

	virtual ID3D11DeviceContext* immediate_context_original()
	{
		return D3DRenderer->GetDeviceContextOriginal();
	}

	virtual IDXGISwapChain* swap_chain()
	{
		return D3DRenderer->GetSwapChain();
	}

	virtual ID3D11RenderTargetView* render_target_view()
	{
		return D3DRenderer->GetRenderTarget();
	}

	virtual void set_scale(double scale)
	{
		Scale = scale;
	}

	virtual double scale() const
	{
		return Scale;
	}

	virtual void set_screen_size(uint32_t width, uint32_t height)
	{
		Width = width;
		Height = height;
	}

	virtual void screen_size(uint32_t& width, uint32_t& height)
	{
		width = Width;
		height = Height;
	}

private:
	D3D::GameInterface * D3DRenderer;

	double		Scale		= 1.0;
	uint32_t	Width		= 0;
	uint32_t	Height	= 0;
};

class Utility;

class UI : public ul::LoadListener
{
public:
	 UI(D3D::GameInterface* Gi);
	~UI();

	bool WindowAcceptsInput(int X, int Y) const;
	bool HandleMouseInput(ul::MouseEvent::Button Button, ul::MouseEvent::Type Type, int X, int Y, int Z = 0);
	bool HandleKeyboardInput(ul::KeyEvent::Type Type, WPARAM Key, LPARAM Params);
	
	void Update();

private:
	HRESULT Initialize();
	HRESULT InitializeResources();

	framework::JSValue SetValue(const framework::JSObject& Object, 
								const framework::JSArgs& Args);

	framework::JSValue GetValue(const framework::JSObject& Object, 
								const framework::JSArgs& Args);

	ul::RefPtr<ul::View>				View;
	ul::RefPtr<ul::Renderer>			Renderer;

	std::shared_ptr<Utility>			Utility;

	ComPtr<ID3D11ShaderResourceView>	WebShaderView;
	ComPtr<ID3D11RasterizerState>		CullNone, CullNone1;
	ComPtr<ID3D11SamplerState>			SamplerState;
	ComPtr<ID3D11BlendState>			BlendState, BlendState1;
	Shader<ID3D11PixelShader>			PixelShader;
	Shader<ID3D11VertexShader>			VertexShader;
	ComPtr<ID3D11Texture2D>				ViewTextureCPU;
	ComPtr<ID3D11ShaderResourceView>	Srv;
	ComPtr<IDXGIKeyedMutex>				SharedMutex;
	ComPtr<ID3D11Texture2D>				SharedTexture;

	ID3D11Texture2D*					ViewTexture;
	D3D::GameInterface*					Gi;

	std::unique_ptr<ul::GPUDriverD3D11>	Driver;
	std::unique_ptr<GPUContext>			Context;

	HANDLE								SharedTextureHandle;
	ComPtr<IDXGIKeyedMutex>				SharedTextureMutex;

	int									WindowX, 
										WindowY, 
										WindowW, 
										WindowH;
	std::atomic<bool>					WindowOpened,
										WindowLoaded,
										ResourcesLoaded;

	virtual void OnFinishLoading(ul::View* caller) override;
	virtual void OnDOMReady(ultralight::View* caller) override;

	bool IsFinished() const
	{
		return WindowLoaded;
	}
};