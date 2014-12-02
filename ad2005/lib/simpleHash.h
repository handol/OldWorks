#ifndef SIMPLE_HASH_H
#define SIMPLE_HASH_H

#include <stdio.h>
#include <string.h>

#ifndef WIN32
#include <strings.h>
#endif

#include "HashUtil.h"
#include "simpleAlloc.h"

#ifndef uint2 
typedef unsigned short uint2;
#endif

#define	PRN	printf

/*
기본 가정: class HashItem 은 key 라는 필드를 포함하여야 한다.
key는 char * 타입이어야 한다.
key는 hash key값을 계산하기 위한 key필드이다.
*/
template <class HashItem>
class simpleHash
{

	/**
	교훈: 아래와 같이 정의한 type을 외부에서 사용할 수 없다. (hashSlotType)
	또한 이 클래스의 method을 return type으로도 사용할 수 없다.
	*/
	typedef struct Node
	{
		HashItem data;
		struct Node *next;
	} hashSlotType;
		
	
	public:
	
	simpleHash()
	{
		itemCount = 0;
		hashTable = NULL;
		hashCount = NULL;
		
	}
	
	~simpleHash()
	{
		if (hashTable) {
			delete [] hashTable;
			hashTable =  0;
		}
		if (hashCount) {
			delete [] hashCount;
			hashCount =  0;
		}
	}

	int init(int h_size);
	HashItem	*search(char *str);
	HashItem * insert(HashItem *new_item);
	HashItem * remove(char *str);

	int	size() {
		return hashTabSize;
	}
	int count()
	{
		return itemCount;
	}

	void print();

	private:
	
	int hashTabSize;
	int itemCount;
	hashSlotType **hashTable;
	uint2  *hashCount;

	simpleAlloc	mymem;
	
	void prn_chains(hashSlotType    *hptr);
};


/**
hash table 메모리를 할당.
return n: hash table 크기. 성공시에.
return 0: 메모리 할당 실패시.

bucket: one element in the hash table, # of buckets == table size
slot: slots per one bucket. if slot > 1, it's collision.
*/
template <class HashItem>
int simpleHash<HashItem>::init(int h_size)
{
	hashTabSize = HashUtil::getprime(h_size);
	hashTable = (hashSlotType **) new hashSlotType* [ hashTabSize];
	hashCount = (uint2 *) new uint2[hashTabSize];

	memset(hashCount, 0, sizeof(uint2) * hashTabSize );
	memset(hashTable, 0, sizeof(hashSlotType*) * hashTabSize );

	mymem.set ( sizeof(hashSlotType)*10 );
	
#ifdef BUG
	PRN("hashTable=%X, slotcnt=%X\n", hashTable, hashCount);
#endif
	if (hashTable==NULL || hashCount==NULL) return 0;

	else return hashTabSize;
}


/**
주어진 key를 찾으면 return TRUE (1)
아니면 return FALSE (0)
*/
template <class HashItem>
HashItem	*simpleHash<HashItem>::search(char *keystr)
{
	hashSlotType    *htmp, *hprv;

	if (hashTabSize == 0)
		return NULL;

	uint4 hval = HashUtil::hashfunc(keystr) % hashTabSize;
	for( hprv = NULL, htmp=hashTable[hval]
		; htmp != NULL && strcmp((char*)htmp->data.key, keystr) != 0
		; hprv = htmp, htmp = htmp->next )
	{
		;
	}

	if (htmp) return &htmp->data;
	return NULL;
}


