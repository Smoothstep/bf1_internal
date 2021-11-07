#include <include/cef_app.h>

#include "stdafx.h"
#include "Utility.h"
#include "UI_Cef.h"
#include "Globals.h"
#include "D3DHooks.h"
#include "D3DCommon.h"

class UiApp : public CefApp
{
    IMPLEMENT_REFCOUNTING(UiApp);
};

namespace Cef
{
    bool IsKeyDown(WPARAM wParam) 
    {
        return (GetKeyState(static_cast<int>(wParam)) & 0x8000) != 0;
    }

    int GetCefMouseModifiers(WPARAM wparam) 
    {
        int Modifiers = 0;

        if (wparam & MK_CONTROL)
        {
            Modifiers |= EVENTFLAG_CONTROL_DOWN;
        }
        if (wparam & MK_SHIFT)
        {
            Modifiers |= EVENTFLAG_SHIFT_DOWN;
        }
        if (IsKeyDown(VK_MENU))
        {
            Modifiers |= EVENTFLAG_ALT_DOWN;
        }
        if (wparam & MK_LBUTTON)
        {
            Modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
        }
        if (wparam & MK_MBUTTON)
        {
            Modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
        }
        if (wparam & MK_RBUTTON)
        {
            Modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
        }

        if (GetKeyState(VK_NUMLOCK) & 1)
        {
            Modifiers |= EVENTFLAG_NUM_LOCK_ON;
        }
        if (GetKeyState(VK_CAPITAL) & 1)
        {
            Modifiers |= EVENTFLAG_CAPS_LOCK_ON;
        }

        return Modifiers;
    }

    int GetCefKeyboardModifiers(WPARAM wParam, LPARAM lParam) 
    {
        int Modifiers = 0;
        
        if (IsKeyDown(VK_SHIFT))
        {
            Modifiers |= EVENTFLAG_SHIFT_DOWN;
        }
        
        if (IsKeyDown(VK_CONTROL))
        {
            Modifiers |= EVENTFLAG_CONTROL_DOWN;
        }
        
        if (IsKeyDown(VK_MENU))
        {
            Modifiers |= EVENTFLAG_ALT_DOWN;
        }
        
        if (GetKeyState(VK_NUMLOCK) & 1)
        {
            Modifiers |= EVENTFLAG_NUM_LOCK_ON;
        }

        if (GetKeyState(VK_CAPITAL) & 1)
        {
            Modifiers |= EVENTFLAG_CAPS_LOCK_ON;
        }

        switch (wParam) 
        {
            case VK_RETURN:
                if ((lParam >> 16) & KF_EXTENDED)
                {
                    Modifiers |= EVENTFLAG_IS_KEY_PAD;
                }
                break;
            case VK_INSERT:
            case VK_DELETE:
            case VK_HOME:
            case VK_END:
            case VK_PRIOR:
            case VK_NEXT:
            case VK_UP:
            case VK_DOWN:
            case VK_LEFT:
            case VK_RIGHT:
                if (!((lParam >> 16) & KF_EXTENDED))
                {
                    Modifiers |= EVENTFLAG_IS_KEY_PAD;
                }
                break;
            case VK_NUMLOCK:
            case VK_NUMPAD0:
            case VK_NUMPAD1:
            case VK_NUMPAD2:
            case VK_NUMPAD3:
            case VK_NUMPAD4:
            case VK_NUMPAD5:
            case VK_NUMPAD6:
            case VK_NUMPAD7:
            case VK_NUMPAD8:
            case VK_NUMPAD9:
            case VK_DIVIDE:
            case VK_MULTIPLY:
            case VK_SUBTRACT:
            case VK_ADD:
            case VK_DECIMAL:
            case VK_CLEAR:
                Modifiers |= EVENTFLAG_IS_KEY_PAD;
                break;
            case VK_SHIFT:
                if (IsKeyDown(VK_LSHIFT))
                {
                    Modifiers |= EVENTFLAG_IS_LEFT;
                }
                else if (IsKeyDown(VK_RSHIFT))
                {
                    Modifiers |= EVENTFLAG_IS_RIGHT;
                }
                break;
            case VK_CONTROL:
                if (IsKeyDown(VK_LCONTROL))
                {
                    Modifiers |= EVENTFLAG_IS_LEFT;
                }
                else if (IsKeyDown(VK_RCONTROL))
                {
                    Modifiers |= EVENTFLAG_IS_RIGHT;
                }
                break;
            case VK_MENU:
                if (IsKeyDown(VK_LMENU))
                {
                    Modifiers |= EVENTFLAG_IS_LEFT;
                }
                else if (IsKeyDown(VK_RMENU))
                {
                    Modifiers |= EVENTFLAG_IS_RIGHT;
                }
                break;
            case VK_LWIN:
                Modifiers |= EVENTFLAG_IS_LEFT;
                break;
            case VK_RWIN:
                Modifiers |= EVENTFLAG_IS_RIGHT;
                break;
        }
        return Modifiers;
    }

