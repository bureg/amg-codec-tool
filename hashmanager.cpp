/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include <stdio.h>
#include <stdlib.h>

#include <QString>

#include "hashmanager.h"

HashManager::HashManager()
{
    /* Keep this line first */
    mmCnt = 0;

    /* Do everything else */
    pool    = (HashEntry*) mmMalloc(N_ENTRIES * sizeof(HashEntry));
    buckets = (HashEntry**) mmMalloc(N_BUCKETS * sizeof(HashEntry*));
    linearData = (HashEntry**) mmMalloc(N_ENTRIES * sizeof(HashEntry*));

    /* Init buckets */
    for(size_t i = 0; i < N_BUCKETS; i++)
    {
        buckets[i] = NULL;
    }

    /* Init pool */
    pool[ N_ENTRIES - 1 ].next = NULL;
    for(size_t i = 0; i < (N_ENTRIES - 1); i++)
    {
        pool[i].next = &pool[i+1];
    }

    /* Init entry manager */
    freeEntries = pool;

    /* Fill with 0-es */
    for(size_t i = 0; i < N_ENTRIES; i++)
    {
        AddEntry(0, i);
    }
}

HashManager::~HashManager()
{
    mmFree(buckets);
    mmFree(pool);
    mmFree(linearData);
    /* Check for memory leaks */
    assert(mmCnt == 0);
}

void HashManager::AddEntry(HashEntry* newEntry, quint8 value, size_t offset)
{
    HashEntry* bucket = buckets[ value % N_BUCKETS ];

    if(bucket != NULL)
    {
        bucket->prev = newEntry;
    }

    newEntry->value = value;
    newEntry->next = bucket;
    newEntry->prev = NULL;
    newEntry->offset = offset;

    buckets[ value % N_BUCKETS ] = newEntry;

    assert(offset < N_ENTRIES);
    linearData[ offset ] = newEntry;
}

void HashManager::AddEntry(quint8 value, size_t offset)
{
    //fprintf(stderr, "Add 0x%x to offset %d\n", value, offset);
    AddEntry(AllocEntry(), value, offset);
}

void HashManager::ClearByOffset(size_t offset, size_t length)
{
    for(size_t pos = offset; pos < (offset + length); pos++)
    {
        size_t wrappedPos = pos % N_ENTRIES;
        HashEntry* delEntry = linearData[ wrappedPos ];

        //fprintf(stderr, ">>> %d => 0x%lx\n", wrappedPos, (size_t)delEntry);
        if(delEntry != NULL)
        {
            HashEntry* prevEntry = delEntry->prev;
            HashEntry* nextEntry = delEntry->next;

#if 0
            if(delEntry->value == 18)
            {
                fprintf(stderr, "============ BEFORE ================\n");
                HashEntry* bkt = buckets[ delEntry->value % N_BUCKETS ];
                fprintf(stderr, ">>> Bucket: 0x%lx\n", (size_t)bkt);
                while(bkt)
                {
                    fprintf(stderr, ">>> Prev: 0x%lx\n", (size_t)bkt->prev);
                    fprintf(stderr, ">>> Next: 0x%lx\n", (size_t)bkt->next);
                    bkt = bkt->next;
                }
                fprintf(stderr, "===================================\n");
            }
#endif

            //fprintf(stderr, ">>> Prev: 0x%lx\n", (size_t)prevEntry);
            //fprintf(stderr, ">>> Next: 0x%lx\n", (size_t)nextEntry);

            if(nextEntry != NULL)
            {
                nextEntry->prev = prevEntry;
            }

            /* Case when bucket points to the deleted entry */
            if(prevEntry == NULL)
            {
                quint8 value = delEntry->value;
                buckets[ value % N_BUCKETS ] = nextEntry;
            }
            else
            {
                prevEntry->next = nextEntry;
            }

#if 0
            if(delEntry->value == 18)
            {
                fprintf(stderr, "============= AFTER ==============\n");
                HashEntry* bkt = buckets[ delEntry->value % N_BUCKETS ];
                fprintf(stderr, ">>> Bucket: 0x%lx\n", (size_t)bkt);
                while(bkt)
                {
                    fprintf(stderr, ">>> Prev: 0x%lx\n", (size_t)bkt->prev);
                    fprintf(stderr, ">>> Next: 0x%lx\n", (size_t)bkt->next);
                    bkt = bkt->next;
                }
                fprintf(stderr, "===================================\n");
            }
#endif

            /* Finally, release memory chunk */
            FreeEntry(delEntry);

            /* Destroy linear data link */
            linearData[ wrappedPos ] = NULL;
        }
    }
}

HashManager::HashEntry* HashManager::FindByValue(quint8 value)
{
    return buckets[ value % N_BUCKETS ];
}

HashManager::HashEntry* HashManager::AllocEntry()
{
    HashEntry* tmp = freeEntries;
    assert( tmp != NULL );
    freeEntries = freeEntries->next;
    return tmp;
}

void HashManager::FreeEntry(HashManager::HashEntry * entry)
{
    entry->next = freeEntries;
    freeEntries = entry;
}

void *HashManager::mmMalloc(size_t size)
{
    mmCnt++;
    return malloc(size);
}

void HashManager::mmFree(void *mem)
{
    mmCnt--;
    free(mem);
}
