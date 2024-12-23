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

// Feeding Schedule İşlev Prototipleri
void addFeedingSchedule(const char* petName, const char* scheduleDetails, Pet* petList);
void updateFeedingSchedule(const char* petName, const char* newDetails, Pet* petList);
void deleteFeedingSchedule(const char* petName, Pet* petList);
void viewFeedingSchedules(Pet* petList);

// Medicine Schedule İşlev Prototipleri
void addMedicineSchedule(const char* petName, const char* scheduleDetails, Pet* petList);
void updateMedicineSchedule(const char* petName, const char* newDetails, Pet* petList);
void deleteMedicineSchedule(const char* petName, Pet* petList);
void viewMedicineSchedules(Pet* petList);

// Feeding and Medicine Schedules File Operations
void saveFeedingSchedulesToFile();
void loadFeedingSchedulesFromFile();
void saveMedicineSchedulesToFile();
void loadMedicineSchedulesFromFile();


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


// Feeding Schedule için Queue tanımları
typedef struct FeedingSchedule {
    char petName[50];
    char scheduleDetails[100];
    struct FeedingSchedule* next;
} FeedingSchedule;

typedef struct Queue {
    FeedingSchedule* front;
    FeedingSchedule* rear;
} Queue;

//Global feedingschedule bildirimi
extern Queue* feedingQueue;
// Global medicineQueue bildirimi
extern Queue* medicineQueue;

// Queue işlemleri
Queue* createQueue();
void enqueue(Queue* queue, const char* petName, const char* scheduleDetails);
FeedingSchedule* dequeue(Queue* queue);
int isQueueEmpty(Queue* queue);

// Feeding Schedule işlevleri
void addFeedingSchedule(Queue* feedingQueue);
void updateFeedingSchedule(Queue* feedingQueue, const char* petName, const char* newDetails);
void deleteFeedingSchedule(Queue* feedingQueue, const char* petName);
void viewFeedingSchedules(Queue* feedingQueue);

//Medicine schedule işlevleri
void addMedicineSchedule(Queue* medicineQueue, const char* petName, const char* scheduleDetails);
void updateMedicineSchedule(Queue* medicineQueue, const char* petName, const char* newDetails);
void deleteMedicineSchedule(Queue* medicineQueue, const char* petName);
void viewMedicineSchedules(Queue* medicineQueue);
void findSCC();

// B+ Tree Node
typedef struct BPlusNode {
    int keys[10];
    int values[10];
    int count;
    struct BPlusNode* children[10];
} BPlusNode;

// B+ Tree
typedef struct BPlusTree {
    BPlusNode* root;
} BPlusTree;

// Function prototypes for B+ tree
BPlusTree* createBPlusTree();
void insertBirthday(BPlusTree* tree, const char* petName, int day, int month, int year);
bool isPetOwnedByUser(Pet* petList, const char* petName, const char* owner);

typedef struct Date {
    int day;
    int month;
    int year;
} Date;

void saveBirthdaysToFile(BPlusTree* birthdayTree, const char* filename, Pet* petList);
Pet* findPetByName(Pet* petList, int key);
void saveBPlusTreeToFile(BPlusNode* node, FILE* file, Pet* petList);
void loadBirthdaysFromFile(BPlusTree* birthdayTree, const char* filename, Pet** petList);
void listPetBirthdays(BPlusTree* birthdayTree, Pet* petList);

void addExerciseRoutine(const char* petName, const char* exercise);
void listAllExercises();
void undoLastExercise();


#define MAX_ROUTINES 100

typedef struct {
    char petName[50];
    char exercise[100];
} ExerciseRoutine;

typedef struct {
    ExerciseRoutine stack[MAX_ROUTINES];
    int top;
} ExerciseStack;

extern ExerciseStack exerciseStack; // Global değişken bildirimi


// Sokak hayvanı yapısı
typedef struct StrayAnimal {
    int id;
    char type[50];
    char gender[10];
    char arrivalDate[20];
    int age;
    struct StrayAnimal* next;
} StrayAnimal;

// Evlat edinilmiş hayvan yapısı
typedef struct AdoptedAnimal {
    int id;
    char type[50];
    char gender[10];
    char arrivalDate[20];
    int age;
    char owner[50];
    char adoptionDate[20];
    struct AdoptedAnimal* next;
} AdoptedAnimal;

// petcare.h
void adoptStrayAnimal(StrayAnimal** strayList,
    const char* activeUser,
    int chosenID,
    const char* newName,
    const char* adoptionDate);

// Sokak hayvanları (adoptable.dat) fonksiyonları
void loadStrayAnimalsFromFile(StrayAnimal** list, const char* filename);
void saveStrayAnimalsToFile(StrayAnimal* list, const char* filename);
void addStrayAnimalToList(StrayAnimal** list, const char* type, const char* gender,
    const char* arrivalDate, int age);
void updateStrayAnimal(
    StrayAnimal* list,
    int id,
    const char* newType,
    const char* newGender,
    const char* newArrivalDate,
    int newAge
);

void deleteStrayAnimal(StrayAnimal** list, int id);
void listStrayAnimals(StrayAnimal* list);

// Sokak hayvanlarını aramak için KMP
void searchStrayAnimalsKMP(StrayAnimal* list, const char* searchKey);
bool KMPcontains(const char* text, const char* pattern);


// Evlat edinme (adopted.dat) fonksiyonları
void loadAdoptedAnimalsFromFile(AdoptedAnimal** list, const char* filename);
void saveAdoptedAnimalsToFile(AdoptedAnimal* list, const char* filename);
// petcare.h
void adoptStrayAnimal(StrayAnimal** strayList, const char* activeUser);

void listAllAdoptedAnimals(AdoptedAnimal* list);
#endif
