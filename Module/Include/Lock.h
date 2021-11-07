#pragma once
#include <thread>
#include <optional>
#include <atomic>

class SpinLock
{
	class Guard
	{
	public:
		Guard(const Guard&) = delete;
		Guard(Guard&& Other) noexcept
			: Lock{ Other.Lock }
		{
			Other.Lock = {};
		}

		Guard(SpinLock& Lock)
			: Lock{ Lock }
		{
			while (Lock.Flag.test_and_set(std::memory_order_acquire))
			{
				std::this_thread::yield();
			}
		}

		~Guard()
		{
			if (Lock)
			{
				Lock->get().Flag.clear();
			}
		}

	private:
		std::optional<std::reference_wrapper<SpinLock>> Lock;
	};

public:
	constexpr SpinLock() = default;

	Guard Acquire()
	{
		return { *this };
	}

protected:
	std::atomic_flag Flag;
};