    template<size_t Depth, class TFn>
    static void ConvertValueImpl(CefRefPtr<CefValue> Value, TFn Fn)
    {
        switch (Value->GetType())
        {
        case CefValueType::VTYPE_DICTIONARY:
            if constexpr (Depth <= 4)
            {
                const auto Dict = Value->GetDictionary();
                CefDictionaryValue::KeyList Keys;

                if (Dict->GetKeys(Keys))
                {
                    std::unordered_map<std::string, TConfigValue> Map;
                    Map.reserve(Keys.size());

                    for (const auto& Key : Keys)
                    {
                        const auto Value = Dict->GetValue(Key);

                        ConvertValueImpl<Depth + 1>(Value, [&Map, &Key](const auto Value)
                        {
                            Map[Key.ToString()] = Value;
                        });
                    }

                    Fn(std::move(Map));
                }
                else
                {
                    Logger::Warn() << "GetKeys failed" << Logger::EndLine;
                }
            }

            break;

        case CefValueType::VTYPE_DOUBLE:
            Fn(Value->GetDouble());
            break;

        case CefValueType::VTYPE_BOOL:
            Fn(Value->GetBool());
            break;

        case CefValueType::VTYPE_INT:
            Fn(Value->GetInt());
            break;

        case CefValueType::VTYPE_LIST:
            if constexpr (Depth <= 4)
            {
                const auto ValueList = Value->GetList();
                std::vector<TConfigValue> Values{ ValueList->GetSize() };

                for (size_t N = 0; N < Values.size(); ++N)
                {
                    ConvertValueImpl<Depth + 1>(ValueList->GetValue(N), [&Values, N](const auto Value)
                    {
                        Values[N] = Value;
                    });
                }

                Fn(std::move(Values));
            }
            break;

        case CefValueType::VTYPE_STRING:
            Fn(Value->GetString().ToString());
            break;

        default:
            Logger::Warn() << "Can't convert value of unsupported type: " << std::to_string(Value->GetType()) << Logger::EndLine;
            break;
        }
    }

    template<size_t Depth, class TFn>
    static void ConvertValueImpl(CefRefPtr<CefV8Value> Value, TFn Fn)
    {
        if (Value->IsInt())
        {
            Fn(Value->GetIntValue());
        }
        else if (Value->IsArray())
        {
            if constexpr (Depth <= 4)
            {
                std::vector<TConfigValue> Values{ Value->GetArrayLength() };

                for (int N = 0; N < Values.size(); ++N)
                {
                    ConvertValueImpl<Depth + 1>(Value->GetValue(N), [&Values, N](const auto Value)
                    {
                        Values[N] = Value;
                    });
                }

                Fn(std::move(Values));
            }
        }
        else if (Value->IsBool())
        {
            Fn(Value->GetBoolValue());
        }
        else if (Value->IsString())
        {
            Fn(Value->GetStringValue().ToString());
        }
        else if (Value->IsDouble())
        {
            Fn(Value->GetDoubleValue());
        }
        else if (Value->IsUInt())
        {
            Fn(Value->GetUIntValue());
        }
        else if (Value->IsObject())
        {
            if constexpr (Depth <= 4)
            {
                CefDictionaryValue::KeyList Keys;

                if (Value->GetKeys(Keys))
                {
                    tsl::bhopscotch_map<std::string, TConfigValue> Map;
                    Map.reserve(Keys.size());

                    for (const auto& Key : Keys)
                    {
                        const auto V = Value->GetValue(Key);

                        ConvertValueImpl<Depth + 1>(V, [&Map, &Key](const auto Value)
                        {
                            Map[Key.ToString()] = Value;
                        });
                    }
                }
                else
                {
                    Logger::Warn() << "GetKeys failed" << Logger::EndLine;
                }
            }
        }
        else
        {
            Logger::Warn() << "Can't convert value of unsupported type" << Logger::EndLine;
        }
    }

    template<class T>
    struct TypeIdentity
    {
        using Type = T;
    };

    struct ConvertV8
    {
        template<class TFrom>
        CefRefPtr<CefV8Value> operator()(typename TypeIdentity<TFrom>::Type Value);

        static CefRefPtr<CefV8Value> From(double Value);
        static CefRefPtr<CefV8Value> From(bool Value);
        static CefRefPtr<CefV8Value> From(int Value);
        static CefRefPtr<CefV8Value> From(unsigned int Value);
        static CefRefPtr<CefV8Value> From(std::string_view Value);
        static CefRefPtr<CefV8Value> From(const std::unordered_map<std::string, TConfigValue>& Values);
        static CefRefPtr<CefV8Value> From(const std::vector<TConfigValue>& Values);
    };

    CefRefPtr<CefV8Value> ConvertV8::From(double Value)
    {
        return CefV8Value::CreateDouble(Value);
    }

    CefRefPtr<CefV8Value> ConvertV8::From(bool Value)
    {
        return CefV8Value::CreateBool(Value);
    }

    CefRefPtr<CefV8Value> ConvertV8::From(int Value)
    {
        return CefV8Value::CreateInt(Value);
    }

    CefRefPtr<CefV8Value> ConvertV8::From(unsigned int Value)
    {
        return CefV8Value::CreateUInt(Value);
    }

    CefRefPtr<CefV8Value> ConvertV8::From(std::string_view Value)
    {
        return CefV8Value::CreateString(Value.data());
    }

