#pragma once

#include <tsl/bhopscotch_map.h>
#include <any>
#include <variant>
#include <memory>
#include <unordered_map>

#include "Aimbot.h"
#include "GamePatcher.h"
#include "ClientPlayer.h"
#include "Config.h"
#include "AutoSpot.h"
#include "D3DHooks.h"

//#include "Ultralight.h"
//#include "Framework/JSHelpers.h"

template<class TBase>
using TConfigVariant = std::variant<ToggleState, bool, int, unsigned int, float, double, std::string, std::vector<TBase>, std::unordered_map<std::string, TBase>>;

template <template<class> class TUnderlying>
struct FixedConfigValue : public TUnderlying<FixedConfigValue<TUnderlying>>
{
	using TBody = TUnderlying<FixedConfigValue<TUnderlying>>;
	using TUnderlying<FixedConfigValue<TUnderlying>>::TUnderlying;
};

using TConfigValue = FixedConfigValue<TConfigVariant>;

template<class TValue, class TVariant>
struct AnyConvertible;

template<class TValue, class TType, class... TRest>
struct AnyConvertible<TValue, std::variant<TType, TRest...>>
{
	static constexpr bool Value = std::is_convertible<TType, TValue>::value || AnyConvertible<TValue, std::variant<TRest...>>::Value;
};

template<class TValue, class TType>
struct AnyConvertible<TValue, std::variant<TType>>
{
	static constexpr bool Value = std::is_convertible<TType, TValue>::value;
};

template<class TValue, class = void>
struct TypeAdapter;

template<class TTo>
struct AdapterVisitor
{
	template<class TVariant>
	constexpr TTo operator()(TVariant&& Var)
	{
		return std::visit([](auto&& Value) -> TTo {
			return TypeAdapter<TTo>(std::forward<decltype(Value)>(Value));
		}, std::forward<TVariant>(Var));
	}
};

template<class T>
struct TypeAdapter<std::vector<T>, std::void_t<T&>>
	: public std::vector<T>
{
	using TSource = std::vector<TConfigValue>;

	template<class TOther>
	TypeAdapter(TOther&&) noexcept {};
	TypeAdapter(TSource&& Vec) noexcept
		: std::vector<T>{ [&Vec]() {
			std::vector<T> Result(Vec.size());

			if constexpr (!std::is_same_v<typename TypeAdapter<T>::TSource, T>)
			{
				std::transform(
					std::make_move_iterator(Vec.begin()), 
					std::make_move_iterator(Vec.end()), Result.begin(), [](auto&& Val) -> T {
					return TypeAdapter<T>(std::get<std::vector<TConfigValue>>(Val));
				});
			}
			else
			{
				std::transform(
					std::make_move_iterator(Vec.begin()),
					std::make_move_iterator(Vec.end()), Result.begin(), AdapterVisitor<T>{});
			}

			return Result;
		}()
		}
	{}

	TypeAdapter(const TSource& Vec) noexcept
		: std::vector<T>{ [&Vec]() {
			std::vector<T> Result(Vec.size());

			if constexpr (!std::is_same_v<typename TypeAdapter<T>::TSource, T>)
			{
				std::transform(Vec.begin(), Vec.end(), Result.begin(), [](auto&& Val) -> T {
					return TypeAdapter<T>(std::get<std::vector<TConfigValue>>(Val));
				});
			}
			else
			{
				std::transform(Vec.begin(), Vec.end(), Result.begin(), AdapterVisitor<T>{});
			}

			return Result;
		}()
		}
	{}
};

template<class T, size_t N>
struct TypeAdapter<std::array<T, N>, std::void_t<T&>>
	: public std::array<T, N>
{
	using TSource = std::vector<TConfigValue>;

	template<class TOther>
	TypeAdapter(TOther&&) noexcept {};
	TypeAdapter(TSource&& Src) noexcept
		: std::array<T, N>{ [&Src]() {
			std::array<T, N> Result;

			if constexpr (!std::is_same_v<typename TypeAdapter<T>::TSource, T>)
			{
				std::transform(
					std::make_move_iterator(Src.begin()),
					std::make_move_iterator(Src.begin() + std::min(N, Src.size())), Result.begin(), [](auto&& Val) -> T {
					return TypeAdapter<T>(std::get<std::vector<TConfigValue>>(Val));
				});
			}
			else
			{
				std::transform(
					std::make_move_iterator(Src.begin()),
					std::make_move_iterator(Src.end()), Result.begin(), AdapterVisitor<T>{});
			}

			return Result;
		}()
	} {}

	TypeAdapter(const TSource& Src) noexcept
		: std::array<T, N>{ [&Src]() {
			std::array<T, N> Result;

			if constexpr (!std::is_same_v<typename TypeAdapter<T>::TSource, T>)
			{
				std::transform(Src.begin(), Src.begin() + std::min(N, Src.size()), Result.begin(), [](auto&& Val) -> T {
					return TypeAdapter<T>(std::get<std::vector<TConfigValue>>(Val));
				});
			}
			else
			{
				std::transform(Src.begin(), Src.end(), Result.begin(), AdapterVisitor<T>{});
			}

			return Result;
		}()
	} {}
};

