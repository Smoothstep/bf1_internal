#pragma once

#include <include/cef_app.h>
#include <include/cef_client.h>

#include "D3DHooks.h"

class Utility;

namespace D3D
{
	class FsQuadAlphaRenderer;
	class FsBlurAlphaRenderer;
}

namespace Cef
{
	class UIClient;
	class UI
	{
	public:
		UI(D3D::GameInterface* Gi);
		~UI();

		bool WindowAcceptsInput(int X, int Y) const;
		bool HandleMouseButton(CefBrowserHost::MouseButtonType Button, int X, int Y, bool KeyUp);
		bool HandleMouseMove(WPARAM Modifiers, int X, int Y, int Z = 0);
		bool HandleMouseWheel(WPARAM Modifiers, int X, int Y, int Z = 0);
		bool HandleKeyboardInput(WPARAM Key, LPARAM Params, UINT MsgId);

		bool Update();
		bool UpdateInput();
		void RenderOverlay(GameRenderer* Renderer);
		void Shutdown();

	private:
		bool CheckMouseCaptured(int X, int Y);

		HRESULT Initialize();
		HRESULT InitializeResources();
		HRESULT InitializeCef(const bool UseOsr = true);

		std::shared_ptr<Utility>					Utility;
		std::shared_ptr<D3D::FsBlurAlphaRenderer>	FsRenderer;

		IntPoint									MousePos,
													MousePosPrev;

		CefRefPtr<UIClient>							Client;
		CefRefPtr<CefApp>							App;

		ComPtr<ID3D11DeviceContext>					DefCtx;
		ComPtr<ID3D11Buffer>						Cbv;
		ComPtr<ID3D11Texture2D>						ViewTextureCPU;
		ComPtr<ID3D11Texture2D>						ViewTexture;
		ComPtr<ID3D11ShaderResourceView>			Srv;
		ComPtr<ID3D11RenderTargetView>				Rtv;
		ComPtr<IDXGIKeyedMutex>						SharedMutex;
		ComPtr<ID3D11Texture2D>						SharedTexture;

		D3D::GameInterface*							Gi;
		ComPtr<IDXGIKeyedMutex>						SharedTextureMutex;

		std::atomic<bool>							WindowOpened,
													MouseCaptureAlphaTest,
													MouseCaptured,
													WindowLoaded,
													ResourcesLoaded;

		bool IsFinished() const
		{
			return WindowLoaded;
		}
	};
}