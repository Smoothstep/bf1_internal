#pragma once

#include <utility>
#include <functional>

template<class T, class Fn = std::function<T()>, class Enabled = void>
class FnValue;

template<class T, class Fn>
class FnValue<T, Fn, std::enable_if_t<std::is_pointer_v<T>>>
{
public:
	constexpr FnValue(Fn&& Func)
		: Func(std::move(Func))
	{}

	constexpr T operator ->()
	{
		return Func();
	}

	operator T ()
	{
		return Func();
	}

private:
	Fn Func;
};

template<class Fn>
FnValue(Fn&&)->FnValue<std::invoke_result_t<Fn>, Fn>;

template<class T>
class MaybeInitialized
{
public:
	constexpr MaybeInitialized(bool IsInitialized = false)
		: Storage{}
		, Initialized{ IsInitialized }
	{}

	MaybeInitialized(T&& Init)
		: Initialized{ true }
	{
		new (&Storage) T(std::move(Init));
	}

	template<class... TArgs>
	MaybeInitialized(TArgs&&... Args)
		: Initialized{ true }
	{
		new (&Storage) T(std::forward<TArgs>(Args)...);
	}

	~MaybeInitialized()
	{
		if (Initialized)
		{
			static_cast<T&>(*this).~T();
		}
	}

	constexpr operator T& ()
	{
		return *reinterpret_cast<T*>(&Storage);
	}

private:
	std::aligned_storage_t<sizeof(T)> Storage;
	bool Initialized;
};
