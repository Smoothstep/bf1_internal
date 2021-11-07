#pragma once

#ifndef NULL
#define NULL 0
#endif
#ifndef INLINE
#define INLINE inline
#endif
#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif
#ifndef FASTCALL
#define FASTCALL __fastcall
#endif
#ifndef STDCALL
#define STDCALL __stdcall
#endif
#ifndef CDECL
#define CDECL __cdecl
#endif
#ifndef CONSTEXPR
#ifdef  CONSTEXPR_LITE
#define CONSTEXPR constexpr
#else
#define CONSTEXPR FORCEINLINE
#endif
#endif
#ifndef REINTERPRET_CAST
#define REINTERPRET_CAST reinterpret_cast
#endif
#ifndef STATIC_CAST
#define STATIC_CAST static_cast
#endif
#ifndef DYNAMIC_CAST
#define DYNAMIC_CAST dynamic_cast
#endif
#ifndef CONST_CAST
#define CONST_CAST const_cast
#endif
#ifndef DELETE
#define DELETE delete
#endif
#ifndef DELETE_ARRAY
#define DELETE_ARRAY delete[]
#endif
#ifndef RETURN
#define RETURN return
#endif
#ifndef STATIC
#define STATIC static
#endif
#ifndef TEMPLATE
#define TEMPLATE template
#endif
#ifndef CLASS
#define CLASS class
#endif
#ifndef STRUCT
#define STRUCT struct
#endif
#ifndef INTERFACE
#define INTERFACE interface
#endif
#ifndef IN
#define IN		_In_
#endif
#ifndef IN_OPT
#define IN_OPT	_In_opt_
#endif
#ifndef OUT
#define OUT		_Out_
#endif
#ifndef OUT_OPT
#define OUT_OPT	_Out_opt_
#endif
#ifndef IN_OUT
#define IN_OUT	_Inout_
#endif
#ifndef ALIGN
#define ALIGN(n) __declspec(align(n))
#endif
#ifndef RESTRICT
#define RESTRICT __restrict
#endif
#ifndef PURE
#define PURE =0
#endif
#ifndef Ensure
#ifdef _DEBUG
#define Ensure(x) (!(x))
#else
#define Ensure(x)
#endif
#endif
#ifndef __hidden
#define __hidden
#endif
#ifndef CONCATENATE_DIRECT
#define CONCATENATE_DIRECT(s1, s2) s1##s2
#endif
#ifndef CONCATENATE
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)
#endif
#ifndef PADDING
#define PADDING(Length) float CONCATENATE(Padding, __COUNTER__)[Length]
#endif
#ifndef PADDING_BYTE
#define PADDING_BYTE(Length) char CONCATENATE(Padding, __COUNTER__)[Length]
#endif
#ifndef GET_UUID
#define GET_UUID(X) __uuidof(X)
#endif
#ifndef CHECK_NULL_ARG
#define CHECK_NULL_ARG(X)		\
	if((X) == NULL)				\
	{							\
		return E_INVALIDARG;	\
	}
#endif