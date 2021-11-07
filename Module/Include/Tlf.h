#pragma once

#include <Windows.h>
#include <stdint.h>

template<class T>
class TLFCollection
{
	static constexpr size_t Bits = sizeof(uint64_t) * 8;

public:
	TLFCollection()
		: FreeLevels{ ~0ULL }
	{
		std::memset(FreeBitmap, 0xFF, sizeof(FreeBitmap));
	}

	TLFCollection(TLFCollection&& Other)
		: FreeLevels{ Other.FreeLevels }
	{
		std::memcpy(FreeBitmap, Other.FreeBitmap, sizeof(FreeBitmap));
		Other.FreeLevels = ~0;
	}

	~TLFCollection()
	{
		ForEach([this](T& Value)
		{
			Value.~T();
		});
	}

	template<class TFn>
	void ForEach(TFn Fn)
	{
		DWORD Index;

		for (size_t N = 0; N < Bits; ++N)
		{
			uint64_t Bitmap = FreeBitmap[N];
			DWORD Start = 0;

			while (Bitmap != ~0)
			{
				if (!BitScanForward64(&Index, Bitmap))
				{
					for (uint64_t M = 0; M < Bits; ++M)
					{
						std::invoke(Fn, First()[N * Bits + M]);
					}

					break;
				}

				for (uint64_t M = Start; M < Index; ++M)
				{
					std::invoke(Fn, First()[N * Bits + M]);
				}

				Bitmap &= ~(1 << Index);
				Start = Index + 1;
			}
		}
	}

	template<class... TArgs>
	T* Construct(TArgs&&... Args)
	{
		DWORD IndexL0, IndexL1;

		if (BitScanForward64(&IndexL0, FreeLevels))
		{
			if (BitScanForward64(&IndexL1, FreeBitmap[IndexL0]))
			{
				if ((FreeBitmap[IndexL0] &= ~(1ULL << IndexL1)) == 0)
				{
					FreeLevels &= ~(1ULL << IndexL0);
				}

				return new (&First()[IndexL1]) T(std::forward<TArgs>(Args)...);
			}
		}

		return nullptr;
	}

	void Free(T* Ptr)
	{
		const uint64_t Index = std::distance(First(), Ptr);

		if (Index >= Bits * Bits)
		{
			return;
		}

		uint64_t L0 = Index >> 3;
		uint64_t L1 = Index - (L0 << 3);

		FreeLevels |= 1ULL << L0;
		FreeBitmap[L0] |= 1ULL << L1;

		Ptr->~T();
	}

private:
	T* First()
	{
		return reinterpret_cast<T*>(Memory);
	}

	uint8_t* Memory[Bits * Bits * sizeof(T)];

	uint64_t FreeBitmap[Bits];
	uint64_t FreeLevels;
};