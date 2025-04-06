#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>
#include <stdint.h>

#define LOAD_FACTOR_THRESHOLD 0.7
#define TOMBSTONE ((HashSlot *)(intptr_t)-1)

typedef struct {
    const char *key;
    void *value;
} HashSlot;

typedef struct HashTable {
    size_t size;
    size_t element_count;
    HashSlot **table;
} HashTable;

bool ht_set(HashTable *ht, const char *key, void *value);
const void *ht_get(HashTable *ht, const char *key);
void ht_delete(HashTable *ht, const char *key);
bool ht_has(HashTable *ht, const char *key);
void ht_free(HashTable **ht_ptr);
size_t ht_size(HashTable *ht);
size_t ht_count(HashTable *ht);
HashTable *ht_init(size_t initSize);

#endif