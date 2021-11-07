#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include <d3d11_4.h>
#include <functional>
#include <d3dcompiler.h>
#include <optional>
#include <memory_resource>
#include <assert.h>

#include "Offsets.h"
#include "Lock.h"
#include "FnTraits.h"
#include "Tlf.h"
#include "Types.h"

struct RenderFunctionEntry
{
	struct RenderFunctions* Func() const
	{
		return reinterpret_cast<RenderFunctions*>(FuncPtr);
	}

	RenderFunctionEntry* Next() const
	{
		return reinterpret_cast<RenderFunctionEntry*>(reinterpret_cast<uintptr_t>(this) + NextOffset);
	}

	template<class... TArgs>
	void* Execute(const size_t Offset, TArgs... Args) const
	{
		if (!FuncPtr)
		{
			return nullptr;
		}

		return (reinterpret_cast<void* (__thiscall*)(TArgs...)>(reinterpret_cast<void**>(FuncPtr)[Offset]))(Args...);
	}

	size_t FuncPtr : 48;
	size_t NextOffset : 16;
};

template<class T>
struct RenderFunctionEntrySpec : public RenderFunctionEntry
{
	RenderFunctionEntrySpec()
	{
		FuncPtr = reinterpret_cast<size_t>(GetVtbl<T>());
		NextOffset = sizeof(T);
	}
};

struct ShaderRenderFunctionEntry : public RenderFunctionEntry
{
	Pad(0x07);
	ID3D11InputLayout* Il;
	ID3D11VertexShader* Vs;
	ID3D11PixelShader* Ps;
};

struct MeshSet
{};

class Dx11RenderTarget
{
	Pad(0xD0);
	ID3D11RenderTargetView* Rtv;
	Pad(0x38);
	ID3D11DepthStencilView* Dsv;

public:
	ID3D11RenderTargetView* RawRtv()
	{
		return Rtv;
	}

	ID3D11DepthStencilView* RawDsv()
	{
		return Dsv;
	}
};

struct Dx11RenderPass
{
	size_t NumRtvs;
	Dx11RenderTarget* Rtvs;
};

struct RenderFunctions
{
	RenderFunctionEntry* Last()
	{
		RenderFunctionEntry* Prev = First;

		for (RenderFunctionEntry* Curr = Prev; reinterpret_cast<size_t>(Curr) - reinterpret_cast<size_t>(First) < Size; Curr = Curr->Next())
		{
			Prev = Curr;
		}

		return Prev;
	}

	RenderFunctionEntry* Find(size_t Fn)
	{
		RenderFunctionEntry* Prev = First;

		for (RenderFunctionEntry* Curr = Prev; reinterpret_cast<size_t>(Curr) - reinterpret_cast<size_t>(First) < Size; Curr = Curr->Next())
		{
			if (Curr->FuncPtr == Fn)
			{
				return Curr;
			}
		}

		return nullptr;
	}

	template<class T, class TContext, class... TArgs>
	TContext* Allocate(TArgs&&... Args)
	{
		auto Result = CallFunctionAddress<TContext*>(CastPtr(EFunctionOffsets::Offset_AllocateRenderFunc), &First, sizeof(TContext));
		{
			new (Result) TContext(std::forward<TArgs>(Args)...);
			Result->FuncPtr = reinterpret_cast<size_t>(GetVtbl<T>());
			Result->NextOffset = sizeof(TContext);
		}

		return Result;
	}

	size_t Size;
	void* VPtr;
	RenderFunctionEntry* First;
};

struct RenderFunctionsCopy : public RenderFunctions
{
	RenderFunctionsCopy(const RenderFunctions& Other)
		: RenderFunctions{ Other }
	{
		First = reinterpret_cast<RenderFunctionEntry*>(new uint8_t[Size]);
		{
			memcpy(First, Other.First, Size);
		}
	}

	RenderFunctionsCopy(RenderFunctionsCopy&& Other) noexcept
		: RenderFunctions{ std::move(Other) }
	{
		Other.First = nullptr;
		Other.Size = 0;
	}

	~RenderFunctionsCopy()
	{
		if (First)
		{
			delete[] First;
		}
	}
};

struct FunctionInterfaceBase
{
	Pad(0x1C);
	uint32_t RenderFlags;
	Pad(0x164);
	uint32_t Depth;
	Pad(0x2388);
	RenderFunctions RFn;
};

template<class T>
class FunctionInterface : public FunctionInterfaceBase
{
public:
	using TContext = T;

public:
	virtual void* Apply1(T*, void*) = 0;
	virtual void* Apply2(T*, void*) = 0;
	virtual void* Apply3(T*, void*) = 0;
};

struct RenderPassFb
{
	uint8_t Unk[16];
	void* VPtr;
};

struct RenderFunctionDesc
{
	RenderFunctions* Pipeline;
	RenderPassFb* RenderPass;
};

struct RenderDetail
{
	size_t Size() const
	{
		RenderFunctionEntry const* Previous = &Entry;
		size_t Result = Previous->NextOffset;

		for (auto* E = Previous->Next(); E != Previous; Previous = E, E = E->Next())
		{
			Result += E->NextOffset;
		}

		return Result;
	}

	RenderDetail* Copy() const
	{
		const size_t Sz = Size();
		uint8_t* Data = new uint8_t[Sz];
		memcpy(Data, this, Sz);
		return reinterpret_cast<RenderDetail*>(Data);
	}

	RenderFunctionEntry Entry;
	uint32_t Chunk[16];
};

struct RenderInfo
{
	Pad(0x08);
	RenderDetail* Detail;
};

