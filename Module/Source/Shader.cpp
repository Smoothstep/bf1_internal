#include "stdafx.h"
#include "Shader.h"
#include "D3DBlob.h"
#include "Globals.h"
#include "Game.h"

const D3D11_INPUT_ELEMENT_DESC ColorVertex::InputElementDescs[] =
{
	 { "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,								D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "TEXCOORD",	1, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC SimpleVertex::InputElementDescs[] =
{
	 { "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,								D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC NormalVertex::InputElementDescs[] =
{
	 { "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,								D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC TBNVertex::InputElementDescs[] =
{
	 { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,								D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "TEXCOORD",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "TEXCOORD",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

GlobalShaderConstants::GlobalShaderConstants(const Default&)
	: View{ Matrix4x4::Identity() }
	, ViewTransposed{ Matrix4x4::Identity() }
	, ViewInverse{ Matrix4x4::Identity() }
	, WorldViewProjection{ Matrix4x4::Identity() }
	, WorldViewProjectionInverse{ Matrix4x4::Identity() }
	, ScreenSize{}
	, CameraOrigin{}
	, CameraDirection{}
	, ColorAlly{ 0, 0, 1, 1 }
	, ColorAllyOccluded{ 0, 0, 0.5, 0.5 }
	, ColorEnemy{ 1, 0, 0, 1 }
	, ColorEnemyOccluded{ 0.5, 0, 0, 0.5 }
	, ColorNeutral{}
	, ColorNeutralOccluded{}
	, FadeDistance{ 1000.0f }
	, FadeDistanceNameTags{ 1000.0f }
	, FadeDistanceIndicators{ 1000.0f }
	, FadeDistanceEngine{ 1000.0f }
{}

ShaderManager::ShaderManager(const std::wstring_view BaseDir)
	: BaseDir{ BaseDir }
{}

ShaderManager& ShaderManager::Instance()
{
	static ShaderManager Mgr(L"Shaders\\");
	return Mgr;
}

std::wstring ShaderManager::ShaderIdent(const std::wstring_view Path, const std::string_view Entry)
{
	std::wstring Str(Entry.length(), 0);

	MultiByteToWideChar(CP_UTF8, 0, Entry.data(), static_cast<int>(Entry.length()), 
		Str.data(), static_cast<int>(Str.length()));

	return std::wstring(Path) + L"@" + Str;
}

template<class TShader>
std::variant<ShaderDetail<TShader>, HRESULT> CreateExt(ID3D11Device* Dev, const D3DBlob& Code, const typename Shader<TShader>::Parameters& Params)
{
	if constexpr (std::is_same_v<TShader, ID3D11VertexShader>)
	{
		ComPtr<ID3D11InputLayout> InputLayout;

		if (Params.InputDescs &&
			Params.NumInputs > 0)
		{
			HRESULT Hr;

			if (FAILED(Hr = Dev->CreateInputLayout(Params.InputDescs, Params.NumInputs, Code->GetBufferPointer(), Code->GetBufferSize(), &InputLayout)))
			{
				return Hr;
			}

			return ShaderDetail<TShader>{ InputLayout };
		}

		return ShaderDetail<TShader>{ };
	}
	else
	{
		return ShaderDetail<TShader>{ };
	}
}

HRESULT CompileShaderObject(const std::wstring_view Path, const std::string_view Entry, ID3D11PixelShader** Out, D3DBlob& Code)
{
	D3DBlob Error;
	HRESULT Hr;

	if (FAILED(Hr = D3DCompileFromFile(Path.data(), nullptr, nullptr, Entry.data(), "ps_5_0", ShaderCompilationFlags, 0, &Code, &Error)) ||
		FAILED(Hr = DxRenderer::Instance()->Device->CreatePixelShader(Code->GetBufferPointer(), Code->GetBufferSize(), nullptr, Out)))
	{
		if (Error)
		{
			Logger::Error() << "Shader compiler error: " << Error << Logger::EndLine << "With code: " << std::to_string(Hr) << Logger::EndLine;
		}
		return Hr;
	}

	return S_OK;
}

HRESULT CompileShaderObject(const std::wstring_view Path, const std::string_view Entry, ID3D11VertexShader** Out, D3DBlob& Code)
{
	D3DBlob Error;
	HRESULT Hr;

	if (FAILED(Hr = D3DCompileFromFile(Path.data(), nullptr, nullptr, Entry.data(), "vs_5_0", ShaderCompilationFlags, 0, &Code, &Error)) ||
		FAILED(Hr = DxRenderer::Instance()->Device->CreateVertexShader(Code->GetBufferPointer(), Code->GetBufferSize(), nullptr, Out)))
	{
		if (Error)
		{
			Logger::Error() << "Shader compiler error: " << Error << Logger::EndLine << "With code: " << std::to_string(Hr) << Logger::EndLine;
		}
		return Hr;
	}

	return S_OK;
}

ShaderDetail<ID3D11VertexShader>::ShaderDetail(ComPtr<ID3D11InputLayout> InputLayout) noexcept
	: InputLayout{ InputLayout }
{}

template<class TShader>
Shader<TShader>& ShaderManager::CompileImpl(const std::wstring_view Path, const std::string_view Entry, bool ForceRecompile, 
	const typename Shader<TShader>::Parameters& Params)
{
	auto Ident = ShaderIdent(Path, Entry);

	if (!ForceRecompile)
	{
		if (auto It = ShaderMap.find(Ident); It != ShaderMap.end())
		{
			return static_cast<Shader<TShader>&>(*It.value().get());
		}
	}

	const auto AbsPath = fs::path(Globals::BaseDirectory).append(BaseDir).append(Path);

	D3DBlob Code;
	D3DBlob Error;

	TShader* ShaderObj;
	{
		ThrowOnError(CompileShaderObject(AbsPath.c_str(), Entry, &ShaderObj, Code));
	}

	Shader<TShader>* ResultPtr = new Shader<TShader>(ShaderObj, ThrowOnError(
		CreateExt<TShader>(DxRenderer::Instance()->Device, Code, Params)));

	ShaderMap.insert_or_assign(Ident, std::shared_ptr<ShaderBase>(ResultPtr));

	return *ResultPtr;
}

template<class TShader>
HRESULT ShaderManager::TryCompileImpl(const std::wstring_view Path, const std::string_view Entry, Shader<TShader>& Result, bool ForceRecompile, 
	const typename Shader<TShader>::Parameters& Params) noexcept
{
	HRESULT Hr;

	auto Ident = ShaderIdent(Path, Entry);

	if (!ForceRecompile)
	{
		if (auto It = ShaderMap.find(Ident); It != ShaderMap.end())
		{
			Result = static_cast<Shader<TShader>&>(*It.value().get());
			return S_OK;
		}
	}

	const auto AbsPath = fs::path(Globals::BaseDirectory).append(Path);

	TShader* ShaderObj;

	D3DBlob Code;
	D3DBlob Error;

	if (FAILED(Hr = CompileShaderObject(AbsPath.c_str(), Entry, &ShaderObj, Code)))
	{
		return Hr;
	}

	Shader<TShader>* ResultPtr = new Shader<TShader>(ShaderObj, ThrowOnError(
		CreateExt<TShader>(DxRenderer::Instance()->Device, Code, Params)));

	ShaderMap.insert_or_assign(Ident, std::shared_ptr<ShaderBase>(ResultPtr));

	Result = *ResultPtr;

	return S_OK;
}

template<>
Shader<ID3D11PixelShader>& ShaderManager::Compile(const std::wstring_view Path, const std::string_view Entry, bool ForceRecompile, 
	const typename Shader<ID3D11PixelShader>::Parameters& Params)
{
	return CompileImpl<ID3D11PixelShader>(Path, Entry, ForceRecompile, Params);
}

template<>
Shader<ID3D11VertexShader>& ShaderManager::Compile(const std::wstring_view Path, const std::string_view Entry, bool ForceRecompile, 
	const typename Shader<ID3D11VertexShader>::Parameters& Params)
{
	return CompileImpl<ID3D11VertexShader>(Path, Entry, ForceRecompile, Params);
}

template<>
HRESULT ShaderManager::TryCompile(const std::wstring_view Path, const std::string_view Entry, Shader<ID3D11PixelShader>& Result, bool ForceRecompile, 
	const typename Shader<ID3D11PixelShader>::Parameters& Params) noexcept
{
	return TryCompileImpl(Path, Entry, Result, ForceRecompile, Params);
}

template<>
HRESULT ShaderManager::TryCompile(const std::wstring_view Path, const std::string_view Entry, Shader<ID3D11VertexShader>& Result, bool ForceRecompile, 
	const typename Shader<ID3D11VertexShader>::Parameters& Params) noexcept
{
	return TryCompileImpl(Path, Entry, Result, ForceRecompile, Params);
}