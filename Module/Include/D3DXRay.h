#pragma once

#include <array>

#include "D3DCommon.h"

namespace D3D
{
	class XRayMaterial : protected FsQuadRenderMaterial
	{
		Shader<ID3D11PixelShader> Ps;
		ComPtr<ID3D11BlendState> BlendState;
		ComPtr<ID3D11SamplerState> Ss;

	public:
		XRayMaterial(ID3D11Device* Dev, Shader<ID3D11PixelShader> Ps);

	public:
		template<size_t NumViews>
		void Apply(ID3D11DeviceContext* Ctx, ID3D11ShaderResourceView* DepthScene,
			const std::array<ID3D11ShaderResourceView*, NumViews>& DepthCmp, ID3D11ShaderResourceView* SrvNormals) const;

	private:
		void Initialize(ID3D11Device* Dev);
	};

	template<size_t NumViews>
	class XRayDepthRenderer : public RenderPass, public ViewState
	{
		static constexpr size_t NumRtvs = 4;

		struct ShaderViews
		{
			ComPtr<ID3D11ShaderResourceView> Srv;
			ComPtr<ID3D11Texture2D> Tex;
		};

		struct DepthViews : public ShaderViews
		{
			ComPtr<ID3D11DepthStencilView> Dsv;
		};

		struct RenderViews : public ShaderViews
		{
			ComPtr<ID3D11RenderTargetView> Rtv;
		};

	public:
		static constexpr EType Type = Final;

	public:
		XRayDepthRenderer();

	public:
		void Initialize(const XRayMaterial& Mat);

		virtual void Prepare(const RenderState& State);
		virtual void Render(GameRenderer* Renderer);

	protected:
		void ApplyViews(ID3D11DeviceContext* Ctx, size_t Index);
		void RestoreViews(ID3D11DeviceContext* Ctx, size_t Index);
		void ResourceSync(ID3D11Device* Dev, IntPoint Vp);

	private:
		std::optional<XRayMaterial> Material;
		std::array<DepthViews, NumViews> IntermediateViews;
		RenderViews Normals;

		ComPtr<ID3D11DepthStencilState> IntermediateDss;
		ComPtr<ID3D11DeviceContext> DeferredContext;

		ID3D11DepthStencilView* Dsvs;
		ID3D11RenderTargetView* Rtvs[NumRtvs];

		ID3D11DepthStencilState* Dss;
		UINT StencilRef;

		IntPoint ViewDimension;
	};
}

#include "D3DXRay.inl"