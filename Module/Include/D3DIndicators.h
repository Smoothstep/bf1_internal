#pragma once

#include "D3DCommon.h"

#include <dwrite.h>
#include <d2d1.h>

namespace D3D
{
	struct IndicatorOptions
	{
		IndicatorOptions() noexcept = default;
		constexpr IndicatorOptions(const Default&) noexcept;

		bool IgnoreSameTeam : 1;
		bool IgnoreVehicles : 1;
		bool IgnoreSoldiers : 1;
		bool Nametags : 1;
		bool BBox : 1;
		bool Bones : 1;
	};

	class RenderIndicators : public RenderPass
	{
	public:
		static constexpr EType Type = Merge;

	public:
		RenderIndicators();
		~RenderIndicators();

		template<class TFn>
		constexpr void Configure(TFn&& Fn)
		{
			std::invoke(Fn, Options);
		}

		void Prepare(const RenderState& State) override;
		void Render(GameRenderer* Renderer) override;
		void Initialize(GameInterface* Gi);

	protected:
		void Indicate(const RenderState& State, const PhysicsEntityBase* Entity,
			const Vector3f& TargetExtent, const Vector3f& Color, const std::wstring_view Tag = {});

	private:
		void PrepareResources();

		std::vector<ColorVertex>			Vertices;
		Shader<ID3D11VertexShader>			BoxVertexShader;
		Shader<ID3D11PixelShader>			BoxPixelShader;
		ComPtr<ID3D11Buffer>				BoxVertexBuffer;
		ComPtr<ID3D11Buffer>				BoxIndexBuffer;
		ComPtr<ID3D11RenderTargetView>		RenderTarget;
		ComPtr<ID3D11BlendState>			BlendState; 

		std::unique_ptr<FsQuadAlphaRenderer>NametagsOverlayRenderer;

		ComPtr<ID3D11Buffer>				ConstantBuffer;

		ComPtr<ID3D11RenderTargetView>		NametagsRtv;
		ComPtr<ID3D11ShaderResourceView>	NametagsSrv;
		ComPtr<ID3D11Texture2D>				NametagsTex;
		ComPtr<IDXGIKeyedMutex>				NametagsMutex;
		ComPtr<ID3D11Texture2D>				NametagsSharedTexture;

		ComPtr<IDXGIResource>				DXGIResource;
		ComPtr<IDXGISurface>				DXGISurface;

		ComPtr<IDWriteFactory>				DWriteFactory;
		ComPtr<ID2D1Factory>				D2DFactory;
		ComPtr<IDWriteTextFormat>			TextFormat;
		ComPtr<ID2D1RenderTarget>			NametagsRt;
		ComPtr<ID2D1SolidColorBrush>		BlackBrush;

		IndicatorOptions					Options;

		GameInterface*						Gi;
	};
}