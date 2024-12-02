/*
    Generic Open Addressing Hash Table Implementation in C
    Author: Prox
    
    Description:
    This implementation provides a robust and efficient open addressing hash table 
    with support for standard operations such as insertion, deletion, lookup, and more. 
    The hash table uses **linear probing** for collision resolution and employs the 
    FNV-1a (Fowler-Noll-Vo) hashing algorithm to generate hash values.

    Features:
    - Insert Key-Value Pairs: Add data to the hash table using a string key.
    - Retrieve Values by Key: Quickly access stored data using its corresponding key.
    - Delete Entries: Remove key-value pairs from the hash table.
    - Check for Key Existence: Verify if a specific key is present.
    - Dynamic Resizing: Automatically resizes when the load factor exceeds a threshold, ensuring efficiency.
    - Generalized Data Storage: The hash table can store **any data type** as values, 
      provided the user supplies a pointer to the data.
    - Utility Functions: Free memory and get details like size 
      (number of slots) and element count (number of items stored).

    Notes:
    - Keys must be strings (`const char *`), and they should be immutable and valid for the 
      lifetime of the hash table.
    - Values are stored as generic `void *` pointers, allowing storage of any data type (e.g., integers, 
      floats, structs, or even other hash tables).
    - The user is responsible for managing memory associated with stored values.

    Functions:
    - Initialization:
        Create a new hash table with a specified initial size.
    - Insertion (`set`):
        Add a key-value pair to the hash table. Automatically handles collisions using linear probing.
    - Retrieval (`get`):
        Retrieve the value associated with a given key.
    - Deletion (`delete`):
        Remove a key-value pair from the hash table.
    - Existence Check (`has`):
        Check if a specific key exists in the hash table.
    - Memory Management (`free`):
          Clean up and release all memory used by the hash table.
    - Utility:
      - Get the number of slots (`getSize`).
      - Get the number of elements (`count`).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

#define LOAD_FACTOR_THRESHOLD 0.7

static void memAllocError(const char *err) {
    fprintf(stderr, "Cannot allocate a memory for %s.\n", err);
}

// FNV-1a (Fowler-Noll-Vo) Algorithm
static uint32_t hash(const char *key, size_t size) {
    uint32_t hashValue = 2166136261; // FNV offset basis

    for(size_t i = 0; key[i] != '\0'; i++) {
        hashValue ^= (unsigned char)key[i];
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
    hashSlot->occupied = false; // Initialize as unoccupied

    return hashSlot;
}

static void freeNewTable(HashSlot **table, size_t size) {
    // Free the new hash table
    for(size_t i = 0; i < size; i++)
        if(table[i])
            free(table[i]);

    free(table);
    table = NULL;
}

static void hashTableResize(HashTable *hashTable) {
    size_t newSize = hashTable->size * 2;
    HashSlot **newTable = calloc(newSize, sizeof(HashSlot *));

    if(!newTable) {
        hashTableFree(&hashTable);
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
                hashTableFree(&hashTable);
                memAllocError("new hash table slot");

                return;
            }

            newTable[newIndex]->key = current->key;
            newTable[newIndex]->value = current->value;
            newTable[newIndex]->occupied = true;
            
            free(current);
            current = NULL;
        }
    }

    // Free the old table and update the hash table with the new one
    if(hashTable->table)
        free(hashTable->table);

    hashTable->size = newSize;
    hashTable->table = newTable;
}

static void hashTableSet(HashTable *hashTable, const char *key, void *value) {
    if(!hashTable || hashTable->size == 0) {
        fputs("Cannot set a value for an unallocated hash table.\n", stderr);
        return;
    }
    else if(!key || !value) {
        fputs("Key or value cannot be NULL.\n", stderr);
        return;
    }
    else if(*key == '\0') {
        fputs("Key cannot be an empty string.\n", stderr);
        return;
    }

    if((float)hashTable->elementCount / (float)hashTable->size > LOAD_FACTOR_THRESHOLD)
        hashTableResize(hashTable);
    
    size_t index = hash(key, hashTable->size);

    while(hashTable->table[index] && hashTable->table[index]->occupied) {
        if(strcmp(hashTable->table[index]->key, key) == 0)
            break;

        index = (index + 1) % hashTable->size;
    }

    if(!hashTable->table[index]) {
        // Allocate a memory for the new hash slot
        hashTable->table[index] = createHashSlot();

        if(!hashTable->table[index]) {
            hashTableFree(&hashTable);
            memAllocError("hash slot");

            return;
        }
    }

    HashSlot *current = hashTable->table[index];

    if(!current->occupied)
        hashTable->elementCount++;

    current->key = key;
    current->value = value;
    current->occupied = true;
}

static const void *hashTableGet(HashTable *hashTable, const char *key) {
    if(!hashTable || hashTable->elementCount == 0 || !key || *key == '\0')
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

static void hashTableDelete(HashTable *hashTable, const char *key) {
    if(!hashTable || hashTable->elementCount == 0 || !key || *key == '\0')
        return;

    size_t index = hash(key, hashTable->size);

    // Linear probing to find and delete the key
    while(hashTable->table[index] && hashTable->table[index]->occupied) {
        if(strcmp(hashTable->table[index]->key, key) == 0) {
            HashSlot *current = hashTable->table[index];
            hashTable->table[index] = NULL; // Set the table slot to NULL before freeing it
             
            free(current);
            hashTable->elementCount--;

            break;
        }

        index = (index + 1) % hashTable->size;
    }
}

static bool hashTableHas(HashTable *hashTable, const char *key) {
    if(!hashTable || hashTable->elementCount == 0 || !key || *key == '\0')
        return false;

    size_t index = hash(key, hashTable->size);

    // Linear probing to find the key
    while(hashTable->table[index] && hashTable->table[index]->occupied) {
        if(strcmp(hashTable->table[index]->key, key) == 0)
            return true;

        index = (index + 1) % hashTable->size;
    }

    return false;
}

static void hashTableFree(HashTable **hashTablePtr) {
    HashTable *hashTable = *hashTablePtr;

    if(!hashTable || !hashTable->table || hashTable->size == 0)
        return;
    
    for(size_t i = 0; i < hashTable->size; i++) {
        if(hashTable->table[i] && hashTable->table[i]->occupied) {
            free(hashTable->table[i]);
            hashTable->table[i] = NULL;
        }
    }

    free(hashTable->table);
    hashTable->table = NULL;

    hashTable->size = 0;
    hashTable->elementCount = 0;

    free(hashTable);

    // Set the caller's pointer to NULL 
    *hashTablePtr = NULL;
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

    initSize = initSize <= 0 ? 1 : initSize;

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
    hashTable->getSize = hashTableSize;
    hashTable->count = hashTableCount;

    return hashTable;
}