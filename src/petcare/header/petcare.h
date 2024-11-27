#ifndef PETCARE_H
#define PETCARE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the hash table size
#define HASH_TABLE_SIZE 100

// Define the User structure
typedef struct User {
    char* username;
    char* encryptedPassword;
    struct User* next; // For handling collisions
} User;

// Define the HashTable structure
typedef struct HashTable {
    User* buckets[HASH_TABLE_SIZE];
} HashTable;

// Function declarations
HashTable* createHashTable();
unsigned int hashFunction(const char* str);
void addUser(HashTable* table, const char* username, const char* password);
int authenticateUser(HashTable* table, const char* username, const char* password);
void saveUsersToFile(HashTable* table, const char* filename);
void loadUsersFromFile(HashTable* table, const char* filename);
char* encryptPassword(const char* password);
void freeHashTable(HashTable* table);

#endif
