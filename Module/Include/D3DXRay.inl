#pragma once

#include "D3DRenderer.h"

namespace D3D
{
	template<size_t NumViews>
	XRayDepthRenderer<NumViews>::XRayDepthRenderer()
		: Dsvs{}
		, Rtvs{}
		, Dss{}
		, StencilRef{}
		, ViewDimension{}
	{}

	template<size_t NumViews>
	void XRayDepthRenderer<NumViews>::Initialize(const XRayMaterial& Mat)
	{
		CD3D11_DEPTH_STENCIL_DESC Desc{ D3D11_DEFAULT };
		{
			Desc.StencilEnable = true;
			Desc.DepthEnable = true;
			Desc.StencilWriteMask = 0xFF;
			Desc.StencilReadMask = 0;
			Desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			Desc.BackFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;
			Desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			Desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			Desc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;
			Desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			Desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		}

		ThrowOnError(Dev->CreateDepthStencilState(&Desc, &IntermediateDss));
		ThrowOnError(Dev->CreateDeferredContext(0, &DeferredContext));

		Material = Mat;
	}

	template<size_t NumViews>
	void XRayDepthRenderer<NumViews>::Prepare(const RenderState& State)
	{
		if (!Material)
		{
			return;
		}

		const auto& Rs = static_cast<const RenderStateFinal&>(State);
		
		const auto Vp = IntPoint
		{ 
			GameRenderer::Instance()->FinalTextureWidth, 
			GameRenderer::Instance()->FinalTextureHeight 
		};

		if (Vp != ViewDimension)
		{
			try
			{
				ResourceSync(Dev, Vp);
			}
			catch (const std::exception& Ex)
			{
				Logger::Error() << Ex.what() << Logger::EndLine;
				return;
			}
		}

		std::array<ID3D11ShaderResourceView*, NumViews> Srvs = {};

		for (size_t N = 0; N < NumViews; ++N)
		{
			Srvs[N] = IntermediateViews[N].Srv.Get();
		}

		Material->Apply(DeferredContext.Get(), Rs.DepthCopy.Srv.Get(), Srvs, Normals.Srv.Get());

		ID3D11RenderTargetView* Rtvs[NumRtvs] =
		{
			Rs.Textures[0]->RawRtv(),
			Rs.Textures[1]->RawRtv(),
			Rs.Textures[2]->RawRtv(),
			Rs.Textures[3]->RawRtv(),
		};

		if (Dss)
		{
			DeferredContext->OMSetDepthStencilState(Dss, StencilRef);
		}

		D3D11_VIEWPORT Vp3d = { 0, 0, static_cast<float>(Vp.X), static_cast<float>(Vp.Y), 0.0f, 1.0f };

		DeferredContext->PSSetConstantBuffers(0, 1, &State.ConstantBuffer);
		DeferredContext->RSSetViewports(1, &Vp3d);
		DeferredContext->OMSetRenderTargets(NumRtvs, Rtvs, Rs.Depth->RawDsv());
		DeferredContext->Draw(3, 0);

		for (auto& View : IntermediateViews)
		{
			DeferredContext->ClearDepthStencilView(View.Dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0);
		}

		const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		DeferredContext->ClearRenderTargetView(Normals.Rtv.Get(), ClearColor);
	}

	template<size_t NumViews>
	void XRayDepthRenderer<NumViews>::Render(GameRenderer* Renderer)
	{
		ComPtr<ID3D11CommandList> CmdList;
		
		HRESULT Hr;

		if (FAILED(Hr = DeferredContext->FinishCommandList(TRUE, &CmdList)))
		{
			Logger::Error() << "FinishCommandList error: " << std::to_string(Hr) << Logger::EndLine;
		}
		else
		{
			Ctx->ExecuteCommandList(CmdList.Get(), TRUE);
		}
	}

	template<size_t NumViews>
	void XRayDepthRenderer<NumViews>::ApplyViews(ID3D11DeviceContext* Ctx, size_t Index)
	{
		Ctx->OMGetRenderTargets(NumRtvs, Rtvs, &Dsvs);
		Ctx->OMGetDepthStencilState(&Dss, &StencilRef);
		Ctx->OMSetDepthStencilState(IntermediateDss.Get(), StencilRef);

		ID3D11RenderTargetView* NewRtvs[NumRtvs] = { Normals.Rtv.Get(), nullptr, nullptr, nullptr };
		{
			Ctx->OMSetRenderTargets(NumRtvs, NewRtvs, IntermediateViews[Index].Dsv.Get());
		}
	}

