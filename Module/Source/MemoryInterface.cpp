#include "stdafx.h"
#include "MemoryInterface.h"

#include <stdint.h>
#include <assert.h>
#include <Windows.h>

BatchMemoryService::Task::Task(Content& Data) noexcept
	: Data{ Data }
{}

void BatchMemoryService::Task::Execute(IMemoryInterface& On)
{
	std::visit([&](auto& C)
	{
		using Type = std::decay_t<decltype(C)>;

		if constexpr (std::is_same_v<Type, WriteContent>)
		{
			C.Promise.set_value(On.WriteMemory(C.Dst, C.Src, Data.Size));
		}
		else
		{
			C.Promise.set_value(On.ReadMemory(C.Src, Data.Size));
		}
	}, Data.Union);
}

BatchMemoryService::BatchMemoryInterface::BatchMemoryInterface(BatchMemoryService& Service)
	: Service(Service)
{}

std::vector<uint8_t> BatchMemoryService::BatchMemoryInterface::ReadMemory(const void* Address, const size_t Size)
{
	if (std::this_thread::get_id() == Service.DispatcherThreadId)
	{
		return Service.MemoryInterface.ReadMemory(Address, Size);
	}

	Content Read{ Size, ReadContent{ Address, {} } };
	{
		std::lock_guard Lock{ Service.QueueMutex };
		Service.Queue.push(Read);
	}

	return std::get<ReadContent>(Read.Union).Promise.get_future().get();
}

void* BatchMemoryService::BatchMemoryInterface::ReadMemory(const void* Address, const size_t Size, void* Target)
{
	if (std::this_thread::get_id() == Service.DispatcherThreadId)
	{
		return Service.MemoryInterface.ReadMemory(Address, Size, Target);
	}

	Content Read{ Size, ReadContent{ Address, {} } };
	{
		std::lock_guard Lock{ Service.QueueMutex };
		Service.Queue.push(Read);
	}

	const auto& Bytes = std::get<ReadContent>(Read.Union).Promise.get_future().get();
	memcpy(Target, Bytes.data(), Bytes.size());
	return Target;
}

void* BatchMemoryService::BatchMemoryInterface::WriteMemory(void* Address, const void* Bytes, const size_t Size)
{
	if (std::this_thread::get_id() == Service.DispatcherThreadId)
	{
		return Service.MemoryInterface.WriteMemory(Address, Bytes, Size);
	}

	Content Write{ Size, WriteContent{ Address, Bytes, {} } };
	{
		std::lock_guard Lock{ Service.QueueMutex };
		Service.Queue.push(Write);
	}

	return std::get<WriteContent>(Write.Union).Promise.get_future().get();
}

BatchMemoryService::BatchMemoryService() noexcept
	: BatchInterface(*this)
	, DispatcherThreadId(std::this_thread::get_id())
{}

IMemoryInterface& BatchMemoryService::Interface()
{
	return BatchInterface;
}

void BatchMemoryService::DispatchTasks()
{
	assert(std::this_thread::get_id() == DispatcherThreadId);
	std::lock_guard Lock{ QueueMutex };
	
	while (!Queue.empty())
	{
		auto Task = Queue.front();
		Task.Execute(MemoryInterface);
		Queue.pop();
	}
}