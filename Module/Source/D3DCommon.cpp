#include "stdafx.h"
#include "D3DCommon.h"

namespace D3D
{
	RenderState::RenderState(const GlobalShaderConstants& Constants, ID3D11RenderTargetView* RenderTarget, ID3D11Buffer* Cb)
		: Constants{ Constants }
		, RenderTarget{ RenderTarget }
		, ConstantBuffer{ Cb }
	{}

	RenderStateFinal::RenderStateFinal(RenderState&& State, const SceneDepth& DepthCopy, Dx11Texture** Textures, Dx11Texture* Depth)
		: RenderState{ std::move(State) }
		, Textures{ Textures }
		, Depth{ Depth }
		, DepthCopy{ DepthCopy }
	{}

	RenderStateMerge::RenderStateMerge(RenderState&& State, Dx11Texture* Ui, Dx11Texture* Game)
		: RenderState{ std::move(State) }
		, TextureUi{ Ui }
		, TextureGame{ Game }
	{}

	FsQuadRenderMaterial::~FsQuadRenderMaterial() {}

	FsQuadRenderMaterial::FsQuadRenderMaterial(ID3D11Device* Dev)
		: Vs{ ShaderManager::Instance().Compile<ID3D11VertexShader>(L"Quad.hlsl", "VSMain2") }
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

		ThrowOnError(Dev->CreateDepthStencilState(&Desc, &Dss));

		CD3D11_RASTERIZER_DESC RssDesc{ D3D11_DEFAULT };
		{
			RssDesc.CullMode = D3D11_CULL_NONE;
			RssDesc.MultisampleEnable = FALSE;
			RssDesc.AntialiasedLineEnable = FALSE;
			RssDesc.DepthClipEnable = FALSE;
		}

		ThrowOnError(Dev->CreateRasterizerState(&RssDesc, &Rss));
	}

	void FsQuadRenderMaterial::Apply(ID3D11DeviceContext* Ctx) const
	{
		Ctx->IASetInputLayout(nullptr);
		Ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		Ctx->OMSetDepthStencilState(Dss.Get(), 0);
		Ctx->RSSetState(Rss.Get());
		Ctx->VSSetShader(Vs.Get(), nullptr, 0);
		Ctx->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
	}

	FsQuadAlphaRenderer::FsQuadAlphaRenderer(ID3D11Device* Dev, bool Reverse)
		: Material{ Dev }
	{
		if (Reverse)
		{
			PixelShader = ShaderManager::Instance().Compile<ID3D11PixelShader>(L"Sample.hlsl", "MainReverse");
		}
		else
		{
			PixelShader = ShaderManager::Instance().Compile<ID3D11PixelShader>(L"Sample.hlsl", "Main");
		}

		D3D11_SAMPLER_DESC SamplerDesc = {};
		{
			SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			SamplerDesc.MaxAnisotropy = 16;
			SamplerDesc.MinLOD = 0;
			SamplerDesc.MaxLOD = 0;
		}

		ThrowOnError(Dev->CreateSamplerState(&SamplerDesc, &SamplerState));

		CD3D11_BLEND_DESC BlendDesc(D3D11_DEFAULT);
		{
			D3D11_RENDER_TARGET_BLEND_DESC& RenderTargetBlendDesc = BlendDesc.RenderTarget[0];
			{
				RenderTargetBlendDesc.BlendEnable = TRUE;

				RenderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				RenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;

				RenderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				RenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

				RenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_MAX;
				RenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_MAX;
			}
		}

		ThrowOnError(Dev->CreateBlendState(&BlendDesc, &BlendState));

		D3D11_RASTERIZER_DESC RasterizerDesc = {};
		{
			RasterizerDesc.FrontCounterClockwise = FALSE;
			RasterizerDesc.CullMode = D3D11_CULL_NONE;
			RasterizerDesc.DepthClipEnable = FALSE;
			RasterizerDesc.FillMode = D3D11_FILL_SOLID;
			RasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
			RasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
			RasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			RasterizerDesc.ScissorEnable = FALSE;
			RasterizerDesc.MultisampleEnable = FALSE;
			RasterizerDesc.AntialiasedLineEnable = FALSE;
		}

		ThrowOnError(Dev->CreateRasterizerState(&RasterizerDesc, &CullNone));
	}

	void FsQuadAlphaRenderer::Draw(ID3D11DeviceContext* Ctx, ID3D11ShaderResourceView* Srv) const
	{
		Material.Apply(Ctx);

		const float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		Ctx->PSSetShader(PixelShader.Get(), 0, 0);
		Ctx->PSSetSamplers(0, 1, SamplerState.GetAddressOf());
		Ctx->PSSetShaderResources(0, 1, &Srv);
		Ctx->OMSetBlendState(BlendState.Get(), BlendFactor, 0xFFFFFFFF);

		Ctx->Draw(3, 0);
	}

	FsBlurAlphaRenderer::FsBlurAlphaRenderer(ID3D11Device* Dev, bool Reverse)
		: Material{ Dev }
	{
		if (Reverse)
		{
			PixelShader = ShaderManager::Instance().Compile<ID3D11PixelShader>(L"Sample.hlsl", "MainBlurReverse");
		}
		else
		{
			PixelShader = ShaderManager::Instance().Compile<ID3D11PixelShader>(L"Sample.hlsl", "MainBlur");
		}

		D3D11_SAMPLER_DESC SamplerDesc = {};
		{
			SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			SamplerDesc.MaxAnisotropy = 16;
			SamplerDesc.MinLOD = 0;
			SamplerDesc.MaxLOD = 0;
		}

		ThrowOnError(Dev->CreateSamplerState(&SamplerDesc, &SamplerState));

		CD3D11_BLEND_DESC BlendDesc(D3D11_DEFAULT);
		{
			D3D11_RENDER_TARGET_BLEND_DESC& RenderTargetBlendDesc = BlendDesc.RenderTarget[0];
			{
				RenderTargetBlendDesc.BlendEnable = TRUE;

				RenderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				RenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;

				RenderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				RenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

				RenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
				RenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			}
		}

		ThrowOnError(Dev->CreateBlendState(&BlendDesc, &BlendState));

		D3D11_RASTERIZER_DESC RasterizerDesc = {};
		{
			RasterizerDesc.FrontCounterClockwise = FALSE;
			RasterizerDesc.CullMode = D3D11_CULL_NONE;
			RasterizerDesc.DepthClipEnable = FALSE;
			RasterizerDesc.FillMode = D3D11_FILL_SOLID;
			RasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
			RasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
			RasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			RasterizerDesc.ScissorEnable = FALSE;
			RasterizerDesc.MultisampleEnable = FALSE;
			RasterizerDesc.AntialiasedLineEnable = FALSE;
		}

		ThrowOnError(Dev->CreateRasterizerState(&RasterizerDesc, &CullNone));
	}

	void FsBlurAlphaRenderer::Draw(ID3D11DeviceContext* Ctx, ID3D11ShaderResourceView* Srv, ID3D11ShaderResourceView* Blur) const
	{
		Material.Apply(Ctx);

		const float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		ID3D11ShaderResourceView* Srvs[2] = { Srv, Blur };

		Ctx->PSSetShader(PixelShader.Get(), 0, 0);
		Ctx->PSSetSamplers(0, 1, SamplerState.GetAddressOf());
		Ctx->PSSetShaderResources(0, _countof(Srvs), Srvs);
		Ctx->OMSetBlendState(BlendState.Get(), BlendFactor, 0xFFFFFFFF);

		Ctx->Draw(3, 0);
	}
}