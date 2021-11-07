#pragma once

#include <Windows.h>
#include <cstdint>
#include <functional>
#include "FnTraits.h"

enum ERegister
{
	RAX,
	RCX,
	RDX,
	RBX,
	RSP,
	RBP,
	RSI,
	RDI
};

enum EExtendedRegister
{
	R8,
	R9,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15
};

enum ESegments
{
	SS,
	GS,
	FS
};

#pragma pack(push, 1)
struct InstructionBase
{
	static constexpr bool Relative = false;

	uint8_t Op;
	constexpr InstructionBase(uint8_t Code) : Op(Code) {}

	template<class TInstruction, class = std::enable_if_t<TInstruction::OpCodeSecondary>>
	static constexpr bool HasSecondaryOpCode()
	{
		return true;
	}

	template<class TInstruction>
	static constexpr bool HasSecondaryOpCode()
	{
		return false;
	}
};

template<class TInstruction>
struct InstructionWrapper : public InstructionBase
{
	using InstructionBase::InstructionBase;
	using Base = InstructionWrapper<TInstruction>;

	auto begin() { return reinterpret_cast<uint8_t*>(this); }
	auto begin() const { return reinterpret_cast<const uint8_t*>(this); }

	auto end() { return std::addressof(reinterpret_cast<uint8_t*>(this)[sizeof(TInstruction)]); }
	auto end() const { return std::addressof(reinterpret_cast<const uint8_t*>(this)[sizeof(TInstruction)]); }

	static constexpr bool HasSecondaryOpCode_v = []() { return InstructionBase::HasSecondaryOpCode<TInstruction>(); }();
};

struct RetInstruction : public InstructionWrapper<RetInstruction>
{
	static constexpr uint8_t OpCode = 0xC3;

	constexpr RetInstruction() noexcept
		: Base(OpCode)
	{}
};

struct NopInstruction : public InstructionWrapper<NopInstruction>
{
	static constexpr uint8_t OpCode = 0x90;

	constexpr NopInstruction() noexcept
		: Base(OpCode)
	{}
};

template<class TSo, class TTo>
struct MovRegisterInstruction : public InstructionWrapper<MovRegisterInstruction<TSo, TTo>>
{
	static constexpr uint8_t OpCode = []() 
	{
		if constexpr (std::is_same<TTo, EExtendedRegister>::value)
		{
			return 0x49;
		}
		else
		{
			return 0x48;
		}
	}();

	static constexpr uint8_t OpMove = []() 
	{
		if constexpr (sizeof(TSo) == 4)
		{
			// Mov
			return 0xC7;
		}
		else if constexpr (sizeof(TSo) == 8)
		{
			// Mobabs
			return 0xB8;
		}
		else if constexpr (std::is_same<TSo, ERegister>::value)
		{
			return 0x49;
		}
		else if constexpr (std::is_same<TSo, EExtendedRegister>::value)
		{
			return 0x4D;
		}
		else
		{
			// ???
			return 0;
		}
	}();

	uint8_t OperandDst;
	TSo OperandSrc;

	constexpr MovRegisterInstruction(TTo Register, TSo Value) noexcept
		: InstructionWrapper<MovRegisterInstruction<TSo, TTo>>(OpCode)
		, OperandDst(OpMove | Register)
		, OperandSrc(Value)
	{}
};

using MovRegister64Instruction = MovRegisterInstruction<uint64_t, ERegister>;

struct PopRegisterInstruction : public InstructionWrapper<PopRegisterInstruction>
{
	static constexpr uint8_t OpCode = 0xFF;

	uint8_t Register;

	constexpr PopRegisterInstruction(ERegister Register) noexcept
		: Base(OpCode)
		, Register(Register)
	{}
};

template<class TSo>
struct PushValueInstruction : public InstructionWrapper<PushValueInstruction<TSo>>
{
	static constexpr uint8_t OpCode = 0x53;

	TSo Value;

	constexpr PushValueInstruction(TSo Value) noexcept
		: InstructionWrapper<PushValueInstruction<TSo>>(OpCode)
		, Value(Value)
	{}
};

struct PushRegisterInstruction : public InstructionWrapper<PushRegisterInstruction>
{
	static constexpr uint8_t OpCode = 0xFF;

	uint8_t Register;

	constexpr PushRegisterInstruction(ERegister Register) noexcept
		: Base(OpCode)
		, Register(Register)
	{}
};

struct CallRVAInstruction : public InstructionWrapper<CallRVAInstruction>
{
	static constexpr uint8_t OpCode = 0xE8;
	static constexpr bool Relative = true;

	uint32_t Target;
	uintptr_t AbsoluteAddress() const
	{
		return reinterpret_cast<uintptr_t>(this) + Target;
	}

