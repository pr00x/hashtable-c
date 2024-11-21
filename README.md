# Open Addressing Hash Table Library in C

The `Open Addressing Hash Table Library` in C implements a high-performance hash table using linear probing for collision resolution. It efficiently stores and retrieves key-value pairs with minimal overhead, leveraging the `FNV-1a (Fowler-Noll-Vo)` hash algorithm to compute hash values. This data structure is particularly suited for applications requiring fast lookups, insertions, and deletions with a focus on simplicity and scalability.

| Index |     Key     |   Value   |
|-------|-------------|-----------|
|   0   |   "key1"    |  "value1" |
|   1   |   "key2"    |  "value2" |
|   2   |   "key3"    |  "value3" |
|   3   |   "key4"    |  "value4" |
|   4   |   "key5"    |  "value5" |

## Features

- 🛠️ Initialization of hash table  
- ➕ Insertion of key-value pairs  
- 🔍 Retrieval of values by key  
- ❌ Deletion of key-value pairs  
- 🔑 Check if a key exists in the hash table  
- 🔄 Auto-resizing of the hash table  
- 🧹 Freeing of the hash table  
- 📏 Getting the size of the hash table (number of slots)  
- 🔢 Getting the count of elements in the hash table

## Installation

To use this library, you can either include the `hashtable.h` header file locally or install it system-wide using the provided `Makefile`.

#### Option 1: Include Locally
- Clone the repository

    ```Bash
    git clone https://github.com/pr00x/hashtable-c.git
    ```

- Include the header file in your project:

    ```c
    #include "hashtable.h"
    ```

- Compile your program along with the hash table source file:

    ```Bash
    gcc myprogram.c hashtable.c -o myprogram
    ```

#### Option 2: Install System-Wide
- Clone the repository:

    ```Bash
    git clone https://github.com/pr00x/hashtable-c.git
    ```
- Build and install the library and header file:
    ```Bash
    sudo make install
    ```

    This installs the library to `/usr/local/lib` and the header to `/usr/local/include`.
- Compile your program by linking to the installed library:
    ```Bash
    gcc myprogram.c -o myprogram -lhashtable
    ```

## Uninstallation

To remove the installed library and header files, run:

```Bash
sudo make uninstall
```

To remove the compiled library and header files from the source directory, run:

```Bash
sudo make clean
```

Uninstall and clean in one command:

```Bash
sudo make clean uninstall
```

## Usage/Example

```c
#include <stdio.h>
#include "hashtable.h" // or <hashtable.h> if it's installed

int main(void) {
    // Initialize the hash table with an initial size of 10
    HashTable *hashTable = initHashTable(10);

    // Insert key-value pairs into the hash table (animal names and habitats)
    hashTable->set(hashTable, "lion", "savannah");
    hashTable->set(hashTable, "tiger", "forest");
    hashTable->set(hashTable, "elephant", "grassland");
    hashTable->set(hashTable, "penguin", "antarctica");
    hashTable->set(hashTable, "kangaroo", "australian outback");

    // Retrieve and print the habitat for the "lion"
    printf("The lion lives in the: %s\n", hashTable->get(hashTable, "lion"));

    // Update the habitat of the "lion"
    hashTable->set(hashTable, "lion", "zoo");
    printf("The updated lion lives in the: %s\n", hashTable->get(hashTable, "lion"));

    // Check if "tiger" exists in the hash table
    if (hashTable->has(hashTable, "tiger"))
        puts("The tiger is in the hash table!");
    else
        puts("The tiger is not in the hash table.\n");

    // Delete the "penguin" entry from the hash table
    hashTable->delete(hashTable, "penguin");
    puts("Deleted the penguin entry.");

    // Check if "penguin" still exists after deletion
    if (hashTable->has(hashTable, "penguin"))
        puts("The penguin is still in the hash table!");
    else
        puts("The penguin has been deleted from the hash table.");

    // Print the size of the hash table (number of slots)
    printf("Size of hash table: %zu\n", hashTable->getSize(hashTable));

    // Print the count of elements currently in the hash table
    printf("Count of animals in hash table: %zu\n", hashTable->count(hashTable));

    // Print the contents of the hash table
    hashTable->print(hashTable);

    // Free the memory allocated for the hash table
    hashTable->free(hashTable);

    return 0;
}
```

### Performance and Efficiency

The Open Addressing Hash Table is designed to offer high performance for common operations, including **insertion**, **deletion**, and **lookup**, while minimizing memory overhead. Its efficiency is mainly determined by the **hash function**, **collision resolution method**, and **dynamic resizing** mechanism. Below is a summary of how these factors contribute to the overall performance:

#### ⏱️ Time Complexity

- **Insertion**: O(1) on average. If the table is not full and there are no collisions, new elements are inserted in constant time. However, if a collision occurs, linear probing is used to resolve it, but the average time complexity remains O(1) due to the low load factor.
  
- **Lookup**: O(1) on average. The key-value pair can be retrieved in constant time if no collisions or minimal probing is needed. The hash function computes the index quickly, and if collisions occur, the linear probing allows for quick access to the correct entry.
  
- **Deletion**: O(1) on average. Deletion of an element is done in constant time, with linear probing employed to find the key. Once the key is found, it is removed, and the table is reorganized if necessary to maintain the integrity of the hash table.

- **Resizing**: O(n) during rehashing. When the load factor exceeds 0.7, the hash table is resized (doubled), and all elements are rehashed to their new positions. This operation takes O(n) time but happens infrequently (only when resizing occurs).

#### 🧮 Space Efficiency

The hash table is **space-efficient**, using a simple array of slots for storage. The only additional memory overhead comes from the storage of keys and values. The resizing process involves creating a new, larger array, but this helps ensure the table can scale efficiently as more elements are added.

#### 🔀 Handling Collisions

Collisions are resolved using **linear probing**, which ensures that all elements remain in contiguous slots. While this method is simple and effective, it may cause performance degradation as the table becomes more crowded. To maintain efficiency, the table automatically resizes when the load factor exceeds 0.7.

#### 🔄 Amortized Time Complexity

While some operations, like resizing and rehashing, may temporarily increase the cost of operations, these events happen infrequently. Over many operations, the average cost of insertion, deletion, and lookup remains constant, making the table highly efficient for long-term use.

#### 📈 Scalability

The hash table is scalable, meaning it can efficiently grow as the number of stored elements increases. The resizing mechanism ensures that the hash table can handle large datasets without significant performance loss. 

By maintaining a low load factor and resizing dynamically, the hash table can handle a high number of elements while keeping operations efficient, even under heavy load.

## Author

[@ProX](https://www.github.com/pr00x)

## Contributing

Contributions are welcome! Please create a pull request or submit an issue for any feature requests or bug reports.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
