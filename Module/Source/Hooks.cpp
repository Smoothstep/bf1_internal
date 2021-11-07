#include <thread>

#include "stdafx.h"
#include "Hooks.h"

#pragma pack(push, 1)

template<class...> struct PackedTupleDetail;

template<>
struct PackedTupleDetail<>
{
};

template<class T>
struct PackedTupleDetail<T>
{
	constexpr PackedTupleDetail() noexcept = default;
	constexpr PackedTupleDetail(T Arg) noexcept
		: Value(Arg) {}

	T Value;
};

template<class T, class... TRest>
struct PackedTupleDetail<T, TRest...> : public PackedTupleDetail<T>
{
	using TTuple = std::tuple<T, TRest...>;

	constexpr PackedTupleDetail() = default;
	constexpr PackedTupleDetail(T Arg, TRest... Rest) noexcept
		: PackedTupleDetail<T>(Arg)
		, Rest(Rest...)
	{}

	PackedTupleDetail<TRest...> Rest;
};

template<class... TRest>
struct PackedTuple : public PackedTupleDetail<TRest...> 
{
	constexpr PackedTuple() = default;
	constexpr PackedTuple(TRest... Rest) noexcept
		: PackedTupleDetail<TRest...>(Rest...)
	{}
};

#pragma pack(pop)

constexpr uint8_t operator""_b(uint64_t Arg) noexcept
{
	return static_cast<uint8_t>(Arg);
}

uint8_t* Tib(size_t Offset = 0)
{
	return reinterpret_cast<uint8_t*>(__readgsqword(0x30)) + Offset;
}

constexpr uint32_t HkRefOffset = 0x1840;

/*[]()
{
	auto Time = std::string_view{ __TIME__ };
	uint32_t Result = 0;
	for (auto N = 0; N < Time.length(); ++N)
	{
		Result ^= Time[N] << N;
	}
	return (Result % 0x1000) + 0x1000;
}();
*/

HookRef HookRef::Current()
{
	return Current<HookRef>();
}

HookDb::~HookDb()
{
	UnhookAll();
}

