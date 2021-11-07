#pragma once

#include <utility>

#include <type_traits>
#include <functional>

constexpr size_t min_expansion_size = 1;

struct emulatable {};

template<size_t Size, class FTy>
class emulate;

template<size_t Size>
class sbase : public emulatable
{
public:
    constexpr sbase() {}

    template<class Ty>
    constexpr auto as() -> Ty&
    {
        return static_cast<Ty&>(*this);
    }

    template<class Ty>
    constexpr auto as() const -> const Ty&
    {
        return static_cast<const Ty&>(*this);
    }

    static constexpr size_t size = Size;
};

template<class Ty> struct is_spec;
template<template<size_t> class Ty, size_t Size>
struct is_spec<Ty<Size>>
{
    static constexpr bool value = std::is_same<Ty<Size>, sbase<Size>>::value;
};
template<class Ty>
struct is_spec
{
    static constexpr bool value = false;
};

using base = sbase<min_expansion_size>;

template<class TLmd, class... TRest>
constexpr auto determine_size() -> size_t
{
    constexpr auto s = sizeof(TLmd);

    if constexpr (sizeof...(TRest) > 0)
    {
        return s + determine_size<TRest...>();
    }
    else
    {
        return s;
    }
}

template<class TLmd , class TBase = sbase<sizeof(TLmd)>>
class expand : public TBase, public TLmd
{
    constexpr static size_t size = std::max(TBase::size, sizeof(TLmd));

public:
    constexpr expand(const TLmd&& v, TBase && = {})
        : TLmd { std::move(v) }
    {}

    using TLmd::operator();
};

template<size_t Size, class FTy>
struct emulate_capture : public emulatable
{
    using data_type = std::array<std::byte, Size>;
    using retn_type = std::invoke_result<FTy>;
    using func_type = FTy;

    constexpr emulate_capture() : data{} {}
    constexpr emulate_capture(data_type&& data) : data{ std::move(data) } {}

    data_type data;
};

template<class Ty>
struct emulate_fn
{
    constexpr emulate_fn() : fn{} {}
    constexpr emulate_fn(Ty v) : fn{ v } {}
    Ty fn;
};

template<class TCy, class RTy, class... Args>
using emulate_fn_discrete = emulate_capture<TCy::size, RTy(TCy::*)(Args...) const>;

template<class CTy, class RTy, class... Args>
struct extract_fn_traits
{
    constexpr extract_fn_traits(RTy(CTy::*)(Args...) const) {}
    using args_tuple_t = std::tuple<Args...>;
};

template<size_t Size, class FTy = sbase<Size>(emulatable::*)() const>
class emulate :
    public sbase<Size>,
    public emulate_fn<FTy>,
    public emulate_capture<Size, FTy>
{
public:
    using capture = emulate_capture<Size, FTy>;
    using functor = emulate_fn<FTy>;

public:
    constexpr emulate() : functor{}, capture{} {}

    template<class TLmd>
    constexpr emulate(const expand<TLmd>& other)
        : emulate(static_cast<sbase<Size>&&>(other))
    {}

    template<class TLmd>
    constexpr emulate(expand<TLmd>&& other)
        : emulate(static_cast<sbase<Size>&&>(other))
    {}

    constexpr emulate(const sbase<Size>& other)
        : functor{ other.template as<emulate>()
                        .template as<functor>() }
        , capture{ other.template as<emulate>()
                        .template as<capture>() }
    {}

    constexpr emulate(sbase<Size>&& other)
        : functor{ other.template as<emulate>()
                        .template as<functor>() }
        , capture{ std::move(other  .template as<emulate>()
                                    .template as<capture>()) }
    {}

    template<class... Args>
    constexpr auto operator()(Args&&... args) const
    {
        if constexpr (std::is_same<std::tuple<Args...>, typename decltype(extract_fn_traits(functor::fn))::args_tuple_t>::value)
        {
            return (static_cast<const capture&>(*this).*functor::fn)(std::forward<Args>(args)...);
        }
        else
        {
            return static_cast<const emulate<Size, emulate(emulatable::*)(Args...) const>&>(*this)(std::forward<Args>(args)...);
        }
    }

    template<class Ty>
    constexpr auto as() -> Ty&
    {
        return static_cast<Ty&>(*this);
    }

    template<class Ty>
    constexpr auto as() const -> const Ty&
    {
        return static_cast<const Ty&>(*this);
    }
};