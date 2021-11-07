#pragma once

#include <algorithm>
#include <stdint.h>

#include "FnValue.h"

namespace eastl
{
	template<class T, size_t NumElements>
	class FixedVector
	{
		T* First;
		T* Last;
		T* Alloc;
		void* EmptyClassObject;
		T Elements[NumElements];

	public:
		FixedVector() noexcept
			: First{ Elements }
			, Last{ Elements }
			, Alloc{ nullptr }
			, EmptyClassObject{ nullptr }
			, Elements{ }
		{
		}

		void Push(const T& Value)
		{
			if (Last >= &Elements[NumElements])
			{
				return;
			}

			*Last = Value;
			Last++;
		};

		void Push(T&& Value)
		{
			if (Last > First + NumElements)
			{
				return;
			}

			*Last = std::move(Value);
			Last++;
		};

		size_t Size() const
		{
			return std::distance(First, Last);
		}
	};
}
