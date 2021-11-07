#include "stdafx.h"
#include "D3DIndicators.h"
#include "Crypto.h"
#include "ClientPlayer.h"
#include "D3DHooks.h"

#include <string_view>

using namespace std::string_view_literals;

namespace D3D
{
	constexpr IndicatorOptions::IndicatorOptions(const Default&) noexcept
		: IgnoreSameTeam(true)
		, IgnoreVehicles(false)
		, IgnoreSoldiers(false)
		, Nametags(false)
		, BBox(false)
		, Bones(false)
	{}

	RenderIndicators::RenderIndicators()
		: Options(GDefault)
	{
		ThrowOnError(DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(DWriteFactory.GetAddressOf())
		));

		ThrowOnError(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			D2DFactory.GetAddressOf()
		));

		ThrowOnError(DWriteFactory->CreateTextFormat(
			L"Tahoma",
			NULL,
			DWRITE_FONT_WEIGHT_LIGHT,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_EXPANDED,
			9.0f,
			L"en-us",
			TextFormat.GetAddressOf()
		));

		ThrowOnError(TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
		ThrowOnError(TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
		
		Vertices.reserve(MaxPlayers * 4);

		UINT Indices[6 * MaxPlayers];

		for (UINT Idx = 0; Idx < MaxPlayers; ++Idx)
		{
			Indices[Idx * 6 + 0] = Idx * 4;
			Indices[Idx * 6 + 1] = Idx * 4 + 1;
			Indices[Idx * 6 + 2] = Idx * 4 + 2;
			Indices[Idx * 6 + 3] = Idx * 4 + 3;
			Indices[Idx * 6 + 4] = Idx * 4;
			Indices[Idx * 6 + 5] = Idx * 4 + 1;
		}

		const D3D11_BUFFER_DESC IBufferDesc =
		{
			sizeof(Indices),
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_INDEX_BUFFER,
			0
		};

		const D3D11_SUBRESOURCE_DATA SubResourceData =
		{
			Indices,
			sizeof(Indices),
			1
		};

		const D3D11_BLEND_DESC BlendDesc =
		{
			FALSE,
			FALSE,
			{
				{
					TRUE,
					D3D11_BLEND_SRC_ALPHA,
					D3D11_BLEND_ONE,
					D3D11_BLEND_OP_ADD,
					D3D11_BLEND_SRC_ALPHA,
					D3D11_BLEND_ONE,
					D3D11_BLEND_OP_ADD,
					D3D11_COLOR_WRITE_ENABLE_ALL
				}
			}
		};

		const D3D11_BUFFER_DESC VBufferDesc =
		{
			MaxPlayers * sizeof(ColorVertex) * 4,
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE
		};

		BoxPixelShader = ShaderManager::Instance().Compile<ID3D11PixelShader>(L"Box.hlsl", "PMain");
		BoxVertexShader = ShaderManager::Instance().Compile<ID3D11VertexShader>(L"Box.hlsl", "VMain", false, {
				ColorVertex::InputElementDescs, _countof(
					ColorVertex::InputElementDescs) 
			});

		ThrowOnError(Dev->CreateBlendState(&BlendDesc, &BlendState));
		ThrowOnError(Dev->CreateBuffer(&IBufferDesc, &SubResourceData, &BoxIndexBuffer));
		ThrowOnError(Dev->CreateBuffer(&VBufferDesc, 0, &BoxVertexBuffer));
	}

	RenderIndicators::~RenderIndicators()
	{}

	void RenderIndicators::Prepare(const RenderState& State)
	{
		ComPtr<ID3D10Blob> VertexShaderCode, PixelShaderCode, ErrMsg;

		ConstantBuffer = State.ConstantBuffer;
		RenderTarget = State.RenderTarget;
		Vertices.clear();

		PrepareResources();

		const ClientPlayer* Local = Crypto::GetLocalPlayer();

		if (!IsValidPointer(Local))
		{
			return;
		}

		if (NametagsRt)
		{
			//ThrowOnError(NametagsMutex->AcquireSync(0, INFINITE));
			NametagsRt->BeginDraw();
			NametagsRt->SetTransform(D2D1::IdentityMatrix());
			NametagsRt->Clear(D2D1::ColorF(D2D1::ColorF::White, 0.0f));
		}

		Crypto::IteratePlayers([=](ClientPlayer* Player)
		{
			if (Player == Local)
			{
				return true;
			}

			if (Options.IgnoreSameTeam)
			{
				if (Player->TeamId == Local->TeamId)
				{
					return true;
				}
			}

			ClientSoldierEntity*Soldier = Player->Soldier;

			if (!IsValidPointer(Soldier))
			{
				return true;
			}

			if (!IsValidPointer(
				Soldier->ClientStatusComponent) ||
				Soldier->ClientStatusComponent->IsDead())
			{
				return true;
			}

			const float Health = Soldier->ClientStatusComponent->Health / Soldier->ClientStatusComponent->HealthMax;

			constexpr Vector3f Color0(0.1f, 0.9f, 0.3f);
			constexpr Vector3f Color1(0.9f, 0.1f, 0.3f);

			const Vector3f Color = Player->TeamId == Local->TeamId ? State.Constants.ColorNeutral : Color0 * Health + Color1 * (1.0f - Health);

			std::wstring Tag;
			
			if (const auto Len = std::strlen(Player->Name); Len > 0)
			{
				Tag.resize(Len);
				MultiByteToWideChar(CP_UTF8, 0, Player->Name, static_cast<int>(Len), Tag.data(), static_cast<int>(Tag.size()));
			}
			
			if (Player->InVehicle())
			{
				Indicate(State, Player->Vehicle, Vector3f{ 1.0f, 1.0f, 1.0f }, Color, Tag);
			}
			else
			{
				Indicate(State, Soldier, Vector3f{ 1.0f, 1.0f, 1.0f }, Color, Tag);
			}

			return true;
		});

		if (NametagsRt)
		{
			NametagsRt->EndDraw();
			//ThrowOnError(NametagsMutex->ReleaseSync(1));
		}
	}

	void RenderIndicators::Indicate(const RenderState& State, const PhysicsEntityBase* Entity,
		const Vector3f& TargetExtent, const Vector3f& Color, const std::wstring_view Tag)
	{
		AABBTransform AABB = Entity->AABB();

		Matrix4x4 Transform = AABB.Transform;

		Vector3f Target = Transform.GetOrigin();
		Vector3f Position = State.Constants.CameraOrigin;
		Vector3f PositionScreen;

		if (!GameRenderer::Instance()->WorldToScreen(Target + Vector3f::UpVector * (AABB.AABB.Max.Y - AABB.AABB.Min.Y + 0.5f + Vector3f::Dist2D(Position, Target) / 1000.0f), PositionScreen))
		{
			return;
		}

		Position.Y += 1.25f;

		const Vector3f Dimension = State.Constants.ScreenSize;

		if (PositionScreen.X < -100 ||
			PositionScreen.X > Dimension.X + 100 ||
			PositionScreen.Y < -100 ||
			PositionScreen.Y > Dimension.Y + 100)
		{
			return;
		}

		if (Options.BBox)
		{
			const float Distance = Vector3f::Distance(Position, Target) * 0.1f / (GameRenderer::Instance()->RenderView->Fov());
			float Xp = 0.15f / Distance;
			float Yp = 0.60f / Distance;

			Xp = Target.X - AABB.AABB.Min.X;
			Yp = Target.Z - AABB.AABB.Min.Z;

			const Vector3f A[4] =
			{
				Target + Vector3f(AABB.AABB.Min.X, AABB.AABB.Max.Y, AABB.AABB.Min.Z),
				Target + Vector3f(AABB.AABB.Max.X, AABB.AABB.Max.Y, AABB.AABB.Max.Z),
				Target + Vector3f(AABB.AABB.Max.X, AABB.AABB.Min.Y, AABB.AABB.Max.Z),
				Target + Vector3f(AABB.AABB.Min.X, AABB.AABB.Min.Y, AABB.AABB.Min.Z)
			};

			const float HalfHeight = (AABB.AABB.Max.Y - AABB.AABB.Min.Y) * 0.5f;
			Target.Y += HalfHeight;

			for (size_t N = 0; N < 4; ++N)
			{
				ColorVertex& Vertex = Vertices.emplace_back();

				Vertex.Position = A[N];

				Vertex.Uv = Vector2f(
					((N + 1) & -2) == 2,
					((N + 0) & -2) != 2);

				Vertex.Color = Color;
				Vertex.Wv.X = Xp * 2.0f;
				Vertex.Wv.Y = Yp * 2.0f;
				Vertex.Origin = Target;
			}
		}

		if (Options.Nametags)
		{
			constexpr float TextHalfW = 50.0f;
			constexpr float TextHalfH = 50.0f;

			if (NametagsRt)
			{
				const float CenterX = PositionScreen.X;
				const float CenterY = State.Constants.ScreenSize.Y - PositionScreen.Y;

				const D2D1_RECT_F LayoutRect = D2D1::RectF(
					CenterX - TextHalfW,
					CenterY - TextHalfH,
					CenterX + TextHalfW,
					CenterY + TextHalfH
				);

				const float Dist = 1.0f - Vector3f::Distance(Position, Target) / 1000.0f;

				BlackBrush->SetOpacity(Math::Clamp(Dist, 0.0f, 1.0f));

				if (Tag.empty())
				{
					const auto Text = L"Unnamed Entity"sv;
					NametagsRt->DrawTextA(Text.data(), static_cast<UINT32>(Text.length()), TextFormat.Get(), LayoutRect, BlackBrush.Get());
				}
				else
				{
					NametagsRt->DrawTextA(Tag.data(), static_cast<UINT32>(Tag.size()), TextFormat.Get(), LayoutRect, BlackBrush.Get());
				}
			}
		}
	}

	void RenderIndicators::Initialize(GameInterface* Gi)
	{
		this->Gi = Gi;
	}

	void RenderIndicators::PrepareResources()
	{
		if (!RenderTarget)
		{
			return;
		}

		if (NametagsRt)
		{
			const auto Sz = NametagsRt->GetPixelSize();

			if (Sz.width  == Dx->Screen->Width &&
				Sz.height == Dx->Screen->Height)
			{
				return;
			}
		}

		D3D11_TEXTURE2D_DESC TexDesc = {};
		{
			TexDesc.Width = Dx->Screen->Width;
			TexDesc.Height = Dx->Screen->Height;
			TexDesc.MipLevels = TexDesc.ArraySize = 1;
			TexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			TexDesc.SampleDesc.Count = 1;
			TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			TexDesc.Usage = D3D11_USAGE_DEFAULT;
			TexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
		{
			SrvDesc.Format = TexDesc.Format;
			SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SrvDesc.Texture2D.MostDetailedMip = 0;
			SrvDesc.Texture2D.MipLevels = 1;
		}

		ThrowOnError(Gi->GetDevice()->CreateTexture2D(&TexDesc, NULL, &NametagsTex));

		ThrowOnError(NametagsTex->QueryInterface(IID_PPV_ARGS(&DXGIResource)));
		ThrowOnError(NametagsTex->QueryInterface(IID_PPV_ARGS(&DXGISurface)));
		//ThrowOnError(NametagsTex->QueryInterface(IID_PPV_ARGS(&NametagsMutex)));

		HANDLE TextureHandle = nullptr;
		ThrowOnError(DXGIResource->GetSharedHandle(&TextureHandle));
		
		if (!TextureHandle)
		{
			return;
		}

		ThrowOnError(Gi->GetDeviceOriginal()->OpenSharedResource(TextureHandle, IID_PPV_ARGS(&NametagsSharedTexture)));
		ThrowOnError(Gi->GetDeviceOriginal()->CreateShaderResourceView(NametagsSharedTexture.Get(), &SrvDesc, &NametagsSrv));

		const auto RtProperties = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_HARDWARE, 
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
		
		ThrowOnError(D2DFactory->CreateDxgiSurfaceRenderTarget(DXGISurface.Get(),
			&RtProperties,
			&NametagsRt));

		if (NametagsRt)
		{
			ThrowOnError(NametagsRt->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF(0.9f, 0.9f, 0.9f, 0.85f)),
				&BlackBrush
			));

			NametagsRt->SetTextAntialiasMode(
				D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

			NametagsOverlayRenderer = std::make_unique<FsQuadAlphaRenderer>(Gi->GetDeviceOriginal());
		}
	}

	void RenderIndicators::Render(GameRenderer* Renderer)
	{
		auto VBuffer = BoxVertexBuffer.Get();
		auto IBuffer = BoxIndexBuffer.Get();

		ID3D11RenderTargetView* RenderTargets[1] = { RenderTarget.Get() };
		{
			Ctx->OMSetRenderTargets(_countof(RenderTargets), RenderTargets, 0);
		}

		const D3D11_VIEWPORT Vp =
		{
			0.f,
			0.f,
			static_cast<float>(Renderer->FinalTextureWidth),
			static_cast<float>(Renderer->FinalTextureHeight),
			0.f,
			1.f
		};

		Ctx->RSSetViewports(1, &Vp);

		if (VBuffer &&
			IBuffer && !Vertices.empty())
		{
			D3D11_MAPPED_SUBRESOURCE Res;

			if (SUCCEEDED(Ctx->Map(VBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Res)))
			{
				memcpy(Res.pData, Vertices.data(), Vertices.size() * sizeof(ColorVertex));
				Ctx->Unmap(VBuffer, 0);

				const UINT Stride = sizeof(ColorVertex);
				const UINT Offset = 0;

				const float Blend[] = { 1.0f, 1.0f, 1.0f, 1.0f };

				Ctx->PSSetShader(BoxPixelShader.Get(), 0, 0);
				Ctx->VSSetShader(BoxVertexShader.Get(), 0, 0);
				Ctx->IASetInputLayout(BoxVertexShader.GetInputLayout());
				Ctx->OMSetBlendState(BlendState.Get(), Blend, 0xFFFFFFFF);
				Ctx->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				Ctx->IASetVertexBuffers(0, 1, &VBuffer, &Stride, &Offset);
				Ctx->IASetIndexBuffer(IBuffer, DXGI_FORMAT_R32_UINT, 0);
				
				if (ConstantBuffer)
				{
					Ctx->VSSetConstantBuffers(0, 1, &ConstantBuffer);
					Ctx->PSSetConstantBuffers(0, 1, &ConstantBuffer);
				}

				for (UINT Idx = 0; Idx < Vertices.size() / 4; ++Idx)
				{
					Ctx->DrawIndexed(6, Idx * 6, 0);
				}
			}
		}

		if (NametagsOverlayRenderer && NametagsSrv)
		{
			Gi->GetDeviceContext()->Flush();

			//NametagsMutex->AcquireSync(1, INFINITE);
			NametagsOverlayRenderer->Draw(Ctx, NametagsSrv.Get());
			//NametagsMutex->ReleaseSync(0);
		}
	}
}