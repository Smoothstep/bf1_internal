#pragma once

#include <wrl/client.h>
#include <dxgi.h>
#include <d3d11_4.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <memory>

#include <tsl/bhopscotch_map.h>

#include "Config.h"
#include "D3DBlob.h"
#include "ClassDecl.h"

using namespace Microsoft::WRL;

#ifdef _DEBUG_SHADERS
constexpr UINT ShaderCompilationFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
constexpr UINT ShaderCompilationFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

struct ColorVertex
{
	Vector4f Position;
	Vector2f Uv;
	Vector2f Wv; /* Pad */
	Vector4f Color;
	Vector4f Origin;

	static const D3D11_INPUT_ELEMENT_DESC InputElementDescs[5];
};

struct SimpleVertex
{
	Vector4f Position;
	Vector2f Uv;

	static const D3D11_INPUT_ELEMENT_DESC InputElementDescs[2];
};

struct NormalVertex
{
	Vector4f Position;
	Vector4f Normal;
	Vector2f Uv;

	static const D3D11_INPUT_ELEMENT_DESC InputElementDescs[3];
};

struct TBNVertex
{
	Vector3f Position;
	Vector3f Tangent;
	Vector3f Binormal;
	Vector3f Normal;
	Vector2f Uv;

	static const D3D11_INPUT_ELEMENT_DESC InputElementDescs[5];
};

struct GlobalShaderConstants
{
	GlobalShaderConstants() = default;
	GlobalShaderConstants(const Default&);

	Matrix4x4 View;
	Matrix4x4 ViewTransposed;
	Matrix4x4 ViewInverse;
	Matrix4x4 WorldViewProjection;
	Matrix4x4 WorldViewProjectionInverse;
	Vector4f ScreenSize;
	Vector4f CameraOrigin;
	Vector4f CameraDirection;
	Vector4f ColorAlly;
	Vector4f ColorAllyOccluded;
	Vector4f ColorEnemy;
	Vector4f ColorEnemyOccluded;
	Vector4f ColorNeutral;
	Vector4f ColorNeutralOccluded;
	float FadeDistance;
	float FadeDistanceNameTags;
	float FadeDistanceIndicators;
	float FadeDistanceEngine;
};

class ShaderBase
{
public:
	ShaderBase(ShaderBase&& Other) noexcept = default;
	ShaderBase(const ShaderBase& Other) noexcept = default;
	ShaderBase(IUnknown* Ptr = nullptr)
		: Object{ Ptr }
	{}

	ShaderBase& operator=(const ShaderBase& Other) noexcept
	{
		Object = Other.Object;
		return *this;
	}

protected:
	ComPtr<IUnknown> Object;
};

template<class TShader>
class ShaderDetail
{
public:
	struct Parameters 
	{};
};

template<>
class ShaderDetail<ID3D11VertexShader>
{
public:
	struct Parameters
	{
		const D3D11_INPUT_ELEMENT_DESC* InputDescs;
		const UINT NumInputs;
	};

public:
	ShaderDetail(ShaderDetail&& Other) noexcept = default;
	ShaderDetail(const ShaderDetail& Other) noexcept = default;
	ShaderDetail(ComPtr<ID3D11InputLayout> InputLayout = nullptr) noexcept;

	ID3D11InputLayout* GetInputLayout() const
	{
		return InputLayout.Get();
	}

private:
	ComPtr<ID3D11InputLayout> InputLayout;
};

template<class TShader>
class Shader : 
	public ShaderBase, 
	public ShaderDetail<TShader>
{
public:
	Shader(Shader&& Other) noexcept = default;
	Shader(const Shader& Other) noexcept = default;
	Shader(TShader* Ptr = nullptr, ShaderDetail<TShader>&& Ext = {}) noexcept;

	Shader& operator=(const Shader& Other) noexcept
	{
		new (this) Shader(Other);
		return *this;
	}

	TShader* Get() const
	{
		return static_cast<TShader*>(Object.Get());
	}

	operator TShader* () const
	{
		return Get();
	}
};

class ShaderManager
{
public:
	static ShaderManager& Instance();

public:
	template<class TShader>
	Shader<TShader>& Compile(const std::wstring_view Path, const std::string_view Entry, bool ForceRecompile = false, 
		const typename Shader<TShader>::Parameters& Params = {});

	template<class TShader>
	HRESULT TryCompile(const std::wstring_view Path, const std::string_view Entry, Shader<TShader>& Result, bool ForceRecompile = false, 
		const typename Shader<TShader>::Parameters& Params = {}) noexcept;

private:
	ShaderManager(const std::wstring_view BaseDir);

	template<class TShader>
	Shader<TShader>& CompileImpl(const std::wstring_view Path, const std::string_view Entry, bool ForceRecompile,
		const typename Shader<TShader>::Parameters& Params = {});

	template<class TShader>
	HRESULT TryCompileImpl(const std::wstring_view Path, const std::string_view Entry, Shader<TShader>& Result, bool ForceRecompile,
		const typename Shader<TShader>::Parameters& Params = {}) noexcept;

	std::wstring ShaderIdent(const std::wstring_view Path, const std::string_view Entry);
	
	std::wstring BaseDir;
	tsl::bhopscotch_map<std::wstring, std::shared_ptr<ShaderBase>, TStringHash, TStringEqual, TStringLess> ShaderMap;
};

template<class TShader>
Shader<TShader>::Shader(TShader* Ptr, ShaderDetail<TShader>&& Ext) noexcept
	: ShaderBase{ Ptr }
	, ShaderDetail<TShader>{ std::move(Ext) }
{}

template<>
Shader<ID3D11PixelShader> & ShaderManager::Compile(const std::wstring_view Path, const std::string_view Entry, bool ForceRecompile, 
	const typename Shader<ID3D11PixelShader>::Parameters& Params);

template<>
Shader<ID3D11VertexShader>& ShaderManager::Compile(const std::wstring_view Path, const std::string_view Entry, bool ForceRecompile, 
	const typename Shader<ID3D11VertexShader>::Parameters& Params);

template<>
HRESULT ShaderManager::TryCompile(const std::wstring_view Path, const std::string_view Entry, Shader<ID3D11PixelShader>& Result, bool ForceRecompile, 
	const typename Shader<ID3D11PixelShader>::Parameters& Params) noexcept;

template<>
HRESULT ShaderManager::TryCompile(const std::wstring_view Path, const std::string_view Entry, Shader<ID3D11VertexShader>& Result, bool ForceRecompile, 
	const typename Shader<ID3D11VertexShader>::Parameters& Params) noexcept;
