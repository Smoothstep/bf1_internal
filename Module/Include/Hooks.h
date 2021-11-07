#pragma once

#include <Windows.h>
#include <vector>
#include <assert.h>
#include <atomic>
#include <functional>
#include <csetjmp>
#include <deque>

#include "Instruction.h"
#include "Offsets.h"
#include "FnValue.h"

enum class EHookVariant
{
	Hook_VMT,
	Hook_Intermediate,
	Hook_Detour
};

class Hook;
class HookRef;

template<class... TArgs>
class HookRefSpec;

template<class T>
class NestedHook;

class VMTHook;
class IntermediateHook;
class DetourHook;
class HookDb
{
	friend class Hook;

public:
	 HookDb() = default;
	~HookDb();

	template<class THook, class... TArgs>
	void Init(TArgs&&... Args);
	template<class THook>
	void Unhook();
	void UnhookAll(bool WaitForAll = true);

protected:
	void Add(std::shared_ptr<Hook> Hk);
	void Remove(Hook * Hk);

private:
	std::deque<std::shared_ptr<Hook>> Hooks;
};

template<class T, class... TParams>
struct PTag
{
	friend T;

protected:
	constexpr PTag(TParams&&... Args) noexcept
		: Params(std::forward_as_tuple(Args...))
	{}

public:
	constexpr std::tuple<TParams...> Unwrap()
	{
		return std::move(Params);
	}

private:
	std::tuple<TParams...> Params;
};

template<class... TParams>
using HookTag = PTag<HookDb, TParams...>;

using TCtxFn = std::function<void(HookRef&, class PackedContext&)>;

class PackedContext
{
	friend class HookRef;

protected:
	constexpr PackedContext(HookRef* Ref) noexcept
		: ContextFn(nullptr)
		, ContextFnParent(nullptr)
		, PreviousContext(nullptr)
		, Ref(Ref)
		, RefCount(1)
	{}

	constexpr PackedContext(const PackedContext& Other) noexcept
		: ContextFn(Other.ContextFn)
		, ContextFnParent(Other.ContextFnParent)
		, PreviousContext(Other.PreviousContext)
		, Ref(Other.Ref)
		, RefCount(Other.RefCount)
	{}

	constexpr PackedContext(PackedContext&& Other) noexcept
		: ContextFn(Other.ContextFn)
		, ContextFnParent(Other.ContextFnParent)
		, PreviousContext(Other.PreviousContext)
		, Ref(Other.Ref)
		, RefCount(Other.RefCount)
	{
		Other.PreviousContext = nullptr;
		Other.Ref = nullptr;
	}

	constexpr PackedContext& operator=(PackedContext&& Other) noexcept
	{
		ContextFn = Other.ContextFn;
		ContextFnParent = Other.ContextFnParent;
		PreviousContext = Other.PreviousContext;
		Ref = Other.Ref;
		RefCount = Other.RefCount;
		return *this;
	}

	TCtxFn* ContextFn;
	TCtxFn* ContextFnParent;
	PackedContext* PreviousContext;
	HookRef* Ref;
	size_t RefCount;
};

class HookRef : public std::shared_ptr<Hook>
{
public:
	using std::shared_ptr<Hook>::operator->;
	using std::shared_ptr<Hook>::operator*;
	using std::shared_ptr<Hook>::operator bool;

	HookRef(const std::shared_ptr<Hook>& Hk, PackedContext& Ctx) noexcept;
	~HookRef();

	static HookRef Current();

	template<class TSuper>
	static TSuper* Previous();

protected:
	HookRef(HookRef&& Other) noexcept;

	static PackedContext& Context();

	template<class TSuper>
	static TSuper Current();

	template<class TSuper>
	static TSuper* Nested();

private:
	PackedContext HkContext;
	PackedContext& RootContext;
};

template<class... TArgs>
class HookRefSpec : public HookRef, public std::tuple<TArgs...>
{
public:
	using std::shared_ptr<Hook>::operator->;
	using std::shared_ptr<Hook>::operator*;
	using std::shared_ptr<Hook>::operator bool;

public:
	constexpr auto Tuple() noexcept -> std::tuple<TArgs...>&
	{
		return *this;
	}

	static auto Current() noexcept -> HookRefSpec<TArgs...>
	{
		return HookRef::Current<HookRefSpec<TArgs...>>();
	}

	static auto Nested() noexcept -> HookRefSpec<TArgs...>*
	{
		return HookRef::Nested<HookRefSpec<TArgs...>>();
	}
};

class MemoryProvider
{
public:
	virtual ExecutableMemory InitializeMemory() const = 0;
	virtual ExecutableMemory& FinalizeMemory(ExecutableMemory& Memory) const = 0;
};

class TrampolineContext
{
public:
	TrampolineContext(const TrampolineContext&) = delete;
	TrampolineContext() noexcept = default;

	template<class TFn>
	constexpr TrampolineContext(TFn&& Fn) noexcept;

	const ExecutableMemory& PlaceTrampoline(const MemoryProvider& Provider,
		ExecutableMemoryLocation& Src, const ExecutableMemoryLocation& Dst);

