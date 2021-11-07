#pragma once

#include <tuple>
#include <utility>

template<class, class = void>
struct FunctionTraits;

template<class TRet, class... TArgs>
struct FunctionTraits<TRet(TArgs...), void>
{
	using TReturn = TRet;
	using TArguments = std::tuple<TArgs...>;
	using TFunction = TRet(TArgs...);
};

template<class TRet, class... TArgs>
struct FunctionTraits<TRet(*)(TArgs...), void>
{
	using TReturn = TRet;
	using TArguments = std::tuple<TArgs...>;
	using TFunction = TRet(TArgs...);
};

template<class TClass, class TRet, class... TArgs>
struct FunctionTraits<TRet(TClass::*)(TArgs...), void>
{
	using TReturn = TRet;
	using TArguments = std::tuple<TArgs...>;
	using TFunction = TRet(TClass::*)(TArgs...);
};

template<class TClass, class TRet, class... TArgs>
struct FunctionTraits<TRet(TClass::*)(TArgs...) const, void>
{
	using TReturn = TRet;
	using TArguments = std::tuple<TArgs...>;
	using TFunction = TRet(TClass::*)(TArgs...);
};

template<class T>
struct FunctionTraits<T, void>
	: public FunctionTraits<decltype(&T::operator())>
{
	constexpr auto operator()() -> std::decay_t<decltype(&T::operator())> {}
};

template<class T>
using FunctionArgs = typename FunctionTraits<T>::TArguments;

template<size_t Index, class T>
using FunctionArg = std::tuple_element_t<Index, FunctionArgs<T>>;

template<size_t, class, class...>
struct PickLambda;

template<size_t Index, class TArg, class Lmd, class... Rest>
struct PickLambda<Index, TArg, Lmd, Rest...>
{
	using Result = std::conditional_t<std::is_invocable_v<Lmd, TArg>,
		std::integral_constant<size_t, Index>,
		typename PickLambda<Index + 1, TArg, Rest...>::Result>;
};

template<size_t Index, class TArg, class Lmd>
struct PickLambda<Index, TArg, Lmd>
{
	using Result = std::conditional_t<std::is_invocable_v<Lmd, TArg>,
		std::integral_constant<size_t, Index>,
		std::integral_constant<size_t, ~0ULL>>;
};

template<class Selection, class... Tuple>
struct CallablePickerImpl
{
	constexpr static size_t Index = PickLambda<0, Selection, Tuple...>::Result::value;
	using Type = typename std::tuple_element<Index, std::tuple<Tuple...>>::type;
};

template<class TArg, class... Tuple>
struct CallablePicker : public CallablePickerImpl<TArg, Tuple...>::Type
{
	using Base = CallablePickerImpl<TArg, Tuple...>;
	using Base::Type::operator();

	constexpr CallablePicker(const std::tuple<Tuple...> Tpl)
		: Base::Type{ std::get<Base::Index>(Tpl) } {}
};

template<class TArg, class... TTuple>
constexpr auto MakeCallableOptions(const std::tuple<TTuple...> Tuple)
{
	return CallablePicker<TArg, TTuple...>(Tuple);
}

template<class TArg, class... TTuple>
constexpr auto MakeCallableOptions(TArg&& Arg, const std::tuple<TTuple...> Tuple)
{
	return CallablePicker<TArg, TTuple...>(Tuple);
}