#pragma once

#include <optional>
#include <d3d11_4.h>
#include <wrl/client.h>

#include "Hooks.h"
#include "InputController.h"
#include "ClassDecl.h"
#include "Game.h"
#include "GamePatcher.h"
#include "Cleaner.h"

using namespace Microsoft::WRL;

class InputSink
{
public:
	virtual bool Handle(LPMSG Msg) = 0;
};

namespace D3D
{
	class GameInterface
	{
		friend class ::UI;

	public:
		 GameInterface();
		~GameInterface();

		void ToggleIndicators(bool On = true);
		void ToggleBBoxes(bool On = true);
		void ToggleXRay(bool On = true);
		void ToggleBones(bool On = true);
		void ToggleNameTags(bool On = true);
		void ToggleEngineChams(bool On = true);

		ID3D11Device			* GetDevice() const;
		ID3D11Device			* GetDeviceOriginal() const;
		ID3D11DeviceContext		* GetDeviceContext() const;
		ID3D11DeviceContext		* GetDeviceContextOriginal() const;
		IDXGISwapChain			* GetSwapChain() const;
		ID3D11RenderTargetView	* GetRenderTarget() const;

		bool ShouldRenderVisuals() const;
		bool ShouldRenderChams() const;

		float GetFrameDuration() const;
		Renderer* GetRenderer();
		InputController& GetInputCtrl();
		ScreenshotCleaner& GetCleaner();

		std::shared_ptr<ViewState> GetViewState();
		std::shared_ptr<Utility> GetUtility();

		void Shutdown();

	public:
		static HRESULT WINAPI Present(IDXGISwapChain* SwapChain, UINT Sync, UINT Flags);
		static BOOL WINAPI GetMessageHk(LPMSG Msg, HWND Hwnd, UINT MsgFilterMin, UINT MsgFilterMax);

	protected:
		HRESULT InitializeD3D();
		HRESULT InitializeUi();
		HRESULT Initialize();

		void KeyPressed(uint16_t Key);
		void KeyUp(uint16_t Key);

	private:
		ScreenshotCleaner					Cleaner;
		InputController						InputCtrl;

		ID3D11Device					*	Device;
		ID3D11Device					*	DeviceOriginal;
		ID3D11DeviceContext				*	Context;
		ID3D11DeviceContext				*	ContextOriginal;

		ComPtr<ID3D11Texture2D>				BBuffer;
		ComPtr<ID3D11RenderTargetView>		RenderTarget;

		DefaultMemoryInterface<>			DefaultMemInterface;

		std::unique_ptr<BatchMemoryService>	BatchMemService;
		std::unique_ptr<GamePatcher>		Patcher;
		std::unique_ptr<UI>					Ui;
		std::unique_ptr<Renderer>			RenderMgr;
		std::shared_ptr<Utility>			Utils;

		std::weak_ptr<RenderPass>			RenUi;
		std::weak_ptr<RenderIndicators>		RenIndicators;
		std::weak_ptr<RenderBones>			RenBones;
		std::weak_ptr<XRayDepthRenderer<
			Alliance_Num>>					RenderXRay;

		float								LastFrameTime;
		float								FrameDuration;

		bool								SSInProgress;
		bool								EngineChams;
	};
}