	const ExecutableMemory& PlaceTrampoline(const MemoryProvider& Provider,
		const ExecutableMemoryLocation& Src);

private:
	TCtxFn ContextFn;
	ExecutableMemory Memory;
};

class DefaultMemoryProvider : public MemoryProvider
{
public:
	constexpr DefaultMemoryProvider() noexcept = default;

	virtual ExecutableMemory InitializeMemory() const override;
	virtual ExecutableMemory& FinalizeMemory(ExecutableMemory& Memory) const override;
};

class NestedMemoryProvider : public DefaultMemoryProvider
{
public:
	constexpr NestedMemoryProvider(TrampolineContext& Parent) noexcept;

	virtual ExecutableMemory InitializeMemory() const override;
	virtual ExecutableMemory& FinalizeMemory(ExecutableMemory& Memory) const override;

private:
	TrampolineContext& Parent;
};

class Hook : public std::enable_shared_from_this<Hook>
{
	friend class HookDb;

public:
	virtual EHookVariant Type() const = 0;
	virtual const void* GetOriginalPtr() const = 0;

public:
	constexpr bool IsHooked() const;
	inline uint8_t* GetReturnAddress();

	template<class T>
	inline T GetOriginal() const;

protected:
	Hook(const Hook&) = delete;
	Hook(Hook&&) = delete;
	Hook(void* Src, void* Dst) noexcept;

	virtual ~Hook();
	virtual bool Unhook();

protected:
	void * Src;
	void * Dst;
	bool Hooked;
	std::vector<uint8_t> OriginalBytes;
};

class TrampolineHook : public Hook
{
	template<class T>
	friend class NestedHook;

protected:
	using Hook::Hook;

	template<class... TArgs>
	TrampolineContext MakeCtx(TArgs&&... Args);

	template<class... TArgs>
	TrampolineContext MakeCtx(std::tuple<TArgs...>&& Tuple);

protected:
	TrampolineContext Trampoline;
};

class DetourHook : public TrampolineHook
{
public:
	struct InitParameters
	{
		constexpr bool Valid() const
		{
			return NumBytes >= sizeof(AbsoluteJumpInstruction);
		}

		size_t NumBytes;
	};

public:
	DetourHook(const DetourHook&) = delete;
	DetourHook(DetourHook&&) = delete;
	DetourHook(void* Src, void* Dst) noexcept;

	virtual ~DetourHook() {}

	virtual const void* GetOriginalPtr() const override;
	virtual EHookVariant Type() const override;

	template<class... TArgs>
	bool InitHook(std::tuple<TArgs...>&& Args, const InitParameters& Params = {});

protected:
	bool InitHook(const MemoryProvider& MemProvider, const InitParameters& Params, TrampolineContext&& Ctx);

protected:
	ExecutableMemory OriginalContinuation;
};

class IntermediateHook : public DetourHook
{
public:
	struct InitParameters
	{
		constexpr bool Valid() const
		{
			return NumBytes >= sizeof(AbsoluteJumpInstruction);
		}

		size_t NumBytes;
	};

public:
	IntermediateHook(const IntermediateHook&) = delete;
	IntermediateHook(IntermediateHook&&) = delete;
	IntermediateHook(void* Src, void* Dst);

	virtual ~IntermediateHook();

	virtual EHookVariant Type() const final;
	virtual const void* GetOriginalPtr() const final;

	template<class... TArgs>
	bool InitHook(std::tuple<TArgs...>&& Args = {}, const InitParameters& InitParams = {});

protected:
	bool InitHook(const MemoryProvider& MemProvider, const InitParameters& InitParams, TrampolineContext&& Ctx);

private:
	std::jmp_buf JumpBuffer;
};

class VMTHook : public TrampolineHook
{
public:
	struct InitParameters
	{
		constexpr bool Valid() const
		{
			return true;
		}
	};

public:
	VMTHook(const VMTHook&) = delete;
	VMTHook(VMTHook&&) = delete;
	VMTHook(void * VTable, void * Dst, size_t FIndex) noexcept;
	
	template<class T>
	constexpr VMTHook(void* VTable, void* Dst, T FIndex) noexcept;

	virtual EHookVariant Type() const final;
	virtual const void* GetOriginalPtr() const;

	template<class... TArgs>
	bool InitHook(std::tuple<TArgs...>&& Args = {}, const InitParameters& InitParams = {});

protected:
	bool InitHook(const MemoryProvider& MemProvider, const InitParameters& InitParams, TrampolineContext&& Ctx);

private:
	void* VTable;
	size_t FunctionIndex;
};

template<class THook>
class NestedHook : public THook
{
public:
	NestedHook(const NestedHook&) = delete;
	NestedHook(NestedHook&&) = delete;

	template<class TParent>
	NestedHook(std::shared_ptr<TParent> Parent, THook&& Nested) noexcept;

	template<class... TArgs>
	bool InitHook(std::tuple<TArgs...>&& Args = {}, const typename THook::InitParameters& InitParams = {});
	
	const THook& Internal() const;
	
	std::shared_ptr<Hook> ParentHook() const;

private:
	std::shared_ptr<TrampolineHook> Parent;
};

#include "Hooks.inl"