    CefRefPtr<CefV8Value> ConvertV8::From(const std::unordered_map<std::string, TConfigValue>& Values)
    {
        auto Obj = CefV8Value::CreateObject(nullptr, nullptr);

        for (const auto& [K, V] : Values)
        {
            auto Converted = std::visit([](const auto& CValue)
            {
                return ConvertV8::From(CValue);
            }, V);

            Obj->SetValue(K, Converted, CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_NONE);
        }

        return Obj;
    }

    CefRefPtr<CefV8Value> ConvertV8::From(const std::vector<TConfigValue>& Values)
    {
        auto Arr = CefV8Value::CreateArray(static_cast<int>(Values.size()));

        for (int N = 0; N < Values.size(); ++N)
        {
            auto Converted = std::visit([](const auto& CValue)
            {
                return ConvertV8::From(CValue);
            }, Values[N]);

            Arr->SetValue(N, Converted);
        }

        return Arr;
    }

    template<class TFn>
    static void ConvertValue(CefRefPtr<CefValue> Value, TFn Fn)
    {
        ConvertValueImpl<1>(Value, Fn);
    }

    template<class TFn>
    static void ConvertValue(CefRefPtr<CefV8Value> Value, TFn Fn)
    {
        ConvertValueImpl<1>(Value, Fn);
    }

    class UIClient :
        public CefClient,
        public CefLifeSpanHandler,
        public CefContextMenuHandler
    {
        IMPLEMENT_REFCOUNTING(UIClient);

    public:
        UIClient(
            std::shared_ptr<Utility> Utils,
            CefRefPtr<CefRenderHandler> RenderHandler,
            CefRefPtr<CefLoadHandler> LoadHandler)
            : Utils{ Utils }
            , RenderHandler{ RenderHandler}
            , LoadHandler{ LoadHandler }
        {}

        ~UIClient()
        {
            if (ValueDispatcherThread.joinable())
            {
                CurrentBrowser = nullptr;
                ValueDispatcherThread.join();
            }
        }

        CefRefPtr<CefLoadHandler> GetLoadHandler() override
        {
            return LoadHandler;
        }

        CefRefPtr<CefRenderHandler> GetRenderHandler() override
        {
            return RenderHandler;
        }

        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override
        {
            return this;
        }

        CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override 
        {
            return this;
        }

        void OnAfterCreated(CefRefPtr<CefBrowser> Browser) override
        {
            if (ValueDispatcherThread.joinable())
            {
                CurrentBrowser = nullptr;
                ValueDispatcherThread.join();
            }

            CurrentBrowser = Browser;

            ValueDispatcherThread = std::thread([this]()
            {
                while (CurrentBrowser)
                {
                    auto Guard = std::unique_lock{ QueueMutex };
                    {
                        ValuedChanged.wait(Guard);
                    }

                    while (!ValueChangeQueue.empty())
                    {
                        const auto& Pair = ValueChangeQueue.front();
                        const auto ValueKey = Pair.first.ToWString();

                        Logger::Log() << "Received SetValue for: " << ValueKey << Logger::EndLine;

                        ConvertValue(Pair.second, [&](const auto CValue)
                        {
                            Utils->SetConfigValue(ValueKey, CValue);
                        });

                        ValueChangeQueue.pop();
                    }
                }
            });

            Logger::Log() << "Browser created" << Logger::EndLine;
        }

        bool DoClose(CefRefPtr<CefBrowser> Browser) override
        {
            CurrentBrowser = nullptr;
            
            Logger::Log() << "Browser closed" << Logger::EndLine;

            return false;
        }

        void OnBeforeContextMenu(
            CefRefPtr<CefBrowser> Browser,
            CefRefPtr<CefFrame> Frame,
            CefRefPtr<CefContextMenuParams> Params,
            CefRefPtr<CefMenuModel> Model) override
        {
            if (Model)
            {
                Model->Clear();
            }
        }

        bool OnProcessMessageReceived(
            CefRefPtr<CefBrowser> Browser,
            CefRefPtr<CefFrame> Frame,
            CefProcessId SourceProcess,
            CefRefPtr<CefProcessMessage> Message) override
        {
            if (Message->GetName() == "SetValue")
            {
                const auto Args = Message->GetArgumentList();

                if (Args->GetSize() == 0 || (Args->GetSize() % 2) != 0)
                {
                    return false;
                }

                const auto Guard = std::lock_guard{ QueueMutex };

                for (size_t N = 0; N < Args->GetSize() / 2; ++N)
                {
                    if (Args->GetType(N * 2) != CefValueType::VTYPE_STRING)
                    {
                        continue;
                    }

                    ValueChangeQueue.push(std::make_pair(Args->GetString(N * 2), Args->GetValue(N * 2 + 1)->Copy()));
                    ValuedChanged.notify_all();
                }
            }

            return true;
        }

        void OnBeforeClose(CefRefPtr<CefBrowser> Browser) override
        {
            Logger::Log() << "Closing browser" << Logger::EndLine;
        }

        CefBrowser* Browser()
        {
            return CurrentBrowser.get();
        }

    private:
        std::mutex QueueMutex;
        std::thread ValueDispatcherThread;
        std::shared_ptr<Utility> Utils;
        std::condition_variable ValuedChanged;
        std::queue<std::pair<CefString, CefRefPtr<CefValue>>> ValueChangeQueue;
        CefRefPtr<CefRenderHandler> RenderHandler;
        CefRefPtr<CefLoadHandler> LoadHandler;
        CefRefPtr<CefBrowser> CurrentBrowser;
    };

