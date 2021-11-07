#include "stdafx.h"
#include "D3DHooks.h"
#include "D3DRenderer.h"
#include "Hooks.h"
#include "Offsets.h"
#include "ClientGame.h"
#include "ClientPlayer.h"
#include "Game.h"
#include "Crypto.h"
#include "Globals.h"

#ifdef UI_UL
#include "UI.h"
#include "Ultralight.h"
#else
#include "UI_Cef.h"
#endif

#include "GameHooks.h"
#include "D3DBlob.h"
#include "Utility.h"

#include "D3DIndicators.h"
#include "D3DXRay.h"

#include <intrin.h>
#include <windowsx.h>

namespace D3D
{
	GameInterface::GameInterface()
		: BatchMemService{}
	{
		ThrowOnError(InitializeD3D());
	}

	GameInterface::~GameInterface()
	{
		if (Context)
		{
			Context->Release();
		}

		if (Device)
		{
			Device->Release();
		}
	}

#ifdef UI_UL
	bool UIHandleMessage(UI* Ui, LPMSG Msg)
	{
		bool Dispatch = false;

		switch (Msg->message)
		{
			case WKEYDOWN:
			{
				Dispatch = !Ui->HandleKeyboardInput(ul::KeyEvent::kType_KeyDown, Msg->wParam, Msg->lParam);
				break;
			}
			case WKEYUP:
			{
				Dispatch = !Ui->HandleKeyboardInput(ul::KeyEvent::kType_KeyUp, Msg->wParam, Msg->lParam);
				break;
			}
			case WLBUTTONDOWN:
			{
				Dispatch = !Ui->HandleMouseInput(ul::MouseEvent::kButton_Left, ul::MouseEvent::kType_MouseDown,
					GET_X_LPARAM(Msg->lParam), GET_Y_LPARAM(Msg->lParam));
				break;
			}
			case WRBUTTONDOWN:
			{
				Dispatch = !Ui->HandleMouseInput(ul::MouseEvent::kButton_Right, ul::MouseEvent::kType_MouseDown,
					GET_X_LPARAM(Msg->lParam), GET_Y_LPARAM(Msg->lParam));
				break;
			}
			case WLBUTTONUP:
			{
				Dispatch = !Ui->HandleMouseInput(ul::MouseEvent::kButton_Left, ul::MouseEvent::kType_MouseUp,
					GET_X_LPARAM(Msg->lParam), GET_Y_LPARAM(Msg->lParam));
				break;
			}
			case WRBUTTONUP:
			{
				Dispatch = !Ui->HandleMouseInput(ul::MouseEvent::kButton_Right, ul::MouseEvent::kType_MouseUp,
					GET_X_LPARAM(Msg->lParam), GET_Y_LPARAM(Msg->lParam));
				break;
			}
			case WMOUSEMOVE:
			{
				Dispatch = !Ui->HandleMouseInput(ul::MouseEvent::kButton_None, ul::MouseEvent::kType_MouseMoved,
					GET_X_LPARAM(Msg->lParam), GET_Y_LPARAM(Msg->lParam));
				break;
			}
			case WMOUSEWHEEL:
			{
				Dispatch = !Ui->HandleMouseInput(ul::MouseEvent::kButton_Middle, ul::MouseEvent::kType_MouseMoved,
					GET_X_LPARAM(Msg->lParam), GET_Y_LPARAM(Msg->lParam), GET_WHEEL_DELTA_WPARAM(Msg->wParam));
				break;
			}
		}

		return Dispatch;
	}
#else
	bool UIHandleMessage(UI* Ui, LPMSG Msg)
	{
		bool Dispatch = true;

		switch (Msg->message)
		{
			case WM_CHAR:
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				Dispatch = !Ui->HandleKeyboardInput(Msg->wParam, Msg->lParam, Msg->message);
				break;
			}
			case WM_LBUTTONDOWN:
			{
				Dispatch = !Ui->HandleMouseButton(CefBrowserHost::MouseButtonType::MBT_LEFT,
					GET_X_LPARAM(Msg->lParam), GET_Y_LPARAM(Msg->lParam), false);
				break;
			}
			case WM_RBUTTONDOWN:
			{
				Dispatch = !Ui->HandleMouseButton(CefBrowserHost::MouseButtonType::MBT_RIGHT,
					GET_X_LPARAM(Msg->lParam), GET_Y_LPARAM(Msg->lParam), false);
				break;
			}
			case WM_LBUTTONUP:
			{
				Dispatch = !Ui->HandleMouseButton(CefBrowserHost::MouseButtonType::MBT_LEFT,
					GET_X_LPARAM(Msg->lParam), GET_Y_LPARAM(Msg->lParam), true);
				break;
			}
			case WM_RBUTTONUP:
			{
				Dispatch = !Ui->HandleMouseButton(CefBrowserHost::MouseButtonType::MBT_RIGHT,
					GET_X_LPARAM(Msg->lParam), GET_Y_LPARAM(Msg->lParam), true);
				break;
			}
			case WM_MOUSEMOVE:
			{
				Dispatch = !Ui->HandleMouseMove(Msg->wParam, GET_X_LPARAM(Msg->lParam), GET_Y_LPARAM(Msg->lParam));
				break;
			}
			case WM_MOUSEWHEEL:
			{
				Dispatch = !Ui->HandleMouseWheel(Msg->wParam, GET_X_LPARAM(Msg->lParam), GET_Y_LPARAM(Msg->lParam), GET_WHEEL_DELTA_WPARAM(Msg->wParam));
				break;
			}
		}

