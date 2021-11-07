#pragma once

#include <memory>
#include <vector>
#include <future>
#include <queue>
#include <variant>
#include <Windows.h>

class ScopedMemoryAccess
{
public:
	inline ScopedMemoryAccess(const void* Address, const size_t Size);
	inline ~ScopedMemoryAccess();

private:
	const size_t Size;
	const void* Address;
	DWORD OldProtection;
};

class IMemoryInterface
{
public:
	IMemoryInterface() = default;

	virtual std::vector<uint8_t> ReadMemory(const void* Address, const size_t Size) = 0;
	virtual void* WriteMemory(void* Address, const void* Bytes, const size_t Size) = 0;
	virtual void* ReadMemory(const void* Address, const size_t Size, void* Target) = 0;
};

template<class TAllocator = std::allocator<uint8_t>>
class DefaultMemoryInterface : public IMemoryInterface
{
public:
	DefaultMemoryInterface(TAllocator&& Allocator = {});

public:
	virtual std::vector<uint8_t> ReadMemory(const void* Address, const size_t Size) override;
	virtual void* WriteMemory(void * Address, const void * Bytes, const size_t Size) override;
	virtual void* ReadMemory(const void * Address, const size_t Size, void * Target) override;

private:
	TAllocator Allocator;
};

class BatchMemoryService
{
	struct WriteContent
	{
		void* Dst;
		const void* Src;
		std::promise<void*> Promise;
	};

	struct ReadContent
	{
		const void* Src;
		std::promise<std::vector<uint8_t>> Promise;
	};

	struct Content
	{
		size_t Size;
		std::variant<WriteContent, ReadContent> Union;
	};

	class Task
	{
	public:
		Task(Content& Data) noexcept;
		void Execute(IMemoryInterface& On);

	private:
		Content& Data;
	};

	class BatchMemoryInterface : public IMemoryInterface
	{
	public:
		BatchMemoryInterface(BatchMemoryService& Service);

	public:
		virtual std::vector<uint8_t> ReadMemory(const void* Address, const size_t Size) override;
		virtual void* WriteMemory(void* Address, const void* Bytes, const size_t Size) override;
		virtual void* ReadMemory(const void* Address, const size_t Size, void* Target) override;

	private:
		BatchMemoryService& Service;
	};

public:
	BatchMemoryService() noexcept;

	IMemoryInterface& Interface();
	void DispatchTasks();

protected:
	std::mutex QueueMutex;
	std::queue<Task> Queue;
	BatchMemoryInterface BatchInterface;
	std::thread::id DispatcherThreadId;
	DefaultMemoryInterface<> MemoryInterface;
};

#include "MemoryInterface.inl"