/**
return 1: if given string is the first slot
return 1: if collision happed
return 0: if the same headslot alreay exists
args "keystr", "new_item" must have their own memory location, not temp. memory
*/
template <class HashItem>
//hashSlotType *simpleHash<HashItem>::insert(HashItem *new_item)
HashItem *simpleHash<HashItem>::insert(HashItem *new_item)
{
	hashSlotType    *htmp, *hprv;

	uint4 hval = HashUtil::hashfunc(new_item->key) % hashTabSize;
	for( hprv = NULL, htmp=hashTable[hval]
		; htmp != NULL && strcmp((char*)htmp->data.key, new_item->key) != 0
		; hprv = htmp, htmp = htmp->next )
	{
		;
	}

	if( htmp==NULL )  /* new one. so insert it */
	{		
		//htmp = new hashSlotType;
		htmp = (hashSlotType*) mymem.alloc( sizeof(hashSlotType) );
		if (htmp==NULL) return NULL;
		
		itemCount++;
		memcpy(&htmp->data, new_item, sizeof(HashItem) );
		htmp->next = NULL;
		hashCount[hval]++;
		if( hprv==NULL )						  // this new one is the first one.
		{
			hashTable[hval] = htmp;
		}
		else									  // collision
		{
			hprv->next = htmp;
		}
		return &htmp->data;
	}
	else	  /* not new one */
	{
		/* move to front on access */
		if( hprv!=NULL )
		{
			hprv->next = htmp->next;
			htmp->next = hashTable[hval];
			hashTable[hval] = htmp;
		}

		return NULL;
	}
}

/**
주어진 key값의 slot을 찾아서 delete 한다.
delete된 메모리 값을 return 한다. 이 함수의 사용자는 이 함수의 return 값을 참조해서는 안된다.
*/
template <class HashItem>
//hashSlotType * simpleHash<HashItem>::remove(char *keystr)
HashItem * simpleHash<HashItem>::remove(char *keystr)
{
	hashSlotType    *htmp, *hprv;
	HashItem *data;

	uint4 hval = HashUtil::hashfunc(keystr) % hashTabSize;
	for( hprv = NULL, htmp=hashTable[hval]
		; htmp != NULL && strcmp((char*)htmp->data.key, keystr) != 0
		; hprv = htmp, htmp = htmp->next )
	{
		;
	}

	if( htmp!=NULL )	 /* if found, remove it from the linked list */
	{
		itemCount--;
		hashCount[hval]--;
		
		if( hprv==NULL )						  // this new one is the first one.
		{
			hashTable[hval] = htmp->next;
		}
		else									  // collision
		{
			hprv->next = htmp->next;
		}

		data = & htmp->data;
		//delete htmp; -- Do not delete, because we use 'simpleAlloc".
		return data;
	}
	else	  /* not found */
	{
		return NULL;
	}

}

template <class HashItem>
void simpleHash<HashItem>::prn_chains(hashSlotType *hptr)
{
	for(  ; hptr != NULL; hptr = hptr->next)
		PRN("%s  ", hptr->data.key);
	PRN("\n");
}

template <class HashItem>
void simpleHash<HashItem>::print()
{
	int i;
	int non_empty=0, sum=0, max=0, max_index=0;
	int only_one_cnt=0;
	for(i=0; i<hashTabSize; i++)
	{
		if (hashCount[i] > 0)
		{
			non_empty++;
			sum += hashCount[i];
			if (hashCount[i] == 1) only_one_cnt++;
			if (hashCount[i] > max)
				{ max = hashCount[i]; max_index = i; }
		}
	}

	if (sum != itemCount) {
		PRN("Hash error: count wrong!! \n");
	}
	PRN("\nHash table size = %d\n", hashTabSize);
	PRN("Hash Item Count = %d\n", itemCount);
	PRN("total non-empty slots = %d, total non-empty buckets = %d\n", sum, non_empty);
	if (non_empty)
	PRN("Avg. slots per bucket: %.2f\n", (float)sum/(float)non_empty);

	PRN("Filled buckets = %d (%d%%)\n", non_empty, (100*non_empty)/hashTabSize);

	if (non_empty)
	PRN("No-collison buckets = %d (%d%%)\n", only_one_cnt, (100*only_one_cnt)/non_empty);

	PRN("Longest chain of slots: %d\n", max);
	if (max) prn_chains(hashTable[max_index]);

	mymem.print();
}

#endif
