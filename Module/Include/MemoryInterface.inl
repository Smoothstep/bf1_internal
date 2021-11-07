#pragma once

inline ScopedMemoryAccess::ScopedMemoryAccess(const void* Address, const size_t Size)
	: Size{ Size }, Address{ Address }
{
	VirtualProtect(const_cast<void*>(Address), Size, PAGE_EXECUTE_READWRITE, &OldProtection);
}

inline ScopedMemoryAccess::~ScopedMemoryAccess()
{
	VirtualProtect(const_cast<void*>(Address), Size, OldProtection, &OldProtection);
}

template<class TAllocator>
DefaultMemoryInterface<TAllocator>::DefaultMemoryInterface(TAllocator&& Allocator)
	: Allocator(std::move(Allocator))
{}

template<class TAllocator>
std::vector<uint8_t> DefaultMemoryInterface<TAllocator>::ReadMemory(const void* Address, const size_t Size)
{
	const ScopedMemoryAccess Access{ Address, Size };

	std::vector<uint8_t, TAllocator> Data(Size, Allocator);

	memcpy(Data.data(), Address, Size);

	return Data;
}

template<class TAllocator>
void* DefaultMemoryInterface<TAllocator>::WriteMemory(void* Address, const void* Bytes, const size_t Size)
{
	const ScopedMemoryAccess Access{ Address, Size };

	memcpy(Address, Bytes, Size);

	return reinterpret_cast<uint8_t*>(Address) + Size;
}

template<class TAllocator>
void* DefaultMemoryInterface<TAllocator>::ReadMemory(const void* Address, const size_t Size, void* Target)
{
	const ScopedMemoryAccess Access{ Address, Size };
	memcpy(Target, Address, Size);
	return Target;
}