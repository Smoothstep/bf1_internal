#pragma once

#include <d3dcompiler.h>

#include "Dx11Texture.h"
#include "ClassDecl.h"
#include "Shader.h"
#include "D3DBlob.h"
#include "FnValue.h"
#include "Game.h"

namespace D3D
{
	class ViewState
	{
	protected:
		ViewState() = default;

	public:
		virtual void ApplyViews(ID3D11DeviceContext* Ctx, size_t Index) = 0;
		virtual void RestoreViews(ID3D11DeviceContext* Ctx, size_t Index) = 0;
	};

	struct SceneDepth
	{
		SceneDepth() noexcept = default;

		ComPtr<ID3D11ShaderResourceView> Srv;
		ComPtr<ID3D11Texture2D> Tex;
	};

	struct RenderState
	{
		RenderState(const GlobalShaderConstants& Constants, ID3D11RenderTargetView* RenderTarget, ID3D11Buffer* Cb = nullptr);

		const GlobalShaderConstants& Constants;
		ID3D11RenderTargetView* RenderTarget;
		ID3D11Buffer* ConstantBuffer;
	};

	struct RenderStateFinal : public RenderState
	{
		RenderStateFinal(RenderState&& State, const SceneDepth& DepthCopy, Dx11Texture** Textures, Dx11Texture* Depth);

		Dx11Texture** Textures;
		Dx11Texture* Depth;

		const SceneDepth& DepthCopy;
	};

	struct RenderStateMerge : public RenderState
	{
		RenderStateMerge(RenderState&& State, Dx11Texture* Ui, Dx11Texture* Game);

		Dx11Texture* TextureUi;
		Dx11Texture* TextureGame;
	};

	class FsQuadRenderMaterial
	{
	public:
		virtual ~FsQuadRenderMaterial();
		FsQuadRenderMaterial(ID3D11Device* Dev);

	public:
		void Apply(ID3D11DeviceContext* Ctx) const;

	private:
		Shader<ID3D11VertexShader> Vs;
		ComPtr<ID3D11DepthStencilState> Dss;
		ComPtr<ID3D11RasterizerState> Rss;
	};

	class FsQuadAlphaRenderer
	{
	public:
		FsQuadAlphaRenderer(ID3D11Device* Dev, bool Reverse = false);

	public:
		void Draw(ID3D11DeviceContext* Ctx, ID3D11ShaderResourceView* Srv) const;

	private:
		ComPtr<ID3D11RasterizerState>		CullNone;
		ComPtr<ID3D11SamplerState>			SamplerState;
		ComPtr<ID3D11BlendState>			BlendState;
		Shader<ID3D11PixelShader>			PixelShader;
		FsQuadRenderMaterial				Material;
	};

	class FsBlurAlphaRenderer
	{
	public:
		FsBlurAlphaRenderer(ID3D11Device* Dev, bool Reverse = false);

	public:
		void Draw(ID3D11DeviceContext* Ctx, ID3D11ShaderResourceView* Srv, ID3D11ShaderResourceView* BlurView) const;

	private:
		ComPtr<ID3D11RasterizerState>		CullNone;
		ComPtr<ID3D11SamplerState>			SamplerState;
		ComPtr<ID3D11BlendState>			BlendState;
		Shader<ID3D11PixelShader>			PixelShader;
		FsQuadRenderMaterial				Material;
	};

	class RenderPass : public std::enable_shared_from_this<RenderPass>
	{
	public:
		enum EType
		{
			Final,
			Merge,
			PostProcess,
			NumTypes
		};

		friend class Renderer;

	protected:
		virtual ~RenderPass();

		FnValue<DxRenderer*>			Dx = { [=]() { return DxRenderer::Instance(); } };
		FnValue<ID3D11Device*>			Dev = { [=]() { return Dx->Device; } };
		FnValue<ID3D11DeviceContext4*>	Ctx = { [=]() { return Dx->Context; } };

	protected:
		virtual void Prepare(const RenderState& State) = 0;
		virtual void Render(GameRenderer* Renderer) = 0;

		virtual int32_t Priority() const
		{
			return 1;
		}
	};

	template<RenderPass::EType RenderType, size_t RenderPriority = 1>
	class DelegateRenderPass : public RenderPass
	{
		using TState = std::conditional_t<RenderType == RenderPass::EType::Final, RenderStateFinal,
						std::conditional_t<RenderType == RenderPass::EType::Merge, RenderStateMerge, RenderState>>;

		using TFnPrepare = std::function<void(TState)>;
		using TFnRender = std::function<void(GameRenderer*)>;

	public:
		static constexpr RenderPass::EType Type = RenderType;

	public:
		DelegateRenderPass(
			TFnRender&& Render,
			TFnPrepare&& Prepare = {}) noexcept
			: FnPrepare { std::move(Prepare) }
			, FnRender{ std::move(Render) }
		{}

		virtual void Prepare(const RenderState& State) final
		{ 
			if (FnPrepare)
			{
				FnPrepare(static_cast<const TState&>(State));
			}
		};

		virtual void Render(GameRenderer* Renderer) final
		{ 
			if (FnRender)
			{
				FnRender(Renderer);
			}
		};

		virtual int32_t Priority() const override
		{
			return RenderPriority;
		}

	private:
		TFnPrepare FnPrepare;
		TFnRender FnRender;
	};

	struct SwapChainVMTWrapper
	{
		HRESULT(STDMETHODCALLTYPE* GetDevice)(
			IDXGISwapChain* This,
			_In_  REFIID riid,
			_COM_Outptr_  void** ppDevice);

		HRESULT(STDMETHODCALLTYPE* Present)(
			IDXGISwapChain* This,
			UINT SyncInterval,
			UINT Flags);

		HRESULT(STDMETHODCALLTYPE* GetBuffer)(
			IDXGISwapChain* This,
			UINT Buffer,
			_In_  REFIID riid,
			_COM_Outptr_  void** ppSurface);
	};
}