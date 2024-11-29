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

typedef struct Pet {
    char* name;
    char* type;
    int age;
    char* owner;
    struct Pet* prev;
    struct Pet* next;
} Pet;

void addPet(Pet** petList, const char* name, const char* type, int age, const char* owner);
void updatePet(Pet* petList, const char* name, const char* owner);
void deletePet(Pet** petList, const char* name, const char* owner);
void savePetsToFile(Pet* petList, const char* filename);
void loadPetsFromFile(Pet** petList, const char* filename);
void freePetList(Pet* petList);


// PetInfo yapısı (Heap Sort için kullanılıyor)
typedef struct PetInfo {
    char name[50];
    char type[50];
    int age;
    char owner[50];
} PetInfo;

// Heap Sort ve yardımcı fonksiyonlar
void heapify(PetInfo arr[], int n, int i);
void heapSort(PetInfo arr[], int n);

// List All Pets fonksiyonu
void listAllPets(Pet* petList);






#endif
