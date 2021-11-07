#include "stdafx.h"
#include "Crypto.h"
#include "ClientGame.h"

EncryptedPlayerManager::Iterator::Iterator(const EncryptedPlayerManager* Manager, uint32_t Index) noexcept
	: Manager{ Manager }
	, Index{ Index }
	, Player{ Manager->GetPlayerByIndex(Index) }
{}

bool EncryptedPlayerManager::Iterator::operator==(const Iterator& Other) const
{
	return Index == Other.Index;
}

bool EncryptedPlayerManager::Iterator::operator!=(const Iterator& Other) const
{
	return !(*this == Other);
}

EncryptedPlayerManager::Iterator& EncryptedPlayerManager::Iterator::operator++()
{
	if (Index >= Manager->MaxPlayers)
	{
		Index = Manager->MaxPlayers;
		Player = nullptr;
	}
	else
	{
		while (!(Player = Manager->GetPlayerByIndex(Index++)))
		{
			if (Index >= Manager->MaxPlayers)
			{
				break;
			}
		}
	}

	return *this;
}

EncryptedPlayerManager::Iterator& EncryptedPlayerManager::Iterator::operator--()
{
	if (Index == 0)
	{
		Index = Manager->MaxPlayers;
		Player = nullptr;
	}
	else
	{
		while (!(Player = Manager->GetPlayerByIndex(Index--)))
		{
			if (Index >= Manager->MaxPlayers)
			{
				Index = Manager->MaxPlayers;
				break;
			}
		}
	}

	return *this;
}

ClientPlayer* EncryptedPlayerManager::Iterator::operator*() const
{
	return Player;
}

template<class TKey, class TValue>
constexpr HashTableIterator<TKey, TValue> Hashtable<TKey, TValue>::Find(uint64_t Key)
{
	const uint64_t BucketIndex = uint32_t(Key) % BucketCount;

	for (HashTableEntry<TKey, TValue>* Entry = Buckets[BucketIndex]; Entry; Entry = Entry->Next)
	{
		if (Key == Entry->Value.first)
		{
			return HashTableIterator<TKey, TValue> { Entry, &Entry };
		}
	}

	return end();
}

template<class TKey, class TValue>
constexpr HashTableIterator<TKey, TValue> Hashtable<TKey, TValue>::begin() const
{
	return { Buckets[0], &Buckets[0] };
}

template<class TKey, class TValue>
constexpr HashTableIterator<TKey, TValue> Hashtable<TKey, TValue>::end() const
{
	return { Buckets[BucketCount], &Buckets[BucketCount] };
}

EncryptedPlayerManager* CryptoManager::PlayerManagerLocal()
{
	const ClientGameContext* ClientGameContext = ClientGameContext::Instance();

	if (!IsValidPointer(ClientGameContext))
	{
		return nullptr;
	}

	const ClientPlayerManager* PlayerManager = ClientPlayerManager::Instance();

	if (!IsValidPointer(PlayerManager))
	{
		return nullptr;
	}

	const uint64_t Key = PlayerManager->LocalPlayerList(EncKey);

	if (auto Iter = EncryptionHashTable.Find(Key); Iter != EncryptionHashTable.end())
	{
		return reinterpret_cast<EncryptedPlayerManager*>(**Iter.Entry);
	}

	return nullptr;
}

EncryptedPlayerManager* CryptoManager::PlayerManager()
{
	const ClientGameContext* ClientGameContext = ClientGameContext::Instance();

	if (!IsValidPointer(ClientGameContext))
	{
		return nullptr;
	}

	const ClientPlayerManager* PlayerManager = ClientPlayerManager::Instance();

	if (!IsValidPointer(PlayerManager))
	{
		return nullptr;
	}

	const uint64_t Key = PlayerManager->PlayerList(EncKey);

	if (auto Iter = EncryptionHashTable.Find(Key); Iter != EncryptionHashTable.end())
	{
		return reinterpret_cast<EncryptedPlayerManager*>(**Iter.Entry);
	}

	return nullptr;
}

ClientPlayer* EncryptedPlayerManager::GetPlayerByIndex(uint32_t Index) const
{
	const uint64_t XorValue1 = Xor2 ^ Xor0;
	const uint64_t* XorValue2 = reinterpret_cast<uint64_t*>(XorValue1 ^ Xor1);

	if (!IsValidPointer(XorValue2))
	{
		return nullptr;
	}

	const uint64_t XorPtr = XorValue2[Index];
	const uint64_t Player = XorValue1 ^ XorPtr;

	return reinterpret_cast<ClientPlayer*>(Player);
}

EncryptedPlayerManager::Iterator EncryptedPlayerManager::begin() const
{
	for (uint32_t N = 0; N < MaxPlayers; ++N)
	{
		if (GetPlayerByIndex(N))
		{
			return { this, N };
		}
	}

	return end();
}

EncryptedPlayerManager::Iterator EncryptedPlayerManager::end() const
{
	return { this, MaxPlayers };
}

template<class T>
T* CryptoManager::DecryptPtr(uint64_t EncryptedPtr, uint64_t PointerKey)
{
	return reinterpret_cast<T*>(DecryptRawPtr(EncryptedPtr, PointerKey));
}

void* CryptoManager::DecryptRawPtr(uint64_t EncryptedPtr, uint64_t PointerKey)
{
	if (!(EncryptedPtr & 0x8000000000000000))
	{
		return nullptr;
	}

	const uint64_t Key = PointerKey ^ EncKey;

	if (auto Iter = EncryptionHashTablePred.Find(Key); Iter != EncryptionHashTablePred.end())
	{
		uint64_t EncryptionKey = Key ^ **Iter.Entry;
		EncryptionKey ^= (7 * EncryptionKey);

		uint64_t DecryptedPtr = 0;
		uint8_t* DecryptedPtrBytes = reinterpret_cast<uint8_t*>(&DecryptedPtr);

		const uint8_t* pEncryptedPtrBytes = reinterpret_cast<const uint8_t*>(&EncryptedPtr);
		const uint8_t* pKeyBytes = reinterpret_cast<const uint8_t*>(&EncryptionKey);

		for (unsigned i = 0; i < 7; i++)
		{
			DecryptedPtrBytes[i] = (pKeyBytes[i] * 0x7A) ^ (pEncryptedPtrBytes[i] + pKeyBytes[i]);
			EncryptionKey += 8;
		}

		DecryptedPtrBytes[7] = pEncryptedPtrBytes[7];
		DecryptedPtr &= ~(0x8000000000000000);

		return CastPtr(DecryptedPtr);
	}

	return nullptr;
}	

namespace Crypto
{
	void* DecryptPointer(uint64_t EncryptedPtr, uint64_t PointerKey)
	{
		return CryptoManager::Instance()->DecryptRawPtr(EncryptedPtr, PointerKey);
	}

	ClientPlayer* GetPlayerById(uint32_t id)
	{
		auto* Mgr = CryptoManager::Instance()->PlayerManager();
		if (!Mgr)
		{
			return nullptr;
		}

		return Mgr->GetPlayerByIndex(id);
	}

	ClientPlayer* GetLocalPlayer(void)
	{
		auto* Mgr = CryptoManager::Instance()->PlayerManagerLocal();
		if (!Mgr)
		{
			return nullptr;
		}

		return Mgr->GetPlayerByIndex(0);
	}
}