struct GeometryRenderDesc
{
	RenderDetail* Detail;
	float* Transforms;
	size_t Hash;
};

struct RenderEntityDesc
{
	char Type[16];
	class Dx11Texture* Texture;
	uint64_t Chunk;
	float Nt0[4];
	uint32_t NumGeometries;
	float Nt1;
	RenderEntityDesc* Base;
	GeometryRenderDesc GDesc[32];
};

enum ERenderPasses
{
	RpGeometry = 5
};

struct EntityRenderInstructions
{
	uint8_t Pad[0x1258];
	RenderEntityDesc* ReDesc[8];
	uint8_t Pad1[0xC0];
	RenderPassFb* Passes[16];

	size_t NumMeshes() const
	{
		return ReDesc[RpGeometry] ? ReDesc[RpGeometry]->NumGeometries : 0;
	}
};

class RenderFunctionDispatcher
{
public:
	static size_t DispatchAll(void* Context, RenderFunctionEntry* Entry, size_t NumFn, size_t Offset);
};

class RenderFunctionHooks
{
public:
	template<class TFn>
	class ProxyRenderFunctionEntry : public RenderFunctionEntry, public TFn
	{
	public:
		ProxyRenderFunctionEntry(TFn&& Fn) noexcept
			: RenderFunctionEntry{ Funcs(), sizeof(ProxyRenderFunctionEntry) }
			, TFn{ std::move(Fn) }
		{}

		 ProxyRenderFunctionEntry(ProxyRenderFunctionEntry&& Other) noexcept
			: RenderFunctionEntry{ Funcs(), sizeof(ProxyRenderFunctionEntry) }
			 , TFn{ std::move(Other) }
		{}

		template<size_t Index>
		static void Apply(FunctionArg<0, TFn> Ctx, ProxyRenderFunctionEntry& This)
		{
			This(Ctx, Index);
		}

	private:
		static inline size_t Funcs()
		{
			static void* Ptrs[] =
			{
				CastPtr<void>(&ProxyRenderFunctionEntry::Apply<0>),
				CastPtr<void>(&ProxyRenderFunctionEntry::Apply<1>),
				CastPtr<void>(&ProxyRenderFunctionEntry::Apply<2>)
			};

			return reinterpret_cast<size_t>(Ptrs);
		};
	};

	template<class TFn>
	class ProxyRenderFunctions : public RenderFunctions
	{
	public:
		ProxyRenderFunctions() noexcept = default;
		ProxyRenderFunctions(const ProxyRenderFunctions&) = delete;
		ProxyRenderFunctions(const RenderFunctions* Rf, TFn&& Fn) noexcept
			: RenderFunctions{ *Rf }
			, Target{ sizeof(ProxyRenderFunctionEntry<TFn>), nullptr, &this->Fn }
			, Fn{ std::move(Fn) }
		{}

		ProxyRenderFunctions(ProxyRenderFunctions&& Other) noexcept
			: RenderFunctions{ std::move(Other) }
			, Target{ sizeof(ProxyRenderFunctionEntry<TFn>), nullptr, &this->Fn }
			, Fn{ std::move(Other.Fn) }
		{}

	private:
		RenderFunctions Target;
		ProxyRenderFunctionEntry<TFn> Fn;
	};

	struct State
	{
				void* Address;
		const	void* AddressOriginal;

		std::vector<uint8_t> Original;

		State() noexcept = default;
		State(void* Addr, const void* AddressOriginal, std::vector<uint8_t>&& Original) noexcept
			: Address{ Addr }
			, AddressOriginal{ AddressOriginal }
			, Original{ std::move(Original) }
		{}

		State(State&& Other) noexcept
			: Address{ Other.Address }
			, AddressOriginal{ Other.AddressOriginal }
			, Original{ std::move(Other.Original) }
		{
			Other.Address = nullptr;
		}

		~State()
		{
			if (Address)
			{
				memcpy(Address, Original.data(), Original.size());
			}
		}
	};

public:
	RenderFunctionHooks() = default;
	RenderFunctionHooks(const RenderFunctionHooks&) = delete;

	void HookRenderFunction(RenderFunctionEntry* Entry, void* VTbl, size_t NumFunctions = 3)
	{
		const size_t VTblSize = sizeof(void*) * NumFunctions;

		// Leak few bytes on purpose to prevent crashes after unloading library
		void** VTblCopy = new void* [NumFunctions];
		memcpy(VTblCopy, VTbl, VTblSize);

		std::vector<uint8_t> Bytes;
		{
			Bytes.insert(Bytes.end(),
				reinterpret_cast<uint8_t*>(Entry->FuncPtr),
				reinterpret_cast<uint8_t*>(Entry->FuncPtr) + VTblSize);
		}

		Entry->FuncPtr = reinterpret_cast<size_t>(VTblCopy);

		States.emplace_back(VTblCopy, VTbl, std::move(Bytes));
	}

	template<class T>
	ProxyRenderFunctions<T>* HookRenderFunctions(const RenderFunctions* Rf, T&& Hook)
	{
		return new (ProxyResource.allocate(sizeof(ProxyRenderFunctions<T>)))
			ProxyRenderFunctions<T>(Rf, std::move(Hook));
	}

	void ClearRenderFunction(RenderFunctions* Rf)
	{
		ProxyResource.deallocate(Rf, sizeof(RenderFunctions[2]) + Rf[1].Size);
	}

	bool IsProxy(RenderFunctions* Rf) const
	{
		return Rf && !Rf[1].VPtr;
	}

private:
	std::deque<State> States;
	std::pmr::synchronized_pool_resource ProxyResource;
};