	template<class T>
	constexpr CallRVAInstruction(T Target) noexcept
		: Base(OpCode)
		, Target(reinterpret_cast<uintptr_t>(Target) - reinterpret_cast<uintptr_t>(this))
	{}
};

struct CallRegisterInstruction : public InstructionWrapper<CallRegisterInstruction>
{
	static constexpr uint8_t OpCode = 0xFF;

	uint8_t Register;

	constexpr CallRegisterInstruction(ERegister Register) noexcept
		: Base(OpCode)
		, Register(0xD0 | Register)
	{}
};

struct AbsoluteJumpInstruction : public InstructionWrapper<AbsoluteJumpInstruction>
{
	static constexpr uint8_t OpCode = 0xFF;
	static constexpr uint8_t OpCodeSecondary = 0x25;

	uint8_t JumpType;
	uint32_t Diff;
	uintptr_t Target;

	constexpr AbsoluteJumpInstruction(uintptr_t Target) noexcept
		: Base(OpCode)
		, JumpType(OpCodeSecondary)
		, Diff(0)
		, Target(Target)
	{}
};

struct RelativeJumpInstruction : public InstructionWrapper<RelativeJumpInstruction>
{
	static constexpr uint8_t OpCode = 0xE9;
	static constexpr bool Relative = true;

	uintptr_t Target;
	uintptr_t AbsoluteAddress() const
	{
		return reinterpret_cast<uintptr_t>(this) + Target;
	}

	RelativeJumpInstruction(RelativeJumpInstruction&& Instr) noexcept
		: Base(OpCode)
		, Target(Instr.AbsoluteAddress() - reinterpret_cast<uintptr_t>(this))
	{}

	RelativeJumpInstruction(const RelativeJumpInstruction& Instr) noexcept
		: Base(OpCode)
		, Target(Instr.AbsoluteAddress() - reinterpret_cast<uintptr_t>(this))
	{}

	RelativeJumpInstruction(uintptr_t Target) noexcept
		: Base(OpCode)
		, Target(Target - reinterpret_cast<uintptr_t>(this))
	{}
};
#pragma pack(pop)

template<class TInstruction>
constexpr bool IsRelativeInstruction = std::conditional_t<TInstruction::Relative, std::true_type, std::false_type>::value;

template<class TInstruction, class... TRest>
struct InstructionParser
{
	template<class T>
	static constexpr size_t GetInstruction(uint8_t* Data, T Fn)
	{
		if constexpr (TInstruction::HasSecondaryOpCode_v)
		{
			if (TInstruction::OpCode == Data[0] &&
				TInstruction::OpCodeSecondary == Data[1])
			{
				std::invoke(Fn, reinterpret_cast<TInstruction*>(Data));
				return sizeof(TInstruction);
			}
			else if constexpr (sizeof...(TRest) > 0)
			{
				return InstructionParser<TRest...>::GetInstruction(Data, Fn);
			}
			else
			{
				return 0;
			}
		}
		else
		{
			if (TInstruction::OpCode == Data[0])
			{
				std::invoke(Fn, reinterpret_cast<TInstruction*>(Data));
				return sizeof(TInstruction);
			}
			else if constexpr (sizeof...(TRest) > 0)
			{
				return InstructionParser<TRest...>::GetInstruction(Data, Fn);
			}
			else
			{
				return 0;
			}
		}
	}
};

using InstructionParser_v = InstructionParser<NopInstruction, AbsoluteJumpInstruction, RelativeJumpInstruction, RetInstruction>;

template<class TDerived>
class RelocatableMemory
{
protected:
	void * Data;
	size_t Size;

	void Relocate(uint8_t * To)
	{
		for (size_t Index = 0; Index < Size;)
		{
			Index += InstructionParser_v::GetInstruction(&reinterpret_cast<uint8_t*>(Data)[Index], [&](auto* Instruction)
			{
				using TInstruction = std::remove_pointer_t<decltype(Instruction)>;

				if constexpr (IsRelativeInstruction<TInstruction>)
				{
					new (&To[Index]) TInstruction(std::move(*Instruction));
				}
			});
		}
	}

	TDerived& Derived()
	{
		return static_cast<TDerived&>(*this);
	}

	const TDerived& Derived() const
	{
		return static_cast<const TDerived&>(*this);
	}

public:
	constexpr RelocatableMemory(void* Memory = nullptr, size_t MemorySize = 0) noexcept
		: Data(Memory)
		, Size(MemorySize)
	{}

	constexpr RelocatableMemory(RelocatableMemory && Other) noexcept
		: Data(Other.Data)
		, Size(Other.Size)
	{
		Other.Data = nullptr;
		Other.Size = 0;
	}

