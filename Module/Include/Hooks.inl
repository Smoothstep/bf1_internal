#pragma once

template<class THook, class... TArgs>
void HookDb::Init(TArgs&&... Args)
{
	Unhook<THook>();
	auto Hk = std::make_shared<THook>(HookTag<TArgs...>(std::forward<TArgs>(Args)...));
	Add(std::static_pointer_cast<Hook>(Hk));
}

template<class THook>
void HookDb::Unhook()
{
	if (auto Iter = std::find_if(Hooks.begin(), Hooks.end(), [](const auto& Hk)
	{
		return dynamic_cast<THook*>(Hk.get()) != nullptr;
	}); Iter != Hooks.end())
	{
		const auto Weak = std::weak_ptr{ *Iter };
		Hooks.erase(Iter);
		while (!Weak.expired())
		{
			// Currently in use
			std::this_thread::yield();
		}
	}
}

constexpr bool Hook::IsHooked() const
{
	return Hooked;
}

inline uint8_t* Hook::GetReturnAddress()
{
	return reinterpret_cast<uint8_t*>(Src) + OriginalBytes.size();
}

template<class T>
inline T Hook::GetOriginal() const
{
	union TCast
	{
		const void* Ptr;
		T PtrFn;
	} Cast = { GetOriginalPtr() };

	return Cast.PtrFn;
}

template<class TFn>
constexpr TrampolineContext::TrampolineContext(TFn&& Fn) noexcept
	: ContextFn{ std::move(Fn) }
{}

template<class... TArgs>
TrampolineContext TrampolineHook::MakeCtx(TArgs&&... Args)
{
	return MakeCtx(std::forward_as_tuple(Args...));
}

template<class... TArgs>
TrampolineContext TrampolineHook::MakeCtx(std::tuple<TArgs...>&& Tuple)
{
	return
	{
		[this, Tpl { std::move(Tuple) }] (HookRef& Ref, PackedContext& PreviousCtx)
		{
			new (&Ref) HookRefSpec<TArgs...> { { shared_from_this(), PreviousCtx }, Tpl };
		}
	};
}

template<class TSuper>
TSuper* HookRef::Previous()
{
	if (!HkContext.PreviousContext)
	{
		return nullptr;
	}

	if constexpr (std::is_same_v<TSuper, HookRef>)
	{
		return HkContext.PreviousContext->Ref;
	}
	else
	{
		return dynamic_cast<TSuper*>(HkContext.PreviousContext->Ref);
	}
}

template<class TSuper>
TSuper HookRef::Current()
{
	MaybeInitialized<TSuper> Ref{ true };

	while (true)
	{
		auto& Ctx = Context();

		try
		{
			(*Ctx.ContextFn)(Ref, Ctx);
		}
		catch (const std::bad_weak_ptr& Ex)
		{
			Logger::Error() << Ex.what() << Logger::EndLine;
			continue;
		}

		break;
	}

	return std::move(static_cast<TSuper&>(Ref));
}

template<class TSuper>
TSuper* HookRef::Nested()
{
	auto& Current = Context();

	if (Current.ContextFnParent)
	{
		for (auto* Ctx = Current.PreviousContext; Ctx && Ctx != std::addressof(Current); Ctx = Ctx->PreviousContext)
		{
			if (Ctx->ContextFn == Current.ContextFnParent)
			{
				return reinterpret_cast<TSuper*>(Ctx->Ref);
			}
		}
	}

	return nullptr;
}

template<class... TArgs>
bool DetourHook::InitHook(std::tuple<TArgs...>&& Args, const InitParameters& Params)
{
	return InitHook(DefaultMemoryProvider{}, Params, MakeCtx(std::move(Args)));
}

template<class... TArgs>
bool IntermediateHook::InitHook(std::tuple<TArgs...>&& Args, const IntermediateHook::InitParameters& InitParams)
{
	return InitHook(DefaultMemoryProvider{}, InitParams, MakeCtx(std::move(Args)));
}

template<class THook>
template<class TParent>
NestedHook<THook>::NestedHook(std::shared_ptr<TParent> Parent, THook&& Nested) noexcept
	: THook(std::move(Nested))
	, Parent(std::dynamic_pointer_cast<TrampolineHook>(Parent))
{}

template<class THook>
template<class... TArgs>
bool NestedHook<THook>::InitHook(std::tuple<TArgs...>&& Args, const typename THook::InitParameters& InitParams)
{
	return THook::InitHook(NestedMemoryProvider{ Parent->Trampoline }, InitParams, THook::MakeCtx(std::move(Args)));
}

template<class THook>
const THook& NestedHook<THook>::Internal() const
{
	return static_cast<const THook&>(*this);
}

template<class THook>
std::shared_ptr<Hook> NestedHook<THook>::ParentHook() const
{
	return Parent;
}

template<class T>
constexpr VMTHook::VMTHook(void* VTable, void* Dst, T FIndex) noexcept
	: VMTHook(VTable, Dst, static_cast<size_t>(FIndex)) {}

template<class... TArgs>
bool VMTHook::InitHook(std::tuple<TArgs...>&& Args, const InitParameters& InitParams)
{
	return InitHook(DefaultMemoryProvider{}, InitParams, MakeCtx(std::move(Args)));
}