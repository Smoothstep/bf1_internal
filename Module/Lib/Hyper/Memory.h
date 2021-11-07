#pragma once

#ifdef MKL
#include <mkl.h>
#endif

#ifdef TBB
#include <tbb\tbb.h>
#endif

#include "Defines.h"

namespace Memoryspace
{
#ifndef COPY
#define COPY		memcpy
#endif

	class IMemoryManager
	{
	public:
		virtual FORCEINLINE void * Allocate
		(
			unsigned int Size
		) = 0;

		virtual FORCEINLINE void Deallocate
		(
			void * Memory
		) PURE;

		virtual FORCEINLINE void * Reallocate
		(
			void		*	Memory,
			unsigned int	Size
		) PURE;
	};

#ifdef MKL
	class MKLMemoryManager
	{
	public:
		static FORCEINLINE void * Allocate 
		(
			unsigned int Size
		)
		{
			return MKL_malloc(Size, 1);
		}

		static FORCEINLINE void Deallocate
		(
			void * Memory
		)
		{
			MKL_free(Memory);
		}

		static FORCEINLINE void * Reallocate
		(
			void		*	Memory,
			unsigned int	Size
		)
		{
			return MKL_realloc(Memory, Size);
		}
	};
#endif

#ifdef TBB
	class TBBMemoryManager
	{
	public:
		//tbb::tbb_allocator<int> Allocator;
	};

	static TBBMemoryManager Memory;
#endif

	class CMemoryManager
	{
	public:

		template
		<
			typename T
		>
		static FORCEINLINE T * New
		(
			unsigned int Size
		)
		{
			return new T[Size];
		}

		template
		<
			typename T
		>
		static FORCEINLINE void DeleteArray
		(
			T * Memory
		)
		{
			delete[] Memory;
		}

		template
		<
			typename T
		>
		static FORCEINLINE void Delete
		(
			T * Memory
		)
		{
			delete Memory;
		}
	};

	template
	<
		class Type
	> 
	struct TCopyable
	{
		template
		<
			class T
		> 
		FORCEINLINE static Type & FromMemory
		(
			const T * Memory
		)
		{	
			return *reinterpret_cast<const Type*>(Memory);
		}

		template
		<
			class T
		>
		FORCEINLINE void CopyFromMemory
		(
			const T * Memory
		)
		{	
			COPY(
				reinterpret_cast<void*>(this),
				reinterpret_cast<const void*>(Memory),
				sizeof(Type)
			);
		}

		template
		<
			class T
		>
		FORCEINLINE void ToMemory
		(
			T * Memory
		) const
		{	
			COPY(
				reinterpret_cast<void*>(Memory),
				reinterpret_cast<const void*>(this),
				sizeof(Type)
			);
		}

		template
		<
			class T
		>
		FORCEINLINE void ToSource
		(
			T & Source
		) const
		{
			static_assert(sizeof(T) == sizeof(Type), "Incorrect size of Source Type. Size must be 64");

			Source = *this;
		} 

		template
		<
			class T
		>
		FORCEINLINE static Type & FromSource
		(
			const T & Source
		)
		{
			static_assert(sizeof(T) == sizeof(Type), "Incorrect size of Source Type. Size must be 64");
	
			return *reinterpret_cast<Type*>(Source);
		}

		template
		<
			class T
		>
		FORCEINLINE void CopyFromSource
		(
			const T & Source
		)
		{
			static_assert(sizeof(T) == sizeof(Type), "Incorrect size of Source Type. Size must be 64");

			COPY(
				reinterpret_cast<void*>(this),
				reinterpret_cast<const void*>(&Source),
				sizeof(Type)
			);
		}
	};
}