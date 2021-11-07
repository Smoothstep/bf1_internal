#pragma once

#include <Windows.h>

#undef min
#undef max

#define Match(Y, X)					\
	[&]() {							\
		switch(Y) { X }				\
	}()

#define Cat0(X, Y) X##Y
#define Cat1(X, Y) Cat0(X, Y)
#define Pad(X) uint8_t Cat1(Pad_, __COUNTER__)[X];

#include <stdio.h>
#include <chrono>
#include <variant>
#include <sstream>
#include <exception>
#include <string>
#include <string_view>

#include "Logger.h"

template<class TFrom, class TTo>
union Bitconvert
{
	TFrom From;
	TTo To;
};

template<class T>
constexpr bool IsValidPointer(T* Pointer)
{
	return 
		reinterpret_cast<uintptr_t>(Pointer) > 0x10000 && 
		reinterpret_cast<uintptr_t>(Pointer) < 0x0007FFFFFFFFFFFF;
}

template<class TTo, class TFrom>
inline TTo* CastPtr(const TFrom& Val)
{
	static_assert(sizeof(TFrom) == sizeof(TTo*));
	Bitconvert<TFrom, TTo*> Conv;
	Conv.From = Val;
	return Conv.To;
}

template<class T>
static void* GetVtbl()
{
	static T Obj;
	auto Vtbl = *reinterpret_cast<void**>(&Obj);
	return Vtbl;
}

template<class T>
inline void* CastPtr(T Val)
{
	auto Addr = std::addressof(Val);
	return *reinterpret_cast<void**>(Addr);
}

template<class T>
inline const void* OffsetPtr(const T* Base, size_t OffsetBytes)
{
	const uint8_t* Ptr = reinterpret_cast<const uint8_t*>(Base);
	return reinterpret_cast<const void*>(Ptr + OffsetBytes);
}

template<class T>
inline void* OffsetPtr(T* Base, size_t OffsetBytes)
{
	uint8_t* Ptr = reinterpret_cast<uint8_t*>(Base);
	return reinterpret_cast<void*>(Ptr + OffsetBytes);
}

template<class T>
uintptr_t* Ptr(T* Base, const size_t Offset = 0)
{
	return reinterpret_cast<uintptr_t*>(Base) + Offset;
}

static void DebugConsole()
{
	AllocConsole();
	FILE* pFile;
	freopen_s(&pFile, "conin$", "r", stdin);
	freopen_s(&pFile, "conout$", "w", stdout);
	freopen_s(&pFile, "conout$", "w", stderr);
}

template<class R, class T, class... Args>
__forceinline R CallVirtual(T * Base, size_t Index, Args&&... Arguments)
{
	return (*reinterpret_cast<R(__thiscall**)(T*, Args...)>(*reinterpret_cast<uintptr_t**>(Base) + Index))(Base, std::forward<Args>(Arguments)...);
}

template<class R, class T, class... Args>
__forceinline R CallMemberFunctionAddress(T * Base, void * Address, Args&&... Arguments)
{
	return (reinterpret_cast<R(__thiscall*)(T*, Args...)>(reinterpret_cast<uintptr_t*>(Address)))(Base, std::forward<Args>(Arguments)...);
}

template<class R, class... Args>
__forceinline R CallFunctionAddress(void* Address, Args&&... Arguments)
{
	return (reinterpret_cast<R(*)(Args...)>(*reinterpret_cast<uintptr_t**>(&Address)))(std::forward<Args>(Arguments)...);
}

class HrException : public std::runtime_error
{
public:
	HrException(HRESULT ResultCode, std::string_view What = {}) noexcept
		: std::runtime_error([ResultCode, What]() -> std::string {
			std::stringstream ss;

			ss  << "Failed with code: " 
				<< ResultCode
				<< std::endl
				<< "Message: " << What;

			return ss.str();
		}())
		, ResultCode(ResultCode)
	{}

	HRESULT Result() const
	{
		return ResultCode;
	}

private:
	HRESULT ResultCode;
};

__forceinline void ThrowOnError(HRESULT Hr, std::string_view Error = "Fail")
{
	if (FAILED(Hr))
	{
		throw HrException(Hr, Error);
	}
}

template<class TValue>
__forceinline TValue ThrowOnError(std::variant<TValue, HRESULT> Var, std::string_view Error = "Fail")
{
	if (std::holds_alternative<TValue>(Var))
	{
		return std::get<TValue>(Var);
	}
	else
	{
		if (auto Result = std::get<HRESULT>(Var); Result != S_OK)
		{
			throw HrException(Result, Error);
		}
	}

	throw std::runtime_error("Impossible variant received");
}

using Hrc = std::chrono::high_resolution_clock;
using TimePoint = Hrc::time_point;