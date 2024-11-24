#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const char *key;
    void *value;
    bool occupied;
} HashSlot;

typedef struct HashTable HashTable;

struct HashTable {
    size_t size;
    size_t elementCount;
    HashSlot **table;

    void (*set)(HashTable *this, const char *key, void *value);
    const char *(*get)(HashTable *this, const char *key);
    void (*delete)(HashTable *this, const char *key);
    bool (*has)(HashTable *this, const char *key);
    void (*free)(HashTable **this);
    size_t (*getSize)(HashTable *this);
    size_t (*count)(HashTable *this);
};

static void memAllocError(const char *err);
static uint32_t hash(const char *key, size_t size);
static void hashTableResize(HashTable *hashTable);
static HashSlot *createHashSlot();
static void freeNewTable(HashSlot **table, size_t size);
static void hashTableSet(HashTable *hashTable, const char *key, void *value);
static const char *hashTableGet(HashTable *hashTable, const char *key);
static void hashTableDelete(HashTable *hashTable, const char *key);
static bool hashTableHas(HashTable *hashTable, const char *key);
static void hashTableFree(HashTable **hashTablePtr);
static size_t hashTableSize(HashTable *hashTable);
static size_t hashTableCount(HashTable *hashTable);
HashTable *initHashTable(size_t initSize);

#endif