template<class T>
struct TypeAdapter<T, std::void_t<T&>>
{
	using TSource = T;

	constexpr TypeAdapter(T&& From)
		: Converted{ std::move(From) }
	{}

	template<class TArg, std::enable_if_t<std::is_convertible_v<TArg, T>, int> = 0>
	constexpr TypeAdapter(const TArg& From)
		: Converted{ static_cast<T>(From) }
	{}

	template<class TArg, std::enable_if_t<std::is_constructible_v<T, TArg> && !std::is_convertible_v<TArg, T>, int> = 0>
	constexpr TypeAdapter(TArg&& From)
		: Converted(std::forward<TArg>(From))
	{}

	constexpr TypeAdapter(TypeAdapter<T>&& From)
		: Converted{ std::move(From) }
	{}

	template<class TArg, std::enable_if_t<!std::is_constructible_v<T, TArg> && !std::is_convertible_v<TArg, T>, int> = 0>
	constexpr TypeAdapter(TArg&&)
		: Converted{}
	{}

	template<class TArg, std::enable_if_t<std::is_constructible_v<std::string_view, TArg> && std::is_arithmetic_v<T>, int> = 0>
	TypeAdapter(const TArg& Value)
	{
		std::istringstream{ std::string_view{ Value }.data() } >> Converted;
	}

	constexpr operator T&& ()
	{
		return std::move(static_cast<T&>(Converted));
	}

	T Converted;
};

template<class TVariant>
class ValueFunctionWrapper
{
public:
	template<class TValueRef>
	explicit ValueFunctionWrapper(std::reference_wrapper<TValueRef> ValueRef) noexcept
		: Function([ValueRef](TVariant&& Arg) {
			ValueRef.get() = AdapterVisitor<TValueRef>()(std::forward<TVariant>(Arg));
		})
	{}

	template<class FunctionType>
	explicit ValueFunctionWrapper(FunctionType&& Fx) noexcept
		: Function([F{ std::move(Fx) }](TVariant&& Arg) {
			F(AdapterVisitor<std::decay_t<FunctionArg<0, FunctionType>>>()(std::forward<TVariant>(Arg)));
		})
	{}

	inline operator const TVariant&() const
	{
		return Value;
	}

	inline operator TVariant&()
	{
		return Value;
	}

	template<class Ty>
	inline void operator=(const Ty& Val) const
	{
		Function(Value = Val);
	}

private:
	mutable TVariant Value;
	std::function<void(const TVariant)> Function;
};

using TValueFnMap = tsl::bhopscotch_map<std::wstring, ValueFunctionWrapper<TConfigValue>, std::hash<std::wstring>, TStringEqual, TStringLess>;
using TValueMap = tsl::bhopscotch_map<std::wstring, TConfigValue, std::hash<std::wstring>, TStringEqual, TStringLess>;

template<class TValueRefMap, class TIterator>
class ValueRefIter : public TIterator
{
public:
	ValueRefIter(const TValueRefMap& Map, TIterator Iter) noexcept 
		: TIterator(Iter), Map(Map) {}

	operator bool() const noexcept
	{
		return static_cast<TIterator>(*this) != Map.end();
	}

private:
	const TValueRefMap& Map;
};

template<class TValueRefMap>
using TValueRefIter = ValueRefIter<TValueRefMap, typename TValueRefMap::iterator>;

template<class TValueRefMap>
using TValueRefIterConst = ValueRefIter<TValueRefMap, typename TValueRefMap::const_iterator>;

template<class TValueRefMap>
class ValueRefMapWrapper : public TValueRefMap
{
	static inline std::hash<std::wstring_view> Hasher = std::hash<std::wstring_view>();

public:
	TValueRefIter<TValueRefMap> find(const std::wstring_view Key)
	{
		return { *this, TValueRefMap::find(Key, Hasher(Key)) };
	}

	TValueRefIterConst<TValueRefMap> find(const std::wstring_view Key) const
	{
		return { *this, TValueRefMap::find(Key, Hasher(Key)) };
	}
};

struct MiscSettings
{
	MiscSettings(GamePatcher* Patcher, D3D::GameInterface* Gi);

	const ValueFunctionWrapper<TConfigValue> EnableNoSpread;
	const ValueFunctionWrapper<TConfigValue> EnableNoBreath;
	const ValueFunctionWrapper<TConfigValue> EnableNoRecoil;
	const ValueFunctionWrapper<TConfigValue> EnableThirdPerson;
	const ValueFunctionWrapper<TConfigValue> BipodAlwaysSet;
	const ValueFunctionWrapper<TConfigValue> BipodEverywhere;
	const ValueFunctionWrapper<TConfigValue> BipodEveryWeapon;
};

struct ESPSettings
{
	ESPSettings(GamePatcher* Patcher, D3D::GameInterface* Interface);

