#pragma once

#include "Offsets.h"
#include "ClassDecl.h"

template<class TKey, class TValue>
struct HashTableEntry
{
	constexpr TValue operator*()
	{
		return Value.second;
	}

	constexpr const TValue operator*() const
	{
		return Value.second;
	}

	std::pair<TKey, TValue> Value;
	HashTableEntry<TKey, TValue>* Next;
};

template<class TKey, class TValue>
struct HashTableIterator
{
	constexpr bool operator==(const HashTableIterator& Other) const
	{
		return Entry == Other.Entry;
	}

	constexpr bool operator!=(const HashTableIterator& Other) const
	{
		return Entry != Other.Entry;
	}

	HashTableEntry<TKey, TValue>* Entry;
	HashTableEntry<TKey, TValue>** Bucket;
};

template<class TKey, class TValue>
class Hashtable
{
	using Iterator = HashTableIterator<TKey, TValue>;

	virtual ~Hashtable() = delete;

public:
	constexpr Iterator Find(uint64_t Key);
	constexpr Iterator begin() const;
	constexpr Iterator end() const;

private:
	HashTableEntry<TKey, TValue>** Buckets;
	uint32_t BucketCount;
	uint32_t Size;
};

class EncryptedPlayerManager
{
	class Iterator
	{
	public:
		Iterator(const EncryptedPlayerManager* Manager, uint32_t Index = 1) noexcept;

		bool operator==(const Iterator& Other) const;
		bool operator!=(const Iterator& Other) const;

		Iterator& operator++();
		Iterator& operator--();

		ClientPlayer* operator*() const;

	private:
		const EncryptedPlayerManager* Manager;
		uint32_t Index;
		ClientPlayer* Player;
	};

	virtual void F0() = 0;

public:
	ClientPlayer* GetPlayerByIndex(uint32_t Index) const;

	Iterator begin() const;
	Iterator end() const;

private:
	uint64_t Xor0; //0x08
	uint64_t Xor1; //0x10
	uint32_t MaxPlayers; //0x18
	uint32_t Unk;
	uint64_t Xor2; //0x20
};

class CryptoManager
{
protected:
	virtual ~CryptoManager() = delete;
	CryptoManager() = delete;

public:
	static CryptoManager* Instance()
	{
		return *reinterpret_cast<CryptoManager**>(EInstanceOffsets::Offset_CryptoMgr);
	}

	EncryptedPlayerManager* PlayerManager();
	EncryptedPlayerManager* PlayerManagerLocal();

	template<class T>
	T* DecryptPtr(uint64_t EncryptedPtr, uint64_t PointerKey);

	void* DecryptRawPtr(uint64_t EncryptedPtr, uint64_t PointerKey);

private:
	Hashtable<uint64_t, uint64_t> EncryptionHashTable;
	Pad(0x18);
	Hashtable<uint64_t, uint64_t> EncryptionHashTablePred;
	Pad(0x20);
	uint64_t EncKey;
};

namespace Crypto
{
	void* DecryptPointer(uint64_t EncryptedPtr, uint64_t PointerKey);

	ClientPlayer* GetPlayerById(uint32_t id);
	ClientPlayer* GetLocalPlayer(void);

	template<class Cb>
	uint32_t IteratePlayers(Cb Fn, const uint32_t StartingIdx = 1, const uint32_t MaxIdx = MaxPlayers)
	{
		auto* Mgr = CryptoManager::Instance()->PlayerManager();
		if (!Mgr)
		{
			return 0;
		}

		uint32_t Idx; 

		for (Idx = StartingIdx; Idx < MaxIdx; ++Idx)
		{
			ClientPlayer * Player = Mgr->GetPlayerByIndex(Idx);

			if (IsValidPointer(Player))
			{
				if (!Fn(Player))
				{
					break;
				}
			}
		}

		return StartingIdx - Idx;
	}
}