void HookDb::UnhookAll(bool WaitForAll)
{
	std::vector<std::weak_ptr<Hook>> WeakHooks;

	for (auto Hk : Hooks)
	{
		WeakHooks.push_back(Hk);
		Hk->Unhook();
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	{
		Hooks.clear();
	}

	if (WaitForAll)
	{
		for (auto& Hk : WeakHooks)
		{
			while (!Hk.expired())
			{
				std::this_thread::yield();
			}
		}
	}
}

void HookDb::Add(std::shared_ptr<Hook> Hk)
{
	Hooks.push_back(Hk);
}

void HookDb::Remove(Hook* Hk)
{
	if (auto Iter = std::find_if(Hooks.begin(), Hooks.end(), [Hk](const auto& P) 
		{ 
			return Hk == P.get(); 
		}); 
	Iter != Hooks.end())
	{
		Hooks.erase(Iter);
	}
}

Hook::Hook(void * Src, void * Dst) noexcept
	: Src(Src)
	, Dst(Dst)
	, Hooked(false)
	, OriginalBytes{}
{}

Hook::~Hook()
{
	Unhook();
}

bool Hook::Unhook()
{
	if (Hooked)
	{
		DWORD OldProtection;

		if (!VirtualProtect(Src, OriginalBytes.size(), PAGE_EXECUTE_READWRITE, &OldProtection))
		{
			return false;
		}

		memcpy(Src, OriginalBytes.data(), OriginalBytes.size());

		if (!VirtualProtect(Src, OriginalBytes.size(), OldProtection, &OldProtection))
		{
			return false;
		}

		Hooked = false;
	}

	return true;
}

const void* DetourHook::GetOriginalPtr() const
{
	return OriginalContinuation.Address();
}

EHookVariant DetourHook::Type() const
{
	return EHookVariant::Hook_Detour;
}

IntermediateHook::IntermediateHook(void* Src, void* Dst)
	: DetourHook(Src, Dst)
{}

IntermediateHook::~IntermediateHook()
{
}

EHookVariant IntermediateHook::Type() const
{
	return EHookVariant::Hook_Intermediate;
}

const void* IntermediateHook::GetOriginalPtr() const
{
	return reinterpret_cast<const void*>(reinterpret_cast<size_t>(Src));
}

bool IntermediateHook::InitHook(const MemoryProvider& MemProvider, const InitParameters& Params, TrampolineContext&& Ctx)
{
	if (!Unhook())
	{
		return false;
	}

	OriginalBytes.resize(Params.NumBytes);
	{
		std::copy(
			reinterpret_cast<const uint8_t*>(Src),
			reinterpret_cast<const uint8_t*>(Src) + Params.NumBytes, OriginalBytes.begin());
	}

	ExecutableMemoryLocation Mem{ Src, Params.NumBytes };

	InstructionSequence SrcInstructions(Mem);
	{
		SrcInstructions << PackedTuple{ 0xFF_b, 0x25_b, 0, reinterpret_cast<uintptr_t>(Dst) }; // jmp abs original

		for (size_t N = 0; N < Params.NumBytes - sizeof(AbsoluteJumpInstruction); ++N)
		{
			SrcInstructions << PackedTuple{ 0x90_b };
		}
	}

	return Hooked = true;
}


DetourHook::DetourHook(void * Src, void * Dst) noexcept
	: TrampolineHook(Src, Dst)
{}

bool DetourHook::InitHook(const MemoryProvider& MemProvider, const InitParameters& Params, TrampolineContext&& Ctx)
{
	if (!Unhook())
	{
		return false;
	}

	if (!Params.Valid())
	{
		return false;
	}

	auto SrcLocation = ExecutableMemoryLocation{ Src, Params.NumBytes };

	OriginalBytes.resize(Params.NumBytes);
	{
		std::copy(
			reinterpret_cast<const uint8_t*>(Src),
			reinterpret_cast<const uint8_t*>(Src) + Params.NumBytes, OriginalBytes.begin());
	}

	std::vector<uint8_t> Bytes = OriginalBytes;

	InstructionSequence TrampolineInstructions(OriginalContinuation = {});
	{
		TrampolineInstructions << Bytes;
		TrampolineInstructions << PackedTuple{ 0xFF_b, 0x25_b, 0, reinterpret_cast<uintptr_t>(OffsetPtr(Src, Params.NumBytes)) };
	}
	
	(Trampoline = std::move(Ctx)).PlaceTrampoline(MemProvider, SrcLocation, { Dst });

	return Hooked = true;
}

VMTHook::VMTHook(void * VTable, void * Dst, size_t FIndex) noexcept
	: TrampolineHook(Ptr(VTable, FIndex), Dst)
	, VTable(VTable)
	, FunctionIndex(FIndex)
{}

EHookVariant VMTHook::Type() const
{
	return EHookVariant::Hook_VMT;
}

const void* VMTHook::GetOriginalPtr() const
{
	assert(OriginalBytes.size() == sizeof(void*));
	return *reinterpret_cast<const void* const*>(OriginalBytes.data());
}

bool VMTHook::InitHook(const MemoryProvider& MemProvider, const InitParameters& Params, TrampolineContext&& Context)
{
	if (!Unhook())
	{
		return false;
	}

	if (!Params.Valid())
	{
		return false;
	}
	
	OriginalBytes.resize(sizeof(void*));
	{
		std::copy(
			reinterpret_cast<const uint8_t*>(Src),
			reinterpret_cast<const uint8_t*>(Src) + sizeof(void*), OriginalBytes.begin());
	}

	const ExecutableMemory& TrampolineMemory = (Trampoline = std::move(Context)).PlaceTrampoline(MemProvider, { Dst });

	if (TrampolineMemory.Length() < sizeof(AbsoluteJumpInstruction))
	{
		return Hooked = false;
	}

	ExecutableMemoryLocation SrcLocation{ Src, sizeof(void*) };

	if (!SrcLocation.Insert(TrampolineMemory.Address()))
	{
		return Hooked = false;
	}

	return Hooked = true;
}

const ExecutableMemory& TrampolineContext::PlaceTrampoline(const MemoryProvider& Provider, const ExecutableMemoryLocation& Src)
{
	InstructionSequence TrampolineInstructions(Memory = Provider.InitializeMemory());
	{
		TrampolineInstructions 
			<< PackedTuple{ 0x48_b, 0xB8_b, &ContextFn } // Mov rax, ctx
			<< PackedTuple{ 0x65_b, 0x48_b, 0x87_b, 0x04_b, 0x25_b, HkRefOffset } // xchg gs:[x], rax
			<< PackedTuple{ 0xFF_b, 0x25_b, 0, reinterpret_cast<uintptr_t>(OffsetPtr(Src.Address(), Src.Reserved())) }; // jmp abs original
	}

	return Provider.FinalizeMemory(Memory);
}

const ExecutableMemory& TrampolineContext::PlaceTrampoline(const MemoryProvider& Provider, ExecutableMemoryLocation& Src, const ExecutableMemoryLocation& Dst)
{
	PlaceTrampoline(Provider, Dst);

	if (Src.Reserved() >= sizeof(AbsoluteJumpInstruction))
	{
		InstructionSequence SrcInstructions(Src);
		{
			SrcInstructions << AbsoluteJumpInstruction(reinterpret_cast<uintptr_t>(Memory.Address()));

			for (size_t N = 0; N < Src.Reserved() - sizeof(AbsoluteJumpInstruction); ++N)
			{
				SrcInstructions << PackedTuple{ 0x90_b };
			}
		}
	}

	return Memory;
}

HookRef::HookRef(const std::shared_ptr<Hook>& Hk, PackedContext& Ctx) noexcept
	: std::shared_ptr<Hook>(Hk)
	, HkContext(Ctx)
	, RootContext(Ctx)
{
	HkContext.PreviousContext = Ctx.Ref ? &Ctx.Ref->HkContext : nullptr;
	RootContext.Ref = this;
}

HookRef::~HookRef()
{
	if (InterlockedDecrement(&RootContext.RefCount) == 0 && HkContext.PreviousContext)
	{
		RootContext = std::move(*HkContext.PreviousContext);
	}
	else
	{
		RootContext.Ref = nullptr;
		RootContext.PreviousContext = nullptr;
	}
}

PackedContext& HookRef::Context()
{
	return *reinterpret_cast<PackedContext*>(Tib(HkRefOffset));
}

HookRef::HookRef(HookRef&& Other) noexcept
	: std::shared_ptr<Hook>(std::move(Other))
	, HkContext(std::move(Other.HkContext))
	, RootContext(Other.RootContext)
{
	RootContext.Ref = this;
}

ExecutableMemory DefaultMemoryProvider::InitializeMemory() const
{
	return {};
}

ExecutableMemory& DefaultMemoryProvider::FinalizeMemory(ExecutableMemory& Memory) const
{
	return Memory;
}

constexpr NestedMemoryProvider::NestedMemoryProvider(TrampolineContext& Parent) noexcept
	: Parent{ Parent }
{}

ExecutableMemory NestedMemoryProvider::InitializeMemory() const
{
	ExecutableMemory Mem = DefaultMemoryProvider::InitializeMemory();

	InstructionSequence Seq{ Mem };
	{
		Seq << PackedTuple{ 0x48_b, 0xB8_b, &Parent } // Mov rax, ctx
			<< PackedTuple{ 0x65_b, 0x48_b, 0x87_b, 0x04_b, 0x25_b, HkRefOffset + static_cast<uint32_t>(sizeof(void*)) }; // xchg gs:[x], rax
	}

	return Mem;
}

ExecutableMemory& NestedMemoryProvider::FinalizeMemory(ExecutableMemory& Memory) const
{
	return DefaultMemoryProvider::FinalizeMemory(Memory);
}