	const ValueFunctionWrapper<TConfigValue> Enable3DMap;
	const ValueFunctionWrapper<TConfigValue> EnableRadar;
	const ValueFunctionWrapper<TConfigValue> EnableNametags;
	const ValueFunctionWrapper<TConfigValue> EnableIndicators;
	const ValueFunctionWrapper<TConfigValue> EnableEngineOutlines;
	const ValueFunctionWrapper<TConfigValue> EnableChams;
	const ValueFunctionWrapper<TConfigValue> FadeDistance;
	const ValueFunctionWrapper<TConfigValue> FadeDistanceNames;

	const ValueFunctionWrapper<TConfigValue> ColorFriendly;
	const ValueFunctionWrapper<TConfigValue> ColorFriendlyOccluded;
	const ValueFunctionWrapper<TConfigValue> ColorEnemy;
	const ValueFunctionWrapper<TConfigValue> ColorEnemyOccluded;

	const ValueFunctionWrapper<TConfigValue> DisableTerrainCulling;
};

struct SpottingSettings
{
	SpottingSettings(GamePatcher* Patcher, D3D::GameInterface* Interface, SpottingBehavior& Behavior);

	const ValueFunctionWrapper<TConfigValue> SpottingAlwaysActive;
	const ValueFunctionWrapper<TConfigValue> SpottingNoFOV;
	const ValueFunctionWrapper<TConfigValue> SpottingVisibility;
	const ValueFunctionWrapper<TConfigValue> SpottingTeam;
	const ValueFunctionWrapper<TConfigValue> SpottingOccluded;
	
	bool SpottingBot;
};

struct UtilitySettings
{
	UtilitySettings(GamePatcher* Patcher, D3D::GameInterface* Interface);

	const ValueFunctionWrapper<TConfigValue> InputType;
	const ValueFunctionWrapper<TConfigValue> ScreenshotPolicy;
	
	bool AimBot;
	bool TriggerBot;
};

struct AimSettings
{
	const std::unordered_map<std::string, EBones> BoneIdentMap =
	{
		{ "HEAD", BoneHead },
		{ "NECK", BoneNeck },
		{ "LEGS", BoneSpine2 },
		{ "ARMS", BoneSpine1 },
		{ "BODY", BoneSpine},
		{ "ANY", BoneAny }
	};

	AimSettings(
		RaycastBehavior& RcBehavior, 
		AimBehavior& AimBehavior, 
		TriggerBehavior& TriggerBehavior);

	const ValueFunctionWrapper<TConfigValue> AimBones;
	const ValueFunctionWrapper<TConfigValue> TriggerBone;
	const ValueFunctionWrapper<TConfigValue> AimCurves;
	const ValueFunctionWrapper<TConfigValue> RaycastBones;

	const ValueFunctionWrapper<TConfigValue> EnableTargetAllies;
	const ValueFunctionWrapper<TConfigValue> EnableTargetEnemies;
	const ValueFunctionWrapper<TConfigValue> EnableTargetSoldiers;
	const ValueFunctionWrapper<TConfigValue> EnableTargetVehicles;
	
	int AimKey;
};

class Utility
{
public:
	Utility(D3D::GameInterface* Gi, GamePatcher* Patcher, const ValueRefMapWrapper<TValueMap>& ConfigMap);
	Utility(D3D::GameInterface* Gi, GamePatcher* Patcher, const Default& = {});

	void LoadConfig(const ValueRefMapWrapper<TValueMap>& ConfigMap);
	void ExecuteKeyAction(InputController& Ic, uint16_t Key, bool Up);
	void ExecuteRoutine(InputController& Ic);

	template<class TValue>
	bool SetConfigValue(const std::wstring_view Key, TValue&& Value)
	{
		if (auto Iter = ValueFnRefMap.find(Key); Iter != ValueFnRefMap.end())
		{
			try
			{
				Iter->second = std::move(Value);
			}
			catch (const std::exception& Ex)
			{
				Logger::Error() << "Set config value exception for key: " << Key << " | " <<  Ex.what() << Logger::EndLine;
			}

			return true;
		}
		return false;
	}

	template<class TFn>
	bool GetConfigValue(const std::wstring_view Key, TFn Fn)
	{
		if (auto Iter = ValueFnRefMap.find(Key); Iter != ValueFnRefMap.end())
		{
			std::visit(Fn, static_cast<const TConfigValue&>(Iter->second));
			return true;
		}
		return false;
	}

	const MiscSettings& GetMiscSettings() const { return Misc; }
	const ESPSettings& GetESPSettings() const { return Esp; }
	const UtilitySettings& GetUtilitySettings() const { return Util; }
	const RaycastBehavior& GetRcBehavior() const { return RcBehavior; }

private:
	UtilitySettings Util;
	MiscSettings Misc;
	ESPSettings Esp;
	RaycastBehavior RcBehavior;
	SpottingBehavior SpBehavior;
	SpottingSettings Spotting;
	Aimbot Aimbot;
	Triggerbot Triggerbot;
	AimSettings Aim;
	AutoSpot AutoSpot;
	ValueRefMapWrapper<TValueFnMap> ValueFnRefMap;
};