		Ui->UpdateInput();

		return Dispatch;
	}
#endif

	BOOL WINAPI GameInterface::GetMessageHk(LPMSG Msg, HWND Hwnd, UINT MsgFilterMin, UINT MsgFilterMax)
	{
		auto HkRef = HookRefSpec<GameInterface&>::Current();
		auto& [This] = HkRef.Tuple();
		bool Dispatch = true;

		BOOL Result;

		do
		{
			Result = HkRef->GetOriginal<decltype(&GetMessageHk)>()(Msg, Hwnd, MsgFilterMin, MsgFilterMax);

			if (Result)
			{
				UI* Ui = This.Ui.get();

				if (Ui)
				{
					Dispatch = UIHandleMessage(Ui, Msg);
				}
				else
				{
					Dispatch = true;
				}
			}
		} while (!Dispatch);

		return Result;
	}

	HRESULT WINAPI GameInterface::Present(IDXGISwapChain * SwapChain, UINT Sync, UINT Flags)
	{
		auto HkRef = HookRefSpec<GameInterface&>::Current();
		auto& [This] = HkRef.Tuple();
		auto* RendererGame = GameRenderer::Instance();

		HRESULT Hr;

		if (!This.RenderTarget)
		{
			if (FAILED(Hr = This.Initialize()))
			{
				const auto Ex = HrException(Hr, "Failed to initialize interface");
				{
					Logger::Error() << Ex.what() << Logger::EndLine;
				}

				return HkRef->GetOriginal<HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT)>()(SwapChain, Sync, Flags);
			}
		}

		if (This.RenderMgr && This.ShouldRenderVisuals())
		{
			This.RenderMgr->RenderPostProcess(This.RenderTarget.Get(), RendererGame);
		}

		try
		{
			Hr = HkRef->GetOriginal<HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT)>()(SwapChain, Sync, Flags);
		}
		catch (...)
		{
			Hr = This.Device->GetDeviceRemovedReason();
		}

		if (FAILED(Hr))
		{
			const auto Ex = HrException(Hr, "Failed to present: Device removed?");
			{
				Logger::Error() << Ex.what() << Logger::EndLine;
			}
		}

		const float CurrentTime = ClientGameContext::Instance()->GameTimeMs();

		This.FrameDuration = CurrentTime - This.LastFrameTime;
		This.LastFrameTime = CurrentTime;

		if (This.Utils)
		{
			This.Utils->ExecuteRoutine(This.InputCtrl);
		}

		static bool InThirdPerson = false;

		if (This.SSInProgress)
		{
			This.SSInProgress = false;
			This.Cleaner.CaptureFrame();

			if (InThirdPerson)
			{
				This.Patcher->ToggleThirdPerson(true);
			}
		}

		if ((This.SSInProgress = This.Cleaner.NeedsFrame()))
		{
			Crypto::IteratePlayers([](ClientPlayer* Player)
			{
				if (IsValidPointer(Player) &&
					IsValidPointer(Player->Soldier))
				{
					Player->Soldier->Chams = VisDefault;
				}

				return true;
			});

			if ((InThirdPerson = !!This.InputCtrl.GetViewOffset()) == true)
			{
				This.Patcher->ToggleThirdPerson(false);
			}
		}

		return Hr;
	}

	void GameInterface::KeyPressed(uint16_t Key)
	{
		Utils->ExecuteKeyAction(InputCtrl, Key, false);
	}

	void GameInterface::KeyUp(uint16_t Key)
	{
		Utils->ExecuteKeyAction(InputCtrl, Key, true);
	}

	HRESULT GameInterface::Initialize()
	{
		Device = nullptr;
		Context = nullptr;

		if (!IsValidPointer(DxRenderer::Instance()))
		{
			return E_FAIL;
		}

		Screen * Screen = DxRenderer::Instance()->Screen;

		if (!IsValidPointer(Screen))
		{
			return E_FAIL;
		}

		D3D_FEATURE_LEVEL FeatureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};

		HRESULT Hr;

		while (!Device)
		{
			while (true)
			{
				try
				{
					Hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
						FeatureLevels, _countof(FeatureLevels), D3D11_SDK_VERSION, &Device, 0, &Context);
					break;
				}
				catch (...)
				{
					const auto Ex = HrException(Hr);
					{
						Logger::Error() << Ex.what();
					}
				}
			}

			if (SUCCEEDED(Hr))
			{
				break;
			}
		}

		DeviceOriginal = DxRenderer::Instance()->Device;
		ContextOriginal = DxRenderer::Instance()->Context;

		if (FAILED(Hr))
		{
			Device = DeviceOriginal;
			Context = ContextOriginal;
		}

		if (!Device ||
			!Context)
		{
			return E_FAIL;
		}

		Device->AddRef();
		Context->AddRef();

		IDXGISwapChain * SwapChain = Screen->SwapChain;

		if (!IsValidPointer(SwapChain))
		{
			return E_FAIL;
		}

		SwapChainVMTWrapper * Wrapper = *reinterpret_cast<SwapChainVMTWrapper**>(SwapChain);

		if (FAILED(Hr = Wrapper->GetBuffer(SwapChain, 0, __uuidof(ID3D11Texture2D), &BBuffer)))
		{
			return Hr;
		}

		if (FAILED(Hr = DeviceOriginal->CreateRenderTargetView(BBuffer.Get(), nullptr, &RenderTarget)))
		{
			return Hr;
		}

		BatchMemService = std::make_unique<BatchMemoryService>();
		Patcher = std::make_unique<GamePatcher>(DefaultMemInterface);
		Utils = std::make_shared<Utility>(this, Patcher.get());
		Ui = std::make_unique<UI>(this);

		return Hr;
	}

	HRESULT GameInterface::InitializeUi()
	{
		Ui = std::make_unique<UI>(this);
		return S_OK;
	}

	HRESULT GameInterface::InitializeD3D()
	{
		Device = nullptr;
		Context = nullptr;

		if (!IsValidPointer(DxRenderer::Instance()))
		{
			return E_FAIL;
		}

		Screen * Screen = DxRenderer::Instance()->Screen;

		if (!IsValidPointer(Screen))
		{
			return E_FAIL;
		}

		RenderMgr = std::make_unique<Renderer>();
		RenderMgr->AddPass<DelegateRenderPass<RenderPass::Merge, 0>>([this](GameRenderer* Renderer)
		{
			if (Ui)
			{
				Ui->RenderOverlay(Renderer);
			}
		});

		return S_OK;
	}

	void GameInterface::ToggleIndicators(bool On)
	{
		if (On)
		{
			if (RenIndicators.expired())
			{
				RenIndicators = RenderMgr->AddPass<RenderIndicators>();

				if (auto Locked = RenIndicators.lock())
				{
					Locked->Initialize(this);
				}
			}
		}
		else
		{
			if (auto Locked = RenIndicators.lock())
			{
				RenderMgr->RemovePass(Locked.get());
				RenIndicators.reset();
			}
		}
	}

	void GameInterface::ToggleXRay(bool On)
	{
		auto GetMaterial = [this]() -> std::optional<XRayMaterial>
		{
			try
			{
				return XRayMaterial
				{ 
					GetDeviceOriginal(),
					ShaderManager::Instance().Compile<ID3D11PixelShader>(L"CharmXRay.hlsl", "main", true)
				};
			}
			catch (const std::exception& Ex)
			{
				Logger::Error() << Ex.what() << Logger::EndLine;
				return {};
			}
		};

		if (On)
		{
			auto Mat = GetMaterial();

			if (Mat)
			{
				RenderXRay = RenderMgr->AddPass<XRayDepthRenderer<Alliance_Num>>();
				RenderXRay.lock()->Initialize(*Mat);
			}
		}
		else
		{
			if (auto Locked = RenderXRay.lock(); Locked)
			{
				RenderMgr->RemovePass(Locked.get());
				RenderXRay.reset();
			}
		}
	}

	void GameInterface::ToggleBones(bool On)
	{
		if (On)
		{
			ToggleIndicators();
		}

		if (auto Locked = RenIndicators.lock())
		{
			Locked->Configure([On](IndicatorOptions& Options)
			{
				Options.Bones = On;
			});
		}
	}

	void GameInterface::ToggleNameTags(bool On)
	{
		if (On)
		{
			ToggleIndicators();
		}

		if (auto Locked = RenIndicators.lock())
		{
			Locked->Configure([On](IndicatorOptions& Options)
			{
				Options.Nametags = On;
			});
		}
	}

	void GameInterface::ToggleBBoxes(bool On)
	{
		if (On && RenIndicators.expired())
		{
			ToggleIndicators();
		}

		if (auto Locked = RenIndicators.lock())
		{
			Locked->Configure([On](IndicatorOptions& Options)
			{
				Options.BBox = On;
			});
		}
	}

	void GameInterface::ToggleEngineChams(bool On)
	{
		EngineChams = On;
	}

	ID3D11Device* GameInterface::GetDevice() const
	{
		return Device;
	}

	ID3D11Device* GameInterface::GetDeviceOriginal() const
	{
		return DeviceOriginal;
	}

	ID3D11DeviceContext* GameInterface::GetDeviceContext() const
	{
		return Context;
	}

	ID3D11DeviceContext* GameInterface::GetDeviceContextOriginal() const
	{
		return ContextOriginal;
	}

	IDXGISwapChain* GameInterface::GetSwapChain() const
	{
		return DxRenderer::Instance()->Screen->SwapChain;
	}

	ID3D11RenderTargetView* GameInterface::GetRenderTarget() const
	{
		return RenderTarget.Get();
	}

	float GameInterface::GetFrameDuration() const
	{
		return FrameDuration;
	}

	InputController& GameInterface::GetInputCtrl()
	{
		return InputCtrl;
	}

	bool GameInterface::ShouldRenderVisuals() const
	{
		return !SSInProgress;
	}

	bool GameInterface::ShouldRenderChams() const
	{
		return EngineChams;
	}
	
	Renderer* GameInterface::GetRenderer()
	{
		return RenderMgr.get();
	}

	ScreenshotCleaner& GameInterface::GetCleaner()
	{
		return Cleaner;
	}

	std::shared_ptr<Utility> GameInterface::GetUtility()
	{
		return Utils;
	}
	
	std::shared_ptr<ViewState> GameInterface::GetViewState()
	{
		if (!ShouldRenderVisuals())
		{
			return {};
		}

		return std::static_pointer_cast<ViewState>(RenderXRay.lock());
	}
	
	void GameInterface::Shutdown()
	{
		Ui->Shutdown();
	}
}