    template<class TFn>
    class UiDelegateTask : public CefTask
    {
        IMPLEMENT_REFCOUNTING(UiDelegateTask);

    public:
        UiDelegateTask(TFn&& Fn)
            : Fn { std::move(Fn) }
        {}

        virtual void Execute() override
        {
            Fn();
        }

    private:
        TFn Fn;
    };

    class BaseValueHandler : public CefV8Handler
    {
        IMPLEMENT_REFCOUNTING(BaseValueHandler);

    public:
        BaseValueHandler(std::shared_ptr<Utility> Utility)
            : Utility{ Utility }
        {}

    protected:
        std::shared_ptr<Utility> Utility;
    };

    class UiLoadHandler : public CefLoadHandler
    {
        IMPLEMENT_REFCOUNTING(UiLoadHandler);

    public:
        UiLoadHandler(std::shared_ptr<Utility> Utility)
            : Utility{ Utility }
        {}

        void OnLoadEnd(
            CefRefPtr<CefBrowser> Browser,
            CefRefPtr<CefFrame> Frame,
            int HttpStatusCode) override
        {
        }

    private:
        std::shared_ptr<Utility> Utility;
    };

    class UiRenderHandler : public CefRenderHandler
    {
        IMPLEMENT_REFCOUNTING(UiRenderHandler);

    public:
        UiRenderHandler(ID3D11Device1* Device, ID3D11DeviceContext* Ctx, ID3D11RenderTargetView** Rtv, bool UpsideDown = true)
            : Device{ Device }
            , Ctx{ Ctx }
            , Rtv{ Rtv }
            , Renderer{ Device, UpsideDown }
        {}

        bool GetScreenPoint(CefRefPtr<CefBrowser> browser,
            int ViewX,
            int ViewY,
            int& ScreenX,
            int& ScreenY) override
        {
            POINT Point{ ViewX, ViewY };
            {
                ClientToScreen(Globals::WindowHandle, &Point);
            }

            ScreenX = Point.x;
            ScreenY = Point.y;

            return true;
        }

        virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> Browser, CefRect& Rect) override
        {
            RECT WRect, CRect;
            {
                GetWindowRect(Globals::WindowHandle, &WRect);
                GetClientRect(Globals::WindowHandle, &CRect);
            }

            POINT Point{ 0, 0 };
            {
                ClientToScreen(Globals::WindowHandle, &Point);
            }

            Rect.x = 0;
            Rect.y = 0;
            Rect.width = GetSystemMetrics(SM_CXSCREEN);
            Rect.height = GetSystemMetrics(SM_CYSCREEN);

            return true;
        }

        virtual void GetViewRect(CefRefPtr<CefBrowser> Browser, CefRect& Rect) override
        {
            RECT WRect, CRect;
            {
                GetWindowRect(Globals::WindowHandle, &WRect);
                GetClientRect(Globals::WindowHandle, &CRect);
            }

            POINT Point{ CRect.left, CRect.top };
            {
                ClientToScreen(Globals::WindowHandle, &Point);
            }

            Rect.x = CRect.left;
            Rect.y = CRect.top;
            Rect.width = CRect.right - CRect.left;
            Rect.height = CRect.bottom - CRect.top;
        }

