#pragma once

#include <d3d11.h>
#include <string_view>
#include <wrl/client.h>

using namespace Microsoft::WRL;

class D3DBlob
{
public:
	D3DBlob(ID3DBlob* Blob = nullptr) noexcept
		: Blob{ Blob }
	{}

	operator std::string_view() const
	{
		if (!Blob)
		{
			return {};
		}

		return { reinterpret_cast<const char*>(Blob->GetBufferPointer()), Blob->GetBufferSize() };
	}

	ID3DBlob* operator -> () const
	{
		return Blob.Get();
	}

	operator bool() const
	{
		return Blob.Get() != nullptr;
	}

	operator ID3DBlob* () const
	{
		return Blob.Get();
	}

	ID3DBlob** operator&()
	{
		return &Blob;
	}

	ID3DBlob* Get() const
	{
		return Blob.Get();
	}

private:
	ComPtr<ID3DBlob> Blob;
};