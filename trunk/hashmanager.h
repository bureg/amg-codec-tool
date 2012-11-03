/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef HASHMANAGER_H
#define HASHMANAGER_H

#include "datatypes.h"
#include "module.h"

class HashManager
{
public:
    HashManager();
    ~HashManager();

    struct HashEntry {
        HashEntry *prev;
        HashEntry *next;
        size_t offset;
        quint8 value;
    };

public:
    void AddEntry(quint8 value, size_t offset);
    void ClearByOffset(size_t offset, size_t length);
    HashEntry* FindByValue(quint8 value);

    /* Simple memory manager */
private:
    void* mmMalloc(size_t size);
    void mmFree(void* mem);

    size_t mmCnt;

private:
    /* Yeah, it's hardcoded. Should be initialized in the c-tor */
    static const size_t N_BUCKETS = 256;
    static const size_t N_ENTRIES = 256 * 8; /* = SECTION_SZ * N_SECTIONS */
    static const size_t ENC_BUF_SZ = 64;

    /* Entry manager */
    HashEntry* AllocEntry();
    void FreeEntry(HashEntry* entry);
    void AddEntry(HashEntry *entry, quint8 value, size_t offset);

private:
    HashEntry* pool;
    HashEntry* freeEntries;
    HashEntry** buckets;
    HashEntry** linearData;
};

#endif // HASHMANAGER_H
