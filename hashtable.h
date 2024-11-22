#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    char *key;
    char *value;
    _Bool occupied;
} HashSlot;

typedef struct HashTable HashTable;

struct HashTable {
    size_t size;
    size_t elementCount;
    HashSlot **table;

    void (*set)(HashTable *this, char *key, char *value);
    const char *(*get)(HashTable *this, char *key);
    void (*delete)(HashTable *this, char *key);
    _Bool (*has)(HashTable *this, char *key);
    void (*free)(HashTable *this);
    void (*print)(HashTable *this);
    size_t (*getSize)(HashTable *this);
    size_t (*count)(HashTable *this);
};

HashTable *initHashTable(size_t initSize);

#endif
