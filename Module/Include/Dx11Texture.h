#pragma once

#include <d3d11_4.h>

class Dx11Texture
{
	union WrView
	{
		ID3D11RenderTargetView* Rtv;
		ID3D11DepthStencilView* Dsv;
	};

	char Pad[0x90];
	ID3D11Texture2D* Texture;
	ID3D11ShaderResourceView* Srv;
	ID3D11UnorderedAccessView* Uav;
	WrView Targets[4];

public:
	ID3D11RenderTargetView* RawRtv(size_t Index = 0)
	{
		return Targets[Index].Rtv;
	}

	ID3D11DepthStencilView* RawDsv(size_t Index = 0)
	{
		return Targets[Index].Dsv;
	}

	ID3D11ShaderResourceView* RawSrv()
	{
		return Srv;
	}

	ID3D11Texture2D* RawTexture()
	{
		return Texture;
	}
};