	template<class TInPlace>
	bool Insert(TInPlace&& Value)
	{
		constexpr size_t NumBytes = sizeof(TInPlace);

		if (!Derived().Prepare(NumBytes))
		{
			return false;
		}

		new (&reinterpret_cast<uint8_t*>(Data)[Size]) TInPlace(std::move(Value));

		Size += NumBytes;

		return true;
	}

	bool Insert(const void* Bytes, size_t NumBytes)
	{
		if (!Derived().Prepare(NumBytes))
		{
			return false;
		}

		memcpy(&reinterpret_cast<uint8_t*>(Data)[Size], Bytes, NumBytes);
		Size += NumBytes;

		return true;
	}
};

class ExecutableMemory : public RelocatableMemory<ExecutableMemory>
{
	friend RelocatableMemory<ExecutableMemory>;
	static constexpr size_t Increment = 256;

protected:
	bool Prepare(size_t NumBytes)
	{
		if (Capacity < Size + NumBytes)
		{
			if (!Advancable)
			{
				return false;
			}

			Capacity += ((Size + NumBytes) / Increment + 1) * Increment;

			void* NewData = VirtualAlloc(nullptr, Capacity, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if (!NewData)
			{
				return false;
			}

			if (Data)
			{
				memcpy(NewData, Data, Size);
				if (!VirtualFree(Data, Capacity, MEM_FREE))
				{
					return false;
				}
			}

			Relocate(reinterpret_cast<uint8_t*>(Data = NewData));
		}

		return true;
	}

protected:
	size_t Capacity;
	bool Advancable;

	constexpr ExecutableMemory(void* MemoryLocation, size_t Capacity, size_t Size) noexcept
		: RelocatableMemory<ExecutableMemory>(MemoryLocation, Size)
		, Capacity(Capacity)
		, Advancable(false)
	{}

public:
	constexpr ExecutableMemory() noexcept
		: Capacity(0)
		, Advancable(true)
	{}

	constexpr ExecutableMemory(ExecutableMemory&& Other) noexcept
		: RelocatableMemory(std::move(Other))
		, Capacity(Other.Capacity)
		, Advancable(Other.Advancable)
	{
		Other.Capacity = 0;
	}

	ExecutableMemory(const ExecutableMemory& Other)
		: RelocatableMemory(VirtualAlloc(nullptr, Other.Capacity, MEM_COMMIT, PAGE_EXECUTE_READWRITE), Other.Size)
		, Capacity(Other.Capacity)
		, Advancable(Other.Advancable)
	{
		if (Data)
		{
			memcpy(Data, Other.Data, Size);
		}
	}

	ExecutableMemory& operator=(ExecutableMemory&& Other) noexcept
	{
		return *(new (this) ExecutableMemory(std::move(Other)));
	}

	ExecutableMemory& operator=(const ExecutableMemory& Other)
	{
		return *(new (this) ExecutableMemory(Other));
	}

	virtual ~ExecutableMemory()
	{
		if (Data)
		{
			VirtualFree(Data, Capacity, MEM_FREE);
		}
	}

	constexpr size_t Length() const
	{
		return Size;
	}

	constexpr size_t Reserved() const
	{
		return Capacity;
	}

	constexpr const void* Address() const
	{
		return Data;
	}
};

class ExecutableMemoryLocation : public ExecutableMemory
{
	DWORD PreviousProtection;

public:
	ExecutableMemoryLocation(void* MemoryLocation, size_t Capacity = 0, size_t Size = 0)
		: ExecutableMemory(MemoryLocation, Capacity, Size)
	{
		Advancable = false;
		VirtualProtect(Data, Capacity, PAGE_EXECUTE_READWRITE, &PreviousProtection);
	}

	virtual ~ExecutableMemoryLocation()
	{
		if (Data)
		{
			VirtualProtect(Data, Capacity, PreviousProtection, &PreviousProtection);
			Data = nullptr;
		}
	}
};

class InstructionSequence
{
	ExecutableMemory& Memory;

public:
	explicit InstructionSequence(ExecutableMemory& Location)
		: Memory(Location) {}

	template<class TInstruction>
	InstructionSequence& operator<<(const std::vector<TInstruction>& Instr)
	{
		if (!Memory.Insert(Instr.data(), Instr.size() * sizeof(TInstruction)))
		{
			throw std::runtime_error("Unable to write memory");
		}
		return *this;
	}

	template<class TInstruction>
	InstructionSequence& operator<<(const TInstruction& Instr)
	{
		if (!Memory.Insert(&Instr, sizeof(TInstruction)))
		{
			throw std::runtime_error("Unable to write memory");
		}
		return *this;
	}
};

