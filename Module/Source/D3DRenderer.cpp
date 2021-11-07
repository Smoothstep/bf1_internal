#include "stdafx.h"
#include "D3DRenderer.h"
#include "D3DHooks.h"
#include "Shader.h"
#include "Hooks.h"
#include "Offsets.h"
#include "ClientPlayer.h"
#include "RenderFunctions.h"
#include "Game.h"
#include "Crypto.h"

namespace D3D
{
	struct LightingRenderViews
	{
		enum EViewType
		{
			Depth = 2,
			Normals = 3,
			Albedo = 4,
			Spec = 5,
			Class = 6,
			NumViews = 23
		};

		const void* Unk;
		Dx11RenderTarget* RenderTarget;
		Dx11Texture* ShaderTextures[EViewType::NumViews];
	};

	RenderPass::~RenderPass()
	{}

	Renderer::Renderer()
		: CurrentVp{ 0, 0 }
		, ShaderConstants{ GDefault }
	{
		auto* Dev = DxRenderer::Instance()->Device;

		D3D11_BUFFER_DESC BufferDesc = {};
		{
			BufferDesc.ByteWidth = sizeof(GlobalShaderConstants);
			BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}

		ThrowOnError(Dev->CreateBuffer(&BufferDesc, nullptr, &ConstantBuffer));
	}

	const GlobalShaderConstants& Renderer::GShaderConstants() const
	{
		return ShaderConstants;
	}

	GlobalShaderConstants& Renderer::GShaderConstants()
	{
		return ShaderConstants;
	}
	
	bool Renderer::RemovePass(const RenderPass* Pass)
	{
		for (auto& P : Passes)
		{
			auto Lock = P.Lock.Acquire();

			const size_t Sz = P.size();

			P.erase(std::remove_if(P.begin(), P.end(), [Pass](std::shared_ptr<RenderPass>& RPass)
			{
				return Pass == RPass.get();
			}), P.end());

			if (Sz < P.size())
			{
				return true;
			}
		}

		return false;
	}

	void Renderer::ResourceSync(ID3D11Device* Dev, IntPoint Vp)
	{
		CD3D11_TEXTURE2D_DESC TexSceneDesc(DXGI_FORMAT_R32G8X24_TYPELESS, Vp.X, Vp.Y, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, 1, 0, 0);
		{
			ThrowOnError(Dev->CreateTexture2D(&TexSceneDesc, nullptr, &DepthCopy.Tex));
		}

		CD3D11_SHADER_RESOURCE_VIEW_DESC SrvSceneDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS, 0, 1);
		{
			ThrowOnError(Dev->CreateShaderResourceView(DepthCopy.Tex.Get(), &SrvSceneDesc, &DepthCopy.Srv));
		}

