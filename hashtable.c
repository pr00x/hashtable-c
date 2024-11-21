/*
    Open Addressing Hash Table Implementation in C
    Author: Prox
    
    This file contains the implementation of an open addressing hash table,
    which supports common operations such as insertion, deletion, and lookup.
    The hash table uses linear probing to resolve collisions and the FNV-1a 
    (Fowler-Noll-Vo) hash algorithm to compute hash values.
    
    Functions:
    - Initialization of hash table
    - Insertion of key-value pairs
    - Retrieval of values by key
    - Deletion of key-value pairs
    - Check if a key exists in the hash table
    - Printing and freeing of the hash table
    - Get the size of the hash table (number of slots)
    - Get the count of elements in the hash table
*/

#include "hashtable.h"

#define LOAD_FACTOR_THRESHOLD 0.7

static void memAllocError(const char *err) {
    fprintf(stderr, "Cannot allocate a memory for %s.\n", err);
    exit(EXIT_FAILURE);
}

// FNV-1a (Fowler-Noll-Vo) Algorithm
static uint32_t hash(char *key, size_t size) {
    uint32_t hashValue = 2166136261; // FNV offset basis

    for(size_t i = 0; key[i] != '\0'; i++) {
        hashValue ^= key[i];
        hashValue *= 16777619; // FNV prime
    }

    return hashValue % size;
}

static HashSlot *createHashSlot() {
    HashSlot *hashSlot = malloc(sizeof(HashSlot));

    if(!hashSlot)
        return NULL;

    hashSlot->key = NULL;
    hashSlot->value = NULL;
    hashSlot->occupied = 0; // Initialize as unoccupied

    return hashSlot;
}

static void hashTableResize(HashTable *hashTable) {
    void freeNewTable(HashSlot **table, size_t size) {
        // Free the new hash table
        for(size_t i = 0; i < size; i++)
            if(table[i]) {
                if(table[i]->key)
                    free(table[i]->key);
                if(table[i]->value)
                    free(table[i]->value);

                free(table[i]);
            }

        free(table);
    }

    size_t newSize = hashTable->size * 2;
    HashSlot **newTable = calloc(newSize, sizeof(HashSlot *));

    if(!newTable) {
        hashTableFree(hashTable);
        memAllocError("new hash table");

        return;
    }

    for(size_t i = 0; i < hashTable->size; i++) {
        HashSlot *current = hashTable->table[i];

        if(current && current->occupied) {
            uint32_t newIndex = hash(current->key, newSize);

            // Linear probing for an empty slot
            while(newTable[newIndex] && newTable[newIndex]->occupied)
                newIndex = (newIndex + 1) % newSize;

            // Create a new hash slot
            newTable[newIndex] = createHashSlot();

            if(!newTable[newIndex]) {
                freeNewTable(newTable, newSize);
                hashTableFree(hashTable);
                memAllocError("new hash table slot");

                return;
            }

            newTable[newIndex]->key = strdup(current->key);

            if(!newTable[newIndex]->key) {
                freeNewTable(newTable, newSize);
                hashTableFree(hashTable);
                memAllocError("new hash table key");

                return;
            }

            newTable[newIndex]->value = strdup(current->value);
            
            if(!newTable[newIndex]->value) {
                freeNewTable(newTable, newSize);
                hashTableFree(hashTable);
                memAllocError("new hash table value");

                return;
            }

            newTable[newIndex]->occupied = 1;

            free(current->key);
            free(current->value);
        }
    }

    // Free the old table and update the hash table with the new one
    free(hashTable->table);
    hashTable->size = newSize;
    hashTable->table = newTable;
}

static void hashTableSet(HashTable *hashTable, char *key, char *value) {
    if(!hashTable || hashTable->size == 0) {
        fputs("Cannot set a value for an unallocated hash table.\n", stderr);
        return;
    }
    else if (!key || !value) {
        fputs("Key or value cannot be NULL.\n", stderr);
        return;
    }

    if((float)hashTable->elementCount / (float)hashTable->size > LOAD_FACTOR_THRESHOLD)
        hashTableResize(hashTable);
    
    size_t index = hash(key, hashTable->size);
    _Bool keyExists = 0;

    while(hashTable->table[index] && hashTable->table[index]->occupied) {
        if(strcmp(hashTable->table[index]->key, key) == 0) {
            keyExists = 1;
            break;
        }

        index = (index + 1) % hashTable->size;
    }

    if(!hashTable->table[index]) {
        // Allocate a memory for the new hash slot
        hashTable->table[index] = createHashSlot();

        if(!hashTable->table[index]) {
            hashTableFree(hashTable);
            memAllocError("hash slot");

            return;
        }
    }

    HashSlot *current = hashTable->table[index];

    if(!current->occupied)
        hashTable->elementCount++;

    // Allocate a memory for the key if key doesn't exist and free a value if it exists
    if(!keyExists) {
        current->key = strdup(key);

        if(!current->key) {
            hashTableFree(hashTable);
            memAllocError("hash key");

            return;
        }
    }
    else {
        if(current->value)
            free(current->value);
    }

    current->value = strdup(value);

    if(!current->value) {
        hashTableFree(hashTable);
        memAllocError("hash table value");

        return;
    }

    current->occupied = 1;
}

