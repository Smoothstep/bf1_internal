#pragma once

#include "Offsets.h"
#include "ClassDecl.h"

namespace eastl
{
	class allocator;
	template<class T, class A = allocator>
	class vector;
}

class RayCastHit
{
public:
	Vector4f Position; //0x0000 
	Vector4f Normal; //0x0010 
	EPhysicsIds PhysicsId; //0x0020
	MaterialInfo Material; //0x0024
	uint32_t Part; //0x0028
	uint32_t Bone; //0x002C
	float Lambda; //0x0030
	Pad(0x0C);
}; //Size = 0x0070

class IRayCaster
{
public:
	virtual void F0() = 0;
	virtual bool Query(
		const char * Ident, 
		const Vector4f& From,
		const Vector4f& To,
		RayCastHit& Result, 
		int Flags, 
		eastl::vector<const PhysicsEntityBase*> * PhysicsEntityList) = 0;
};

class PhysicsManager
{
	virtual void F0() = 0;

public:
	Pad(0x80);
	class IRayCaster* RayCastDispatcher; //0x0088
};

class GameState
{
public:
	Pad(0xF8);
	float TimeIn;
	Pad(0x18);
	float TimePaused;
	Pad(0x18);
	float Time;
};

class ClientGameContext
{
	friend class ClientPlayerManager;
	friend class GameLevel;

	virtual void F0() = 0;

public:
	static ClientGameContext * Instance()
	{
		return *reinterpret_cast<ClientGameContext**>(EInstanceOffsets::Offset_GameCContext);
	}

	float GameTimeMs() const
	{
		if (!IsValidPointer(State))
		{
			return 0.0f;
		}

		return State->TimeIn;
	}

protected:
	Pad(0x28);
	class GameLevel* Level; //0x0030
	Pad(0x30);
	class ClientPlayerManager* ClientPlayerManager; //0x0068
	Pad(0x18);
	GameState* State;
};

class GameLevel
{
public:
	Pad(0x108);
	class PhysicsManager* PhysicsManager; //0x0108

	static GameLevel * GetCurrentLevel()
	{
		if (auto Ctx = ClientGameContext::Instance())
		{
			return Ctx->Level;
		}

		return nullptr;
	}
};

class ClientPlayerManager
{
public:
	constexpr uint64_t LocalPlayerList(uint64_t Key) const
	{
		return LocalPlayerListXor ^ Key;
	}

	constexpr uint64_t PlayerList(uint64_t Key) const
	{
		return PlayerListXor ^ Key;
	}

	static ClientPlayerManager * Instance()
	{
		if (auto Ctx = ClientGameContext::Instance())
		{
			return Ctx->ClientPlayerManager;
		}

		return nullptr;
	}

private:
	Pad(0xF0);
	uint64_t LocalPlayerListXor;
	uint64_t PlayerListXor;
};