		CurrentVp = Vp;
	}

	void Renderer::UpdateConstants(GameRenderer* Renderer)
	{
		ShaderConstants.ScreenSize = DxRenderer::Instance()->Screen->GetViewport();
		ShaderConstants.WorldViewProjection = Renderer->RenderView->ViewProjectionTransposed;
		ShaderConstants.WorldViewProjectionInverse = Renderer->RenderView->ViewProjectionInvTransposed;
		ShaderConstants.View = Renderer->RenderView->View;
		ShaderConstants.ViewInverse = Renderer->RenderView->ViewInv;
		ShaderConstants.CameraOrigin = Renderer->RenderView->View.GetOrigin();
	}

	void Renderer::RenderMerge(GameRenderer* Renderer)
	{
		const RenderStateMerge Rs{ { ShaderConstants, Renderer->FinalRtGame->RawRtv(), ConstantBuffer.Get() }, Renderer->FinalTextureUi, Renderer->FinalTextureGame };

		const auto Lock = Passes[RenderPass::Merge].Lock.Acquire();

		try
		{
			for (auto& Pass : Passes[RenderPass::Merge])
			{
				Pass->Prepare(Rs);
			}

			for (auto& Pass : Passes[RenderPass::Merge])
			{
				Pass->Render(Renderer);
			}
		}
		catch (const std::exception& Ex)
		{
			Logger::Error() << Ex.what() << Logger::EndLine;
		}
	}

	void Renderer::RenderFinal(GameRenderer* Renderer, Dx11RenderTarget* Rt, Dx11Texture** Textures, Dx11Texture* Depth)
	{
		auto* DxRenderer = DxRenderer::Instance();

		const IntPoint Vp = { Renderer->FinalTextureWidth, Renderer->FinalTextureHeight };

		if (CurrentVp != Vp)
		{
			try
			{
				ResourceSync(DxRenderer->Device, Vp);
			}
			catch (const std::exception& Ex)
			{
				Logger::Error() << Ex.what() << Logger::EndLine;
				return;
			}
		}

		UpdateConstants(Renderer);

		DxRenderer->Context->CopyResource(DepthCopy.Tex.Get(), Depth->RawTexture());

		const RenderStateFinal Rs{ { ShaderConstants, Rt->RawRtv(), ConstantBuffer.Get() }, DepthCopy, Textures, Depth };

		const auto Lock = Passes[RenderPass::Final].Lock.Acquire();

		try
		{
			for (auto& Pass : Passes[RenderPass::Final])
			{
				Pass->Prepare(Rs);
			}

			for (auto& Pass : Passes[RenderPass::Final])
			{
				Pass->Render(Renderer);
			}
		}
		catch (const std::exception& Ex)
		{
			Logger::Error() << Ex.what() << Logger::EndLine;
		}
	}

	void Renderer::RenderPostProcess(ID3D11RenderTargetView* Rtv, GameRenderer* Renderer)
	{
		auto* Ctx = DxRenderer::Instance()->Context;
		
		if (ConstantBuffer)
		{
			D3D11_MAPPED_SUBRESOURCE Mapped = {};
			HRESULT Hr;

			if (SUCCEEDED(Hr = Ctx->Map(ConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped)))
			{
				memcpy(Mapped.pData, &ShaderConstants, sizeof(ShaderConstants));
				Ctx->Unmap(ConstantBuffer.Get(), 0);
			}
			else
			{
				Logger::Warn() << "Failed to map shader constants with error: " << std::to_string(Hr) << Logger::EndLine;
			}
		}

		const RenderState Rs{ ShaderConstants, Rtv, ConstantBuffer.Get() };
		
		const auto Lock = Passes[RenderPass::PostProcess].Lock.Acquire();

		try
		{
			for (auto& Pass : Passes[RenderPass::PostProcess])
			{
				Pass->Prepare(Rs);
			}

			for (auto& Pass : Passes[RenderPass::PostProcess])
			{
				Pass->Render(Renderer);
			}
		}
		catch (const std::exception& Ex)
		{
			Logger::Error() << Ex.what() << Logger::EndLine;
		}
	}

	void* FbRenderer::RenderLighting(void* Arg0, LightingRenderViews* Views, bool Arg2, void* Arg3)
	{
		auto Hk = HookRefSpec<GameInterface*>::Current();
		auto& [Gi] = Hk.Tuple();

		if (Gi->ShouldRenderVisuals())
		{
			Renderer* R = Gi->GetRenderer();

			if (R)
			{
				R->RenderFinal(GameRenderer::Instance(),
					Views->RenderTarget,
					&Views->ShaderTextures[LightingRenderViews::Normals],
					Views->ShaderTextures[LightingRenderViews::Depth]);
			}
		}

		return (this->*Hk->GetOriginal<decltype(&FbRenderer::RenderLighting)>())(Arg0, Views, Arg2, Arg3);
	}
	
	void* FbRenderer::RenderMerge(void* Arg0, void* Arg1, void* Arg2, Vector4f Arg3)
	{
		auto Hk = HookRefSpec<GameInterface*>::Current();
		auto& [Gi] = Hk.Tuple();

		if (Gi->ShouldRenderVisuals())
		{
			Renderer* R = Gi->GetRenderer();

			if (R)
			{
				R->RenderMerge(GameRenderer::Instance());
			}
		}

		return (this->*Hk->GetOriginal<decltype(&FbRenderer::RenderMerge)>())(Arg0, Arg1, Arg2, Arg3);
	}
}