        virtual void OnPaint(CefRefPtr<CefBrowser> Browser, PaintElementType Type, const RectList& DirtyRects, const void* Buffer, int W, int H) override
        {
            if (auto Rv = *Rtv)
            {
                HRESULT Hr;

                if (Tex)
                {
                    D3D11_TEXTURE2D_DESC Desc;
                    {
                        Tex->GetDesc(&Desc);
                    }

                    if (W != Desc.Width ||
                        H != Desc.Height)
                    {
                        Tex.Reset();
                        Srv.Reset();
                    }
                }

                if (!Tex)
                {
                    ComPtr<ID3D11Resource> Resource;
                    {
                        (*Rtv)->GetResource(&Resource);
                    }

                    ComPtr<ID3D11Texture2D> RtvTex;
                    {
                        Resource->QueryInterface(IID_PPV_ARGS(&RtvTex));
                    }

                    D3D11_TEXTURE2D_DESC RtvTexDesc;
                    {
                        RtvTex->GetDesc(&RtvTexDesc);
                    }

                    CD3D11_TEXTURE2D_DESC BufferTex(RtvTexDesc.Format, W, H, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, 1, 0, 0);

                    if (!SUCCEEDED(Hr = Device->CreateTexture2D(&BufferTex, nullptr, &Tex)))
                    {
                        Logger::Error() << "Failed to create texture 2d" << Logger::EndLine;
                        return;
                    }
                }

                if (!Srv)
                {
                    ComPtr<ID3D11Resource> Resource;
                    {
                        (*Rtv)->GetResource(&Resource);
                    }

                    ComPtr<ID3D11Texture2D> RtvTex;
                    {
                        Resource->QueryInterface(IID_PPV_ARGS(&RtvTex));
                    }

                    D3D11_TEXTURE2D_DESC RtvTexDesc;
                    {
                        RtvTex->GetDesc(&RtvTexDesc);
                    }

                    D3D11_SHADER_RESOURCE_VIEW_DESC ResDesc{};
                    {
                        ResDesc.Format = RtvTexDesc.Format;
                        ResDesc.Texture2D.MipLevels = 1;
                        ResDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
                    }

                    if (!SUCCEEDED(Hr = Device->CreateShaderResourceView(Tex.Get(), &ResDesc, &Srv)))
                    {
                        Logger::Error() << "Failed to create srv" << Logger::EndLine;
                        return;
                    }
                }

                const D3D11_BOX DirtyBox
                {
                    0U,
                    0U,
                    0U,
                    static_cast<UINT>(W),
                    static_cast<UINT>(H),
                    1U
                };

                Ctx->UpdateSubresource(Tex.Get(), 0, &DirtyBox, Buffer, W * sizeof(DWORD), W * H * sizeof(DWORD));

                const float ClearColor[] = { 0, 0, 0, 0 };
                Ctx->ClearRenderTargetView(*Rtv, ClearColor);

                Ctx->OMSetRenderTargets(1, Rtv, nullptr);

                const auto Vp = DxRenderer::Instance()->Screen->GetD3D11Viewport();
                Ctx->RSSetViewports(1, &Vp);

                Renderer.Draw(Ctx, Srv.Get());
                /*
                D3D11_MAPPED_SUBRESOURCE Data{};

                if (SUCCEEDED(Hr = Ctx->Map(Tex.Get(), 0, D3D11_MAP_WRITE, 0, &Data)))
                {
                    memcpy(Data.pData, Buffer, std::min(
                        static_cast<unsigned int>(W) * static_cast<unsigned int>(H),
                        Data.DepthPitch * Data.RowPitch));

                    Ctx->Unmap(Tex.Get(), 0);

                    const float ClearColor[] = { 0, 0, 0, 0 };
                    Ctx->ClearRenderTargetView(*Rtv, ClearColor);

                    Ctx->OMSetRenderTargets(1, Rtv, nullptr);

                    const auto Vp = DxRenderer::Instance()->Screen->GetD3D11Viewport();
                    Ctx->RSSetViewports(1, &Vp);

                    Renderer.Draw(Ctx, Srv.Get());
                }
                */
                /*
                
                D3D11_BOX DirtyBox
                {
                    0U,
                    0U,
                    0U,
                    static_cast<UINT>(W),
                    static_cast<UINT>(H),
                    1U
                };

                for (auto& Rect : DirtyRects)
                {
                    DirtyBox.left = Math::Min<int>(DirtyBox.left, Rect.x);
                    DirtyBox.right = Math::Max<int>(DirtyBox.right, Rect.x + Rect.width);
                    DirtyBox.top = Math::Min<int>(DirtyBox.top, Rect.y);
                    DirtyBox.bottom = Math::Max<int>(DirtyBox.bottom, Rect.y + Rect.height);
                }

                Ctx->UpdateSubresource(Tex.Get(), 0, 0, Buffer, W * H, 1);

                const float ClearColor[] = { 0, 0, 0, 0 };
                Ctx->ClearRenderTargetView(*Rtv, ClearColor);

                Ctx->OMSetRenderTargets(1, Rtv, nullptr);

                const auto Vp = DxRenderer::Instance()->Screen->GetD3D11Viewport();
                Ctx->RSSetViewports(1, &Vp);

                Renderer.Draw(Ctx, Srv.Get());
                */
            }
        }

        virtual void OnAcceleratedPaint(
            CefRefPtr<CefBrowser> Browser,
            PaintElementType Type,
            const RectList& DirtyRects,
            void* SharedHandle) override
        {
            if (!*Rtv)
            {
                return;
            }

            D3D11_SHADER_RESOURCE_VIEW_DESC ResDesc{};
            {
                ResDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                ResDesc.Texture2D.MipLevels = 1;
                ResDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
            }

            if (SUCCEEDED(Device->OpenSharedResource1(SharedHandle, IID_PPV_ARGS(&Tex))) &&
                SUCCEEDED(Device->CreateShaderResourceView(Tex.Get(), &ResDesc, &Srv)) &&
                SUCCEEDED(Tex->QueryInterface(IID_PPV_ARGS(&Mutex))))
            {
                Mutex->AcquireSync(1, INFINITE);

                const float ClearColor[] = { 0, 0, 0, 0 };
                Ctx->ClearRenderTargetView(*Rtv, ClearColor);
                Ctx->OMSetRenderTargets(1, Rtv, nullptr);

                const auto Vp = DxRenderer::Instance()->Screen->GetD3D11Viewport();
                Ctx->RSSetViewports(1, &Vp);

                Renderer.Draw(Ctx, Srv.Get());

                Mutex->ReleaseSync(0);
            }
        }

    private:
        ComPtr<ID3D11Texture2D> Tex;
        ComPtr<ID3D11ShaderResourceView> Srv;
        ComPtr<IDXGIKeyedMutex> Mutex;

