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


// BFS ve DFS arama fonksiyonları için prototipler
void bfsSearch(Pet* petList, const char* searchKey);
void dfsSearch(Pet* petList, const char* searchKey);



// XOR Linked List Node
typedef struct Appointment {
    char petName[50];
    char description[100];
    int day;
    int month;
    char owner[50];
    struct Appointment* xorPtr; // XOR Pointer
} Appointment;

// XOR Linked List Functions
Appointment* XOR(Appointment* a, Appointment* b);
void addAppointment(const char* petName, const char* description, int day, int month, const char* owner, Pet* petList);
bool updateAppointment(const char* petName, int oldDay, int oldMonth, int newDay, int newMonth, const char* newDescription, const char* owner);
bool cancelAppointment(const char* petName, int day, int month, const char* owner);
void viewAppointments(int month);
// File operations for appointments
void saveAppointmentsToFile();
void loadAppointmentsFromFile();

#endif
