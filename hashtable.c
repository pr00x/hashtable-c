/*
    Generic Open Addressing Hash Table Implementation in C
    Author: prox
    
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
    - Insertion (`ht_set`):
        Add a key-value pair to the hash table. Automatically handles collisions using linear probing.
    - Retrieval (`ht_get`):
        Retrieve the value associated with a given key.
    - Deletion (`ht_delete`):
        Remove a key-value pair from the hash table.
    - Existence Check (`ht_has`):
        Check if a specific key exists in the hash table.
    - Memory Management (`ht_free`):
          Clean up and release all memory used by the hash table.
    - Utility:
      - Get the number of slots (`ht_size`).
      - Get the number of elements (`ht_count`).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "hashtable.h"

static void mem_alloc_error(const char *err) {
    fprintf(stderr, "Cannot allocate a memory for %s.\n", err);
}

// FNV-1a (Fowler-Noll-Vo) Algorithm
static uint32_t hash(const char *key, size_t size) {
    uint32_t hash_value = 2166136261; // FNV offset basis

    for(size_t i = 0; key[i] != '\0'; i++) {
        hash_value ^= (unsigned char)key[i];
        hash_value *= 16777619; // FNV prime
    }

    return hash_value % size;
}

static HashSlot *create_hash_slot() {
    HashSlot *hash_slot = malloc(sizeof(HashSlot));

    if(!hash_slot)
        return NULL;

    hash_slot->key = NULL;
    hash_slot->value = NULL;

    return hash_slot;
}

static void free_new_table(HashSlot **table, size_t size) {
    // Free the new hash table
    for(size_t i = 0; i < size; i++)
        if(table[i])
            free(table[i]);

    free(table);
    table = NULL;
}

static bool ht_resize(HashTable *ht) {
    if(ht->size > SIZE_MAX / 2) {
        fprintf(stderr, "Hash table resize failed: cannot double size beyond maximum limit (SIZE_MAX).\n");
        return false;
    }
    
    size_t new_size = ht->size * 2;
    HashSlot **new_table = calloc(new_size, sizeof(HashSlot *));

    if(!new_table)
        return false;

    for(size_t i = 0; i < ht->size; i++) {
        HashSlot *current = ht->table[i];

        if(current && current != TOMBSTONE) {
            uint32_t new_index = hash(current->key, new_size);

            // Linear probing for an empty slot
            while(new_table[new_index])
                new_index = (new_index + 1) % new_size;

            // Create a new hash slot
            new_table[new_index] = create_hash_slot();

            if(!new_table[new_index]) {
                free_new_table(new_table, new_size);
                return false;
            }

            new_table[new_index]->key = current->key;
            new_table[new_index]->value = current->value;
            
            free(current);
            current = NULL;
        }
    }

    free(ht->table);
    ht->size = new_size;
    ht->table = new_table;

    return true;
}

bool ht_set(HashTable *ht, const char *key, void *value) {
    if(!ht || ht->size == 0) {
        fputs("Cannot set a value for an unallocated hash table.\n", stderr);
        return false;
    }
    else if(!key || !value) {
        fputs("Key or value cannot be NULL.\n", stderr);
        return false;
    }
    else if(*key == '\0') {
        fputs("Key cannot be an empty string.\n", stderr);
        return false;
    }

    if((float)(ht->element_count + 1) / (float)ht->size > LOAD_FACTOR_THRESHOLD)
        if(!ht_resize(ht)) {
            fprintf(stderr, "Hash table resize failed, cannot insert key '%s'.\n", key);
            return false;
        }
    
    uint32_t index = hash(key, ht->size);
    size_t first_tombstone = SIZE_MAX;

    while(ht->table[index]) {
        if(ht->table[index] == TOMBSTONE) {
            if(first_tombstone == SIZE_MAX)
                first_tombstone = index;
        }
        else if(strcmp(ht->table[index]->key, key) == 0) {
            ht->table[index]->value = value;
            return true;
        }

        index = (index + 1) % ht->size;
    }

    size_t insert_index = (first_tombstone != SIZE_MAX) ? first_tombstone : index;
    HashSlot *slot = create_hash_slot();

    if(!slot)
        return false;

    slot->key = key;
    slot->value = value;

    ht->table[insert_index] = slot;
    ht->element_count++;

    return true;
}

const void *ht_get(HashTable *ht, const char *key) {
    if(!ht || ht->element_count == 0 || !key || *key == '\0')
        return NULL;

    uint32_t index = hash(key, ht->size);

    // Linear probing to find the key
    while(ht->table[index]) {
        if(ht->table[index] != TOMBSTONE && strcmp(ht->table[index]->key, key) == 0)
            return ht->table[index]->value;

        index = (index + 1) % ht->size;
    }

    return NULL;
}

void ht_delete(HashTable *ht, const char *key) {
    if(!ht || ht->element_count == 0 || !key || *key == '\0')
        return;

    uint32_t index = hash(key, ht->size);

    // Linear probing to find and delete the key
    while(ht->table[index]) {
        if(ht->table[index] != TOMBSTONE && strcmp(ht->table[index]->key, key) == 0) {
            free(ht->table[index]);
            ht->table[index] = TOMBSTONE;
            ht->element_count--;

            return;
        }

        index = (index + 1) % ht->size;
    }
}

bool ht_has(HashTable *ht, const char *key) {
    if(!ht || ht->element_count == 0 || !key || *key == '\0')
        return false;

    uint32_t index = hash(key, ht->size);

    // Linear probing to find the key
    while(ht->table[index]) {
        if(ht->table[index] != TOMBSTONE && strcmp(ht->table[index]->key, key) == 0)
            return true;

        index = (index + 1) % ht->size;
    }

    return false;
}

void ht_free(HashTable **ht_ptr) {
    if(!ht_ptr || !*ht_ptr)
        return;

    HashTable *ht = *ht_ptr;

    if(!ht->table || ht->size == 0) {
        free(ht);
        *ht_ptr = NULL;

        return;
    }
    
    for(size_t i = 0; i < ht->size; i++) {
        if(ht->table[i] && ht->table[i] != TOMBSTONE) {
            free(ht->table[i]);
            ht->table[i] = NULL;
        }
    }

    free(ht->table);
    ht->table = NULL;
    ht->size = 0;
    ht->element_count = 0;

    free(ht);
    *ht_ptr = NULL;
}

size_t ht_size(HashTable *ht) {
    if(!ht) {
        fputs("Hash table is NULL.\n", stderr);
        return 0;
    }

    return ht->size;
}

size_t ht_count(HashTable *ht) {
    if(!ht) {
        fputs("Hash table is NULL.\n", stderr);
        return 0;
    }

    return ht->element_count;
}

HashTable *ht_init(size_t init_size) {
    HashTable *ht = malloc(sizeof(HashTable));

    if(!ht) {
        mem_alloc_error("hash table struct");
        return NULL;
    }

    init_size = init_size == 0 ? 1 : init_size;

    ht->size = init_size;
    ht->element_count = 0;
    ht->table = calloc(init_size, sizeof(HashSlot *));

    if(!ht->table) {
        free(ht);
        mem_alloc_error("hash table");

        return NULL;
    }

    return ht;
}