static const char *hashTableGet(HashTable *hashTable, char *key) {
    if(!hashTable || hashTable->elementCount == 0 || !key)
        return NULL;

    size_t index = hash(key, hashTable->size);

    // Linear probing to find the key
    while(hashTable->table[index] && hashTable->table[index]->occupied) {
        if(strcmp(hashTable->table[index]->key, key) == 0)
            return hashTable->table[index]->value;

        index = (index + 1) % hashTable->size;
    }

    return NULL;
}

static void hashTableDelete(HashTable *hashTable, char *key) {
    if(!hashTable || hashTable->elementCount == 0 || !key)
        return;

    size_t index = hash(key, hashTable->size);

    // Linear probing to find and delete the key
    while(hashTable->table[index] && hashTable->table[index]->occupied) {
        if(strcmp(hashTable->table[index]->key, key) == 0) {
            HashSlot *current = hashTable->table[index];
 
            if(current->key)
                free(current->key);
            
            if(current->value)
                free(current->value);
            
            current->occupied = 0;
            hashTable->elementCount--;

            break;
        }

        index = (index + 1) % hashTable->size;
    }
}

static _Bool hashTableHas(HashTable *hashTable, char *key) {
    if(!hashTable || hashTable->elementCount == 0 || !key)
        return 0;

    size_t index = hash(key, hashTable->size);

    // Linear probing to find the key
    while(hashTable->table[index] && hashTable->table[index]->occupied) {
        if(strcmp(hashTable->table[index]->key, key) == 0)
            return 1;

        index = (index + 1) % hashTable->size;
    }

    return 0;
}

static void hashTableFree(HashTable *hashTable) {
    if(!hashTable || !hashTable->table || hashTable->size == 0)
        return;
    
    for(size_t i = 0; i < hashTable->size; i++) {
        if(hashTable->table[i] && hashTable->table[i]->occupied) {
            free(hashTable->table[i]->key);
            free(hashTable->table[i]->value);
            free(hashTable->table[i]);
        }
    }

    free(hashTable->table);
    hashTable->table = NULL;

    hashTable->size = 0;
    hashTable->elementCount = 0;

    free(hashTable);
}

static void hashTablePrint(HashTable *hashTable) {    
    if(!hashTable || !hashTable->table)
        return;

    size_t count = 0;
    
    for(size_t i = 0; i < hashTable->size; i++)
        if(hashTable->table[i] && hashTable->table[i]->occupied)
            printf("%lu. {%s} -> {%s}\n", ++count, hashTable->table[i]->key, hashTable->table[i]->value);
}

static size_t hashTableSize(HashTable *hashTable) {
    if(!hashTable) {
        fputs("Hash table is NULL.\n", stderr);
        return 0;
    }

    return hashTable->size;
}

static size_t hashTableCount(HashTable *hashTable) {
    if(!hashTable) {
        fputs("Hash table is NULL.\n", stderr);
        return 0;
    }

    return hashTable->elementCount;
}

HashTable *initHashTable(size_t initSize) {
    HashTable *hashTable = malloc(sizeof(HashTable));

    if(!hashTable) {
        memAllocError("hash table struct");
        return NULL;
    }

    hashTable->size = initSize;
    hashTable->elementCount = 0;
    hashTable->table = calloc(initSize, sizeof(HashSlot *));

    if(!hashTable->table) {
        free(hashTable);
        memAllocError("hash table");

        return NULL;
    }
    
    hashTable->set = hashTableSet;
    hashTable->get = hashTableGet;
    hashTable->delete = hashTableDelete;
    hashTable->has = hashTableHas;
    hashTable->free = hashTableFree;
    hashTable->print = hashTablePrint;
    hashTable->getSize = hashTableSize;
    hashTable->count = hashTableCount;

    return hashTable;
}