	template<size_t NumViews>
	void XRayDepthRenderer<NumViews>::RestoreViews(ID3D11DeviceContext* Ctx, size_t Index)
	{
		Ctx->OMSetDepthStencilState(Dss, StencilRef);
		Ctx->OMSetRenderTargets(NumRtvs, Rtvs, Dsvs);
	}

	template<size_t NumViews>
	void XRayDepthRenderer<NumViews>::ResourceSync(ID3D11Device* Dev, IntPoint Vp)
	{
		for (auto& View : IntermediateViews)
		{
			const CD3D11_TEXTURE2D_DESC TexDesc(DXGI_FORMAT_R32_TYPELESS, Vp.X, Vp.Y, 1, 1, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, 1, 0, 0);
			{
				ThrowOnError(Dev->CreateTexture2D(&TexDesc, nullptr, &View.Tex));
			}

			const CD3D11_SHADER_RESOURCE_VIEW_DESC SrvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT, 0, 1);
			{
				ThrowOnError(Dev->CreateShaderResourceView(View.Tex.Get(), &SrvDesc, &View.Srv));
			}

			const CD3D11_DEPTH_STENCIL_VIEW_DESC DsvDesc(View.Tex.Get(), D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D32_FLOAT);
			{
				ThrowOnError(Dev->CreateDepthStencilView(View.Tex.Get(), &DsvDesc, &View.Dsv));
			}
		}

		const CD3D11_TEXTURE2D_DESC TexNormalsDesc(DXGI_FORMAT_R10G10B10A2_UNORM, Vp.X, Vp.Y, 1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, 1, 0, 0);
		{
			ThrowOnError(Dev->CreateTexture2D(&TexNormalsDesc, nullptr, &Normals.Tex));
			ThrowOnError(Dev->CreateRenderTargetView(Normals.Tex.Get(), nullptr, &Normals.Rtv));
			ThrowOnError(Dev->CreateShaderResourceView(Normals.Tex.Get(), nullptr, &Normals.Srv));
		}

		ViewDimension = Vp;
	}

	XRayMaterial::XRayMaterial(ID3D11Device* Dev, Shader<ID3D11PixelShader> Ps)
		: FsQuadRenderMaterial(Dev)
		, Ps(Ps)
	{
		Initialize(Dev);
	}

	template<size_t NumViews>
	void XRayMaterial::Apply(ID3D11DeviceContext* Ctx, ID3D11ShaderResourceView* DepthScene, const std::array<ID3D11ShaderResourceView*, NumViews>& DepthCmp, ID3D11ShaderResourceView* SrvNormals) const
	{
		FsQuadRenderMaterial::Apply(Ctx);

		const float Blend[4] = { 0, 0, 0, 0 };

		Ctx->OMSetBlendState(BlendState.Get(), Blend, D3D11_DEFAULT_SAMPLE_MASK);
		Ctx->PSSetShader(Ps.Get(), nullptr, 0);
		Ctx->PSSetSamplers(0, 1, Ss.GetAddressOf());

		ID3D11ShaderResourceView* Views[2 + NumViews];
		{
			Views[0] = DepthScene;
			Views[1] = SrvNormals;

			for (size_t N = 0; N < NumViews; ++N)
			{
				Views[N + 2] = DepthCmp[N];
			}
		}

		Ctx->PSSetShaderResources(0, _countof(Views), Views);
	}

	void XRayMaterial::Initialize(ID3D11Device* Dev)
	{
		CD3D11_BLEND_DESC BlendDesc{ D3D11_DEFAULT };
		{
#if XRAY_ALPHA
			D3D11_RENDER_TARGET_BLEND_DESC& RenderTargetBlendDesc = BlendDesc.RenderTarget[0];
			{
				RenderTargetBlendDesc.BlendEnable = TRUE;

				RenderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				RenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;

				RenderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				RenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

				RenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
				RenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			}
#endif
		}

		ThrowOnError(Dev->CreateBlendState(&BlendDesc, &BlendState));

		CD3D11_SAMPLER_DESC SamplerDesc{ D3D11_DEFAULT };
		{
			SamplerDesc.MaxAnisotropy = 16;
		}

		ThrowOnError(Dev->CreateSamplerState(&SamplerDesc, &Ss));
	}
}