        ID3D11Device1* Device;
        ID3D11DeviceContext* Ctx;
        ID3D11RenderTargetView** Rtv;
        D3D::FsQuadAlphaRenderer Renderer;
    };

	UI::UI(D3D::GameInterface* Gi)
        : Utility{ Gi->GetUtility() }
        , FsRenderer{}
        , MousePos{}
        , MousePosPrev{}
        , Gi{ Gi }
        , MouseCaptureAlphaTest { true }
	{}

    UI::~UI()
    {
        if (WindowLoaded)
        {
            Client = nullptr;
            WindowLoaded = false;
        }
    }

    HRESULT UI::Initialize()
    {
        HRESULT Hr;

        if (!WindowLoaded)
        {
            if (FAILED(Hr = InitializeCef()))
            {
                return Hr;
            }

            try
            {
                FsRenderer = std::make_shared<D3D::FsBlurAlphaRenderer>(Gi->GetDeviceOriginal());
            }
            catch (const HrException& Ex)
            {
                return Ex.Result();
            }
        }

        if (!ResourcesLoaded)
        {
            if (FAILED(Hr = InitializeResources()))
            {
                return Hr;
            }
        }

        return S_OK;
    }

    HRESULT UI::InitializeResources()
    {
        HRESULT Hr;

        ID3D11Device* Device = Gi->GetDeviceOriginal();

        RECT WRect, CRect;
        {
            GetWindowRect(Globals::WindowHandle, &WRect);
            GetClientRect(Globals::WindowHandle, &CRect);
        }

        D3D11_TEXTURE2D_DESC TexDesc{};
        {
            TexDesc.Width = CRect.right - CRect.left;
            TexDesc.Height = CRect.bottom - CRect.top;
            TexDesc.MipLevels = 0;
            TexDesc.ArraySize = 1;
            TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            TexDesc.SampleDesc.Count = 1;
            TexDesc.Usage = D3D11_USAGE_DYNAMIC;
            TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            TexDesc.Usage = D3D11_USAGE_DEFAULT;
            TexDesc.MiscFlags = 0;
        }

        if (FAILED(Hr = Device->CreateTexture2D(&TexDesc, nullptr, &ViewTexture)))
        {
            return Hr;
        }

        if (FAILED(Hr = Device->CreateShaderResourceView(ViewTexture.Get(), nullptr, &Srv)))
        {
            return Hr;
        }

        if (FAILED(Hr = Device->CreateRenderTargetView(ViewTexture.Get(), nullptr, &Rtv)))
        {
            return Hr;
        }

        D3D11_TEXTURE2D_DESC StagingTexDesc = TexDesc;
        {
            StagingTexDesc.Width = 1;
            StagingTexDesc.Height = 1;
            StagingTexDesc.Usage = D3D11_USAGE_STAGING;
            StagingTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            StagingTexDesc.MiscFlags = 0;
            StagingTexDesc.BindFlags = 0;
        }

        if (FAILED(Hr = Device->CreateTexture2D(&StagingTexDesc, 0, &ViewTextureCPU)))
        {
            return Hr;
        }

        D3D11_BUFFER_DESC CbDesc{};
        {
            CbDesc.ByteWidth = 16;
            CbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            CbDesc.Usage = D3D11_USAGE_DEFAULT;
        }

        D3D11_SUBRESOURCE_DATA SubRes{};
        {
            SubRes.pSysMem = &TexDesc;
            SubRes.SysMemPitch = sizeof(UINT) * 2;
        }

        if (FAILED(Hr = Device->CreateBuffer(&CbDesc, &SubRes, &Cbv)))
        {
            return Hr;
        }

        ResourcesLoaded = true;

        return S_OK;
    }

    HRESULT UI::InitializeCef(const bool UseOsr)
    {
        HRESULT Hr;

        ID3D11Device* Device = Gi->GetDeviceOriginal();
        ID3D11DeviceContext* Ctx = Gi->GetDeviceContextOriginal();
        ID3D11Device1* Device1;

        if (FAILED(Hr = Device->QueryInterface(IID_PPV_ARGS(&Device1))))
        {
            return Hr;
        }

        App = new UiApp();

        try
        {
            Client = new UIClient(
                Utility,
                new UiRenderHandler(Device1, Ctx, &Rtv, UseOsr),
                new UiLoadHandler(Utility));
        }
        catch (const HrException& Ex)
        {
            Logger::Error() << Ex.what();
            return Ex.Result();
        }

        const CefMainArgs MainArgs(nullptr);
        
        int exit_code = CefExecuteProcess(MainArgs, App.get(), nullptr);
        if (exit_code >= 0)
        {
            return exit_code;
        }

        Logger::Info() << "Initializing cef with base directory: " << Globals::BaseDirectory.c_str() << Logger::EndLine;

        CefSettings Settings{};
        {
            Settings.multi_threaded_message_loop = false;
            Settings.windowless_rendering_enabled = true;
            Settings.external_message_pump = true;
            CefString(&Settings.browser_subprocess_path).FromWString(fs::path(Globals::BaseDirectory).append("Browser.exe"));
            CefString(&Settings.locales_dir_path).FromWString(fs::path(Globals::BaseDirectory).append("Resources\\").append("locales\\"));
            CefString(&Settings.resources_dir_path).FromWString(fs::path(Globals::BaseDirectory).append("Resources\\"));
            CefString(&Settings.user_data_path).FromWString(Globals::BaseDirectory);
            CefString(&Settings.log_file).FromWString(fs::path(Globals::BaseDirectory).append("log.txt"));
            CefString(&Settings.root_cache_path).FromWString(Globals::BaseDirectory);
            CefString(&Settings.locale).FromWString(L"en-US");
        }

        if (!CefInitialize(MainArgs, Settings, App.get(), nullptr))
        {
            return E_FAIL;
        }

        RECT WRect, CRect;
        {
            GetWindowRect(Globals::WindowHandle, &WRect);
            GetClientRect(Globals::WindowHandle, &CRect);
        }

        CefWindowInfo WindowInfo{};
        {
            WindowInfo.SetAsWindowless(Globals::WindowHandle);
            //WindowInfo.SetAsPopup(Globals::WindowHandle, "Browser");
            WindowInfo.x = CRect.left;
            WindowInfo.y = CRect.top;
            WindowInfo.width = CRect.right - CRect.left;
            //WindowInfo.width = WRect.right - WRect.left;
            WindowInfo.height = CRect.bottom - CRect.top;
            //WindowInfo.height = WRect.bottom - WRect.top;
            WindowInfo.shared_texture_enabled = UseOsr;
            WindowInfo.windowless_rendering_enabled = true;
            WindowInfo.external_begin_frame_enabled = false;
        }

        CefBrowserSettings BrowserSettings{};
        {
            BrowserSettings.windowless_frame_rate = 60;
            BrowserSettings.webgl = cef_state_t::STATE_ENABLED;
        }

        if (!CefBrowserHost::CreateBrowser(WindowInfo, Client.get(), "file:///dist/index.html", BrowserSettings, NULL, 0))
        {
            return E_FAIL;
        }

        WindowLoaded = true;
        WindowOpened = true;

        return S_OK;
    }

	bool UI::WindowAcceptsInput(int X, int Y) const
	{
        if (!WindowOpened)
        {
            return false;
        }

        if (MouseCaptureAlphaTest)
        {
            return MouseCaptured;
        }

        return true;
	}

    bool UI::CheckMouseCaptured(int X, int Y)
    {
        if (!ViewTextureCPU)
        {
            return false;
        }

        RECT Rect = { 0 };

        if (GetWindowRect(Globals::WindowHandle, &Rect))
        {
            X -= Rect.left;
            Y -= Rect.top;
        }
        else
        {
            return false;
        }

        D3D11_TEXTURE2D_DESC TexDesc;
        ViewTexture->GetDesc(&TexDesc);

        if (X > -1 &&
            Y > -1 &&
            X < static_cast<int>(TexDesc.Width) - 1 &&
            Y < static_cast<int>(TexDesc.Height) - 1)
        {
            auto Ctx = Gi->GetDeviceContextOriginal();

            const D3D11_BOX Box
            {
                static_cast<unsigned int>(X),
                static_cast<unsigned int>(Y),
                0,
                static_cast<unsigned int>(X) + 1,
                static_cast<unsigned int>(Y) + 1,
                1
            };

            Ctx->CopySubresourceRegion(ViewTextureCPU.Get(), 0, 0, 0, 0, ViewTexture.Get(), 0, &Box);

            D3D11_MAPPED_SUBRESOURCE Mapped;

            if (FAILED(Ctx->Map(ViewTextureCPU.Get(), 0, D3D11_MAP_READ, 0, &Mapped)))
            {
                return false;
            }

            const uint8_t* Data = reinterpret_cast<uint8_t*>(Mapped.pData);
            const uint8_t Alpha = Data[3];

            Ctx->Unmap(ViewTextureCPU.Get(), 0);

            return Alpha > 0;
        }

        return false;
    }

	bool UI::HandleMouseButton(CefBrowserHost::MouseButtonType Button, int X, int Y, bool KeyUp)
	{
        if (!Client)
        {
            return false;
        }

        auto* Browser = Client->Browser();
        if (!Browser)
        {
            return false;
        }

        if (!WindowAcceptsInput(X, Y))
        {
            return false;
        }

        POINT Point{ X, Y };
        {
            ClientToScreen(Globals::WindowHandle, &Point);
        }

        RECT WRect, CRect;
        {
            GetWindowRect(Globals::WindowHandle, &WRect);
            GetClientRect(Globals::WindowHandle, &CRect);
        }

        CefMouseEvent Event;
        {
            Event.x = X - CRect.left;
            Event.y = Y - CRect.top;
        }

        Browser->GetHost()->SendMouseClickEvent(Event, Button, KeyUp, 1);

        return true;
	}

	bool UI::HandleMouseMove(WPARAM Modifiers, int X, int Y, int Z)
	{
        MousePos = IntPoint{ X, Y };

        if (!Client)
        {
            return false;
        }

        auto* Browser = Client->Browser();
        if (!Browser)
        {
            return false;
        }

        if (!WindowAcceptsInput(X, Y))
        {
            return false;
        }

        POINT Point{ X, Y };
        {
            ClientToScreen(Globals::WindowHandle, &Point);
        }

        RECT WRect, CRect;
        {
            GetWindowRect(Globals::WindowHandle, &WRect);
            GetClientRect(Globals::WindowHandle, &CRect);
        }

        CefMouseEvent Event;
        {
            Event.x = X;
            Event.y = Y;
            Event.modifiers = GetCefMouseModifiers(Modifiers);
        }

        Browser->GetHost()->SendMouseMoveEvent(Event, false);

        return true;
	}

	bool UI::HandleMouseWheel(WPARAM Modifiers, int X, int Y, int Z)
	{
        if (!Client)
        {
            return false;
        }

        auto* Browser = Client->Browser();
        if (!Browser)
        {
            return false;
        }

        if (!WindowAcceptsInput(X, Y))
        {
            return false;
        }

        POINT Point{ X, Y };
        {
            ClientToScreen(Globals::WindowHandle, &Point);
        }

        RECT WRect, CRect;
        {
            GetWindowRect(Globals::WindowHandle, &WRect);
            GetClientRect(Globals::WindowHandle, &CRect);
        }

        CefMouseEvent Event;
        {
            Event.x = X - CRect.left;
            Event.y = Y - CRect.top;
            Event.modifiers = GetCefMouseModifiers(Modifiers);
        }

        Browser->GetHost()->SendMouseWheelEvent(Event, 0, Z);

        return true;
	}

	bool UI::HandleKeyboardInput(WPARAM Key, LPARAM Params, UINT MsgId)
	{
        if (!Client)  
        {
            return false;
        }

        auto* Browser = Client->Browser();
        if (!Browser)
        {
            return false;
        }

        if (Key == VK_F6 && MsgId == WM_KEYDOWN)
        {
            WindowOpened = !WindowOpened;
            return true;
        }

        if (!WindowOpened)
        {
            return false;
        }

        CefKeyEvent Event;
        {
            switch (MsgId)
            {
            case WM_KEYUP:
                Event.type = KEYEVENT_KEYUP;
                break;
            case WM_KEYDOWN:
                Event.type = KEYEVENT_RAWKEYDOWN;
                break;
            }

            Event.focus_on_editable_field = true;
            Event.native_key_code = static_cast<int>((MapVirtualKeyW(static_cast<UINT>(Key), MAPVK_VK_TO_VSC) << 16) | 1);
            Event.windows_key_code = static_cast<int>(Key);
            Event.modifiers = GetCefKeyboardModifiers(Key, Params);
        }

        Browser->GetHost()->SendKeyEvent(Event);

        const uint8_t KeyStates[256] = {};
        uint16_t Ascii = 0;

        if (MsgId == WM_KEYDOWN && ToAscii(static_cast<UINT>(Key), Event.native_key_code, KeyStates, &Ascii, 0) > 0)
        {
            Event.character = Event.unmodified_character = Ascii;
            Event.windows_key_code = static_cast<int>(Ascii);
            Event.type = KEYEVENT_CHAR;
            Browser->GetHost()->SendKeyEvent(Event);
        }

        return true;
	}

    void UI::RenderOverlay(GameRenderer* Renderer)
    {
        if (WindowLoaded &&
            ResourcesLoaded)
        {
            if (MousePosPrev != MousePos)
            {
                MousePosPrev = MousePos;
                MouseCaptured = CheckMouseCaptured(MousePos.X, MousePos.Y);
            }

            const bool HasBrowser = Client->Browser();

            CefDoMessageLoopWork();

            if (Renderer)
            {
                auto* FinalGame = Renderer->FinalTextureGame;
                auto* FinalUi = Renderer->FinalTextureUi;

                if (FinalGame &&
                    FinalUi)
                {
                    auto* Ctx = Gi->GetDeviceContextOriginal();

                    ID3D11RenderTargetView* RenderTargets[] = { FinalUi->RawRtv() };
                    {
                        Ctx->OMSetRenderTargets(_countof(RenderTargets), RenderTargets, 0);
                    }

                    const auto Vp = DxRenderer::Instance()->Screen->GetD3D11Viewport();

                    Ctx->RSSetViewports(1, &Vp);
                    Ctx->PSSetConstantBuffers(0, 1, Cbv.GetAddressOf());

                    FsRenderer->Draw(Ctx, Srv.Get(), FinalGame->RawSrv());
                }
            }

            if (HasBrowser && !Client->Browser())
            {
                CefShutdown();
                WindowLoaded.store(false, std::memory_order_release);
            }
        }
        else
        {
            HRESULT Hr;

            if (FAILED(Hr = Initialize()))
            {
                Logger::Error() << "Initialization failed: " << std::to_string(Hr) << Logger::EndLine;
            }
        }
    }

    bool UI::Update()
    {
        return WindowLoaded;
    }
    
    bool UI::UpdateInput()
    {
        return WindowLoaded;
    }
    
    void UI::Shutdown()
    {
        if (!Client ||
            !Client->Browser())
        {
            return;
        }

        Client->Browser()->GetHost()->CloseBrowser(true);
    }
}