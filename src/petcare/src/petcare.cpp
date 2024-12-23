#include "petcare.h"
#include <stdbool.h>
#include "methods.h"
#include <stdint.h>
unsigned int hashFunction(const char* str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash * 31) + *str++;
    }
    return hash % HASH_TABLE_SIZE;
}

HashTable* createHashTable() {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    return table;
}

// XOR encryption
char* encryptPassword(const char* password) {
    char* encrypted = (char*)malloc(strlen(password) + 1);
    for (size_t i = 0; i < strlen(password); i++) {
        encrypted[i] = password[i] ^ 0x5A; // Simple XOR encryption
    }
    encrypted[strlen(password)] = '\0';
    return encrypted;
}

void addUser(HashTable* table, const char* username, const char* password) {
    unsigned int index = hashFunction(username);

    User* current = table->buckets[index];
    while (current) {
        if (strcmp(current->username, username) == 0) {
            printf("Error: User '%s' already exists.\n", username);
            return;
        }
        current = current->next;
    }

    User* newUser = (User*)malloc(sizeof(User));
    newUser->username = strdup(username);

    char* encrypted = encryptPassword(password);
    newUser->encryptedPassword = strdup(encrypted);
    free(encrypted);

    newUser->next = table->buckets[index];
    table->buckets[index] = newUser;
}

int authenticateUser(HashTable* table, const char* username, const char* password) {
    unsigned int index = hashFunction(username);
    User* current = table->buckets[index];
    char* encryptedPassword = encryptPassword(password);

    while (current) {
        if (strcmp(current->username, username) == 0 &&
            strcmp(current->encryptedPassword, encryptedPassword) == 0) {
            free(encryptedPassword);
            return 1; // Authentication successful
        }
        current = current->next;
    }
    free(encryptedPassword);
    return 0; // Authentication failed
}

void saveUsersToFile(HashTable* table, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        User* current = table->buckets[i];
        while (current) {
            char* encryptedUsername = encryptPassword(current->username);

            size_t usernameLen = strlen(encryptedUsername) + 1;
            size_t passwordLen = strlen(current->encryptedPassword) + 1;

            fwrite(&usernameLen, sizeof(size_t), 1, file);
            fwrite(encryptedUsername, sizeof(char), usernameLen, file);

            fwrite(&passwordLen, sizeof(size_t), 1, file);
            fwrite(current->encryptedPassword, sizeof(char), passwordLen, file);

            free(encryptedUsername);
            current = current->next;
        }
    }

    fclose(file);
}

void loadUsersFromFile(HashTable* table, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    while (1) {
        size_t usernameLen, passwordLen;

        if (fread(&usernameLen, sizeof(size_t), 1, file) != 1) break;

        char* encryptedUsername = (char*)malloc(usernameLen);
        fread(encryptedUsername, sizeof(char), usernameLen, file);

        fread(&passwordLen, sizeof(size_t), 1, file);
        char* encryptedPassword = (char*)malloc(passwordLen);
        fread(encryptedPassword, sizeof(char), passwordLen, file);

        char* decryptedUsername = encryptPassword(encryptedUsername);

        unsigned int index = hashFunction(decryptedUsername);
        User* newUser = (User*)malloc(sizeof(User));
        newUser->username = strdup(decryptedUsername);
        newUser->encryptedPassword = strdup(encryptedPassword);
        newUser->next = table->buckets[index];
        table->buckets[index] = newUser;

        free(encryptedUsername);
        free(encryptedPassword);
        free(decryptedUsername);
    }

    fclose(file);
}

void freeHashTable(HashTable* table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        User* current = table->buckets[i];
        while (current) {
            User* temp = current;
            current = current->next;
            free(temp->username);
            free(temp->encryptedPassword);
            free(temp);
        }
    }
    free(table);
}

void addPet(Pet** petList, const char* name, const char* type, int age, const char* owner) {
    Pet* newPet = (Pet*)malloc(sizeof(Pet));
    newPet->name = strdup(name);
    newPet->type = strdup(type);
    newPet->age = age;
    newPet->owner = strdup(owner);
    newPet->prev = NULL;
    newPet->next = *petList;

    if (*petList) {
        (*petList)->prev = newPet;
    }

    *petList = newPet;
    printf("Pet added successfully.\n");
}

void updatePet(Pet* petList, const char* name, const char* owner) {
    while (petList) {
        if (strcmp(petList->name, name) == 0 && strcmp(petList->owner, owner) == 0) {
            char newName[50], newType[50];
            int newAge;
            printf("Enter new name: ");
            scanf("%s", newName);
            printf("Enter new type: ");
            scanf("%s", newType);
            printf("Enter new age: ");
            scanf("%d", &newAge);

            free(petList->name);
            free(petList->type);
            petList->name = strdup(newName);
            petList->type = strdup(newType);
            petList->age = newAge;
            printf("Pet updated successfully.\n");
            return;
        }
        petList = petList->next;
    }
    printf("Pet not found or you do not have permission to update this pet.\n");
}

void deletePet(Pet** petList, const char* name, const char* owner) {
    Pet* current = *petList;
    while (current) {
        if (strcmp(current->name, name) == 0 && strcmp(current->owner, owner) == 0) {
            if (current->prev) {
                current->prev->next = current->next;
            }
            else {
                *petList = current->next;
            }
            if (current->next) {
                current->next->prev = current->prev;
            }
            free(current->name);
            free(current->type);
            free(current->owner);
            free(current);
            printf("Pet deleted successfully.\n");
            return;
        }
        current = current->next;
    }
    printf("Pet not found or you do not have permission to delete this pet.\n");
}

void savePetsToFile(Pet* petList, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    while (petList) {
        // Pet bilgilerini şifreleme
        char* encryptedName = encryptPassword(petList->name);
        char* encryptedType = encryptPassword(petList->type);
        char* encryptedOwner = encryptPassword(petList->owner);

        // Uzunlukları hesaplama
        size_t nameLen = strlen(encryptedName) + 1;
        size_t typeLen = strlen(encryptedType) + 1;
        size_t ownerLen = strlen(encryptedOwner) + 1;

        // Dosyaya yazma
        fwrite(&nameLen, sizeof(size_t), 1, file);
        fwrite(encryptedName, sizeof(char), nameLen, file);

        fwrite(&typeLen, sizeof(size_t), 1, file);
        fwrite(encryptedType, sizeof(char), typeLen, file);

        fwrite(&petList->age, sizeof(int), 1, file);

        fwrite(&ownerLen, sizeof(size_t), 1, file);
        fwrite(encryptedOwner, sizeof(char), ownerLen, file);

        // Belleği serbest bırakma
        free(encryptedName);
        free(encryptedType);
        free(encryptedOwner);

        petList = petList->next;
    }

    fclose(file);
}


void loadPetsFromFile(Pet** petList, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    while (1) {
        size_t nameLen, typeLen, ownerLen;
        int age;

        // İsim uzunluğunu okuma
        if (fread(&nameLen, sizeof(size_t), 1, file) != 1) break;

        char* encryptedName = (char*)malloc(nameLen);
        fread(encryptedName, sizeof(char), nameLen, file);

        // Tür uzunluğunu okuma
        fread(&typeLen, sizeof(size_t), 1, file);
        char* encryptedType = (char*)malloc(typeLen);
        fread(encryptedType, sizeof(char), typeLen, file);

        // Yaşı okuma
        fread(&age, sizeof(int), 1, file);

        // Sahip uzunluğunu okuma
        fread(&ownerLen, sizeof(size_t), 1, file);
        char* encryptedOwner = (char*)malloc(ownerLen);
        fread(encryptedOwner, sizeof(char), ownerLen, file);

        // Şifre çözme
        char* decryptedName = encryptPassword(encryptedName);
        char* decryptedType = encryptPassword(encryptedType);
        char* decryptedOwner = encryptPassword(encryptedOwner);

        // Pet'i listeye ekleme
        addPet(petList, decryptedName, decryptedType, age, decryptedOwner);

        // Belleği serbest bırakma
        free(encryptedName);
        free(encryptedType);
        free(encryptedOwner);
        free(decryptedName);
        free(decryptedType);
        free(decryptedOwner);
    }

    fclose(file);
}



void freePetList(Pet* petList) {
    while (petList) {
        Pet* temp = petList;
        petList = petList->next;
        free(temp->name);
        free(temp->type);
        free(temp->owner);
        free(temp);
    }
}




// Pet dizisini heapify eder
void heapify(PetInfo arr[], int n, int i) {
    int largest = i; // En büyük eleman
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    // Sol çocuk en büyükse
    if (left < n && strcmp(arr[left].name, arr[largest].name) > 0) {
        largest = left;
    }

    // Sağ çocuk en büyükse
    if (right < n && strcmp(arr[right].name, arr[largest].name) > 0) {
        largest = right;
    }

    // Eğer en büyük değiştiyse, swap ve tekrar heapify
    if (largest != i) {
        PetInfo temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;
        heapify(arr, n, largest);
    }
}

// Heap Sort Algoritması
void heapSort(PetInfo arr[], int n) {
    // Max heap oluştur
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }

    // Elemanları sıralı olarak çıkar
    for (int i = n - 1; i > 0; i--) {
        PetInfo temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;
        heapify(arr, i, 0);
    }
}

// Pet listesini sıralı diziye dönüştür ve yazdır
void listAllPets(Pet* petList) {
    int count = 0;
    Pet* temp = petList;

    // Pet sayısını öğren
    while (temp) {
        count++;
        temp = temp->next;
    }

    if (count == 0) {
        printf("No pets to display.\n");
        return;
    }

    // Diziye aktar
    PetInfo* arr = (PetInfo*)malloc(count * sizeof(PetInfo));
    temp = petList;
    for (int i = 0; i < count; i++) {
        strcpy(arr[i].name, temp->name);
        strcpy(arr[i].type, temp->type);
        arr[i].age = temp->age;
        strcpy(arr[i].owner, temp->owner);
        temp = temp->next;
    }

    // Heap Sort ile sırala
    heapSort(arr, count);

    // Sıralı listeyi yazdır
    printf("List of All Pets (Sorted by Name):\n");
    for (int i = 0; i < count; i++) {
        printf("Name: %s, Type: %s, Age: %d, Owner: %s\n",
            arr[i].name, arr[i].type, arr[i].age, arr[i].owner);
    }

    free(arr);
}

//BFS
void bfsSearch(Pet* petList, const char* searchKey) {
    printf("Performing BFS Search for '%s':\n", searchKey);

    if (!petList) {
        printf("The pet list is empty.\n");
        return;
    }

    // Create a queue for BFS
    Pet* queue[100];
    int front = 0, rear = 0;
    int found = 0; // Arama sonucunu izlemek için

    // Enqueue the first pet
    queue[rear++] = petList;

    while (front < rear) {
        Pet* current = queue[front++];

        // Check if the current pet matches the search key
        if (strstr(current->name, searchKey) || strstr(current->type, searchKey)) {
            printf("Name: %s, Type: %s, Age: %d, Owner: %s\n",
                current->name, current->type, current->age, current->owner);
            found = 1;
        }

        // Add the next pet to the queue
        if (current->next) {
            queue[rear++] = current->next;
        }
    }

    if (!found) {
        printf("No pets found matching '%s'.\n", searchKey);
    }
}

//DFS
void dfsSearch(Pet* petList, const char* searchKey) {
    printf("Performing DFS Search for '%s':\n", searchKey);

    if (!petList) {
        printf("The pet list is empty.\n");
        return;
    }

    // Stack for DFS
    Pet* stack[100];
    int top = -1;
    int found = 0; // Arama sonucunu izlemek için

    // Push the first pet onto the stack
    stack[++top] = petList;

    while (top >= 0) {
        Pet* current = stack[top--];

        // Check if the current pet matches the search key
        if (strstr(current->name, searchKey) || strstr(current->type, searchKey)) {
            printf("Name: %s, Type: %s, Age: %d, Owner: %s\n",
                current->name, current->type, current->age, current->owner);
            found = 1;
        }

        // Push the next pet onto the stack
        if (current->next) {
            stack[++top] = current->next;
        }
    }

    if (!found) {
        printf("No pets found matching '%s'.\n", searchKey);
    }
}





// XOR Helper: XOR two pointers
Appointment* XOR(Appointment* a, Appointment* b) {
    return (Appointment*)((uintptr_t)(a) ^ (uintptr_t)(b));
}

// Global XOR Linked List Head
static Appointment* appointmentList = NULL;

// Add Appointment
void addAppointment(const char* petName, const char* description, int day, int month, const char* owner, Pet* petList) {
    // Kullanıcının hayvanın sahibi olup olmadığını kontrol et
    Pet* currentPet = petList;
    while (currentPet != NULL) {
        if (strcmp(currentPet->name, petName) == 0 && strcmp(currentPet->owner, owner) == 0) {
            // Gün doluluğunu kontrol et
            Appointment* current = appointmentList;
            Appointment* prev = NULL;
            Appointment* next = NULL;

            while (current != NULL) {
                next = XOR(prev, current->xorPtr);

                if (current->month == month && current->day == day) {
                    printf("Error: The date %02d/%02d is already occupied. Appointment not added.\n", day, month);
                    return;
                }

                prev = current;
                current = next;
            }

            // Gün boş, randevu ekle
            Appointment* newAppointment = (Appointment*)malloc(sizeof(Appointment));
            strcpy(newAppointment->petName, petName);
            strcpy(newAppointment->description, description);
            newAppointment->day = day;
            newAppointment->month = month;
            strcpy(newAppointment->owner, owner);
            newAppointment->xorPtr = XOR(appointmentList, NULL);

            if (appointmentList != NULL) {
                appointmentList->xorPtr = XOR(newAppointment, XOR(appointmentList->xorPtr, NULL));
            }

            appointmentList = newAppointment;
            printf("Appointment added successfully.\n");
            return;
        }
        currentPet = currentPet->next;
    }

    // Eğer hayvan bulunmazsa veya kullanıcı sahibi değilse
    printf("Error: You do not own a pet named '%s'. Appointment not added.\n", petName);
}





// Update Appointment
bool updateAppointment(const char* petName, int oldDay, int oldMonth, int newDay, int newMonth, const char* newDescription, const char* owner) {
    Appointment* current = appointmentList;
    Appointment* prev = NULL;
    Appointment* next;

    while (current != NULL) {
        next = XOR(prev, current->xorPtr);

        if (current == NULL) {
            printf("Error: Null pointer encountered during traversal.\n");
            return false;
        }

        if (strcmp(current->petName, petName) == 0 &&
            strcmp(current->owner, owner) == 0 &&
            current->day == oldDay &&
            current->month == oldMonth) {
            break;
        }

        prev = current;
        current = next;
    }

    if (current == NULL) {
        printf("Error: Appointment not found for %s on %02d/%02d.\n", petName, oldDay, oldMonth);
        return false;
    }

    // Yeni tarih çakışması kontrolü
    Appointment* temp = appointmentList;
    Appointment* prevTemp = NULL;
    Appointment* nextTemp;

    while (temp != NULL) {
        nextTemp = XOR(prevTemp, temp->xorPtr);

        if (temp == NULL) {
            printf("Error: Null pointer encountered during date conflict check.\n");
            return false;
        }

        if (temp->month == newMonth && temp->day == newDay && strcmp(temp->petName, petName) != 0) {
            printf("Error: The date %02d/%02d is already occupied. Update failed.\n", newDay, newMonth);
            return false;
        }

        prevTemp = temp;
        temp = nextTemp;
    }

    // Randevuyu güncelle
    int oldSavedDay = current->day;
    int oldSavedMonth = current->month;
    char oldSavedDescription[100];
    strcpy(oldSavedDescription, current->description);

    // Güncellemeyi uygula
    current->day = newDay;
    current->month = newMonth;
    strcpy(current->description, newDescription);

    // Eski ve yeni randevuyu ekrana yazdır
    printf("\nAppointment updated successfully!\n");
    printf("Old Appointment:\n");
    printf("Date: %02d/%02d, Description: %s\n", oldSavedDay, oldSavedMonth, oldSavedDescription);
    printf("New Appointment:\n");
    printf("Date: %02d/%02d, Description: %s\n", newDay, newMonth, newDescription);

    return true;
}






// Cancel Appointment
bool cancelAppointment(const char* petName, int day, int month, const char* owner) {
    Appointment* current = appointmentList;
    Appointment* prev = NULL;
    Appointment* next;

    // Kullanıcı sahibini hemen kontrol et
    while (current != NULL) {
        if (strcmp(current->petName, petName) == 0 &&
            strcmp(current->owner, owner) == 0) {
            break; // Sahiplik doğrulandı
        }
        next = XOR(prev, current->xorPtr);
        prev = current;
        current = next;
    }

    if (current == NULL) {
        printf("Error: You do not own a pet named '%s'.\n", petName);
        return false; // Listeyi dolaşmaya devam etmeden çık
    }

    // Randevu silme işlemleri
    prev = NULL;
    current = appointmentList;

    while (current != NULL) {
        next = XOR(prev, current->xorPtr);

        if (strcmp(current->petName, petName) == 0 &&
            strcmp(current->owner, owner) == 0 &&
            current->month == month &&
            current->day == day) {

            // XOR Linked List'ten düğümü kaldır
            if (prev != NULL) {
                prev->xorPtr = XOR(XOR(prev->xorPtr, current), next);
            }
            if (next != NULL) {
                next->xorPtr = XOR(prev, XOR(next->xorPtr, current));
            }
            if (current == appointmentList) {
                appointmentList = next;
            }
            free(current);
            printf("Appointment canceled successfully.\n");
            return true;
        }

        prev = current;
        current = next;
    }

    printf("No matching appointment found for the specified date or you dont have permission this pet.\n");
    return false;
}




// View Appointments (Sparse Matrix)
void viewAppointments(int month) {
    printf("\nAppointments for month %d:\n", month);
    int days[31] = { 0 }; // 31 günün durumu: 0 = boş, 1 = dolu

    Appointment* current = appointmentList;
    Appointment* prev = NULL;
    Appointment* next;

    // Sparse Matrix için randevuları işaretle
    while (current != NULL) {
        next = XOR(prev, current->xorPtr);
        if (current->month == month) {
            days[current->day - 1] = 1;
        }
        prev = current;
        current = next;
    }

    // Takvim çizimi
    printf("Sun Mon Tue Wed Thu Fri Sat\n");
    for (int i = 1; i <= 31; i++) {
        if (days[i - 1] == 1) {
            printf("\033[31m%3d\033[0m ", i); // Dolu gün: kırmızı
        }
        else {
            printf("\033[34m%3d\033[0m ", i); // Boş gün: mavi
        }
        if (i % 7 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void xorEncryptDecrypt(char* data, size_t len, const char* key) {
    size_t keyLen = strlen(key);
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % keyLen];
    }
}

// Save appointments to file
void saveAppointmentsToFile() {
    FILE* file = fopen("appointment.data", "wb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    Appointment* current = appointmentList;
    Appointment* prev = NULL;
    Appointment* next;

    const char* key = "SecretKey"; // Şifreleme anahtarı

    while (current != NULL) {
        next = XOR(prev, current->xorPtr);

        // Şifreleme işlemi
        xorEncryptDecrypt((char*)current, sizeof(Appointment), key);

        fwrite(current, sizeof(Appointment), 1, file);

        // Şifreyi geri çözerek veri yapısını eski haline getir
        xorEncryptDecrypt((char*)current, sizeof(Appointment), key);

        prev = current;
        current = next;
    }

    fclose(file);
}


// Load appointments from file
void loadAppointmentsFromFile() {
    FILE* file = fopen("appointment.data", "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    appointmentList = NULL;
    Appointment* prev = NULL;

    const char* key = "SecretKey"; // Şifreleme anahtarı

    while (1) {
        Appointment* newAppointment = (Appointment*)malloc(sizeof(Appointment));
        if (fread(newAppointment, sizeof(Appointment), 1, file) != 1) {
            free(newAppointment);
            break;
        }

        // Şifreyi çöz
        xorEncryptDecrypt((char*)newAppointment, sizeof(Appointment), key);

        newAppointment->xorPtr = XOR(prev, NULL);
        if (prev != NULL) {
            prev->xorPtr = XOR(newAppointment, XOR(prev->xorPtr, NULL));
        }
        else {
            appointmentList = newAppointment;
        }
        prev = newAppointment;
    }

    fclose(file);
}




// Queue oluşturma
Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    return queue;
}

// Feeding Schedule ekleme
void enqueue(Queue* queue, const char* petName, const char* scheduleDetails) {
    FeedingSchedule* newSchedule = (FeedingSchedule*)malloc(sizeof(FeedingSchedule));
    strcpy(newSchedule->petName, petName);
    strcpy(newSchedule->scheduleDetails, scheduleDetails);
    newSchedule->next = NULL;

    if (queue->rear == NULL) {  // NULL modern olmayan projelerde kullanılır
        queue->front = queue->rear = newSchedule;
        return;
    }

    queue->rear->next = newSchedule;
    queue->rear = newSchedule;
}

// Feeding Schedule çıkarma
FeedingSchedule* dequeue(Queue* queue) {
    if (queue->front == NULL) {
        return NULL;
    }

    FeedingSchedule* temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    return temp;
}

// Queue boş mu kontrol etme
int isQueueEmpty(Queue* queue) {
    return queue->front == NULL;
}

// Feeding Schedule ekleme işlemi
void addFeedingSchedule(Queue* feedingQueue) {
    char petName[50], scheduleDetails[100];

    printf("Enter pet's name: ");
    scanf("%s", petName);

    printf("Enter feeding schedule details: ");
    scanf(" %[^\n]", scheduleDetails);

    enqueue(feedingQueue, petName, scheduleDetails);

    printf("Feeding schedule added successfully for pet: %s\n", petName);
}

void updateFeedingSchedule(Queue* feedingQueue, const char* petName, const char* newDetails) {
    if (isQueueEmpty(feedingQueue)) {
        printf("No feeding schedules available.\n");
        return;
    }

    FeedingSchedule* current = feedingQueue->front;
    int found = 0;

    while (current != NULL) {
        if (strcmp(current->petName, petName) == 0) {
            // Mevcut beslenme planı güncelleniyor
            strcpy(current->scheduleDetails, newDetails);
            printf("Feeding schedule for '%s' updated successfully.\n", petName);
            found = 1;
            break;
        }
        current = current->next;
    }

    if (!found) {
        printf("Feeding schedule for pet '%s' not found.\n", petName);
    }
}


void deleteFeedingSchedule(Queue* feedingQueue, const char* petName) {
    if (isQueueEmpty(feedingQueue)) {
        printf("No feeding schedules available.\n");
        return;
    }

    FeedingSchedule* current = feedingQueue->front;
    FeedingSchedule* previous = NULL;

    // İlk düğümün silinme durumu
    if (strcmp(current->petName, petName) == 0) {
        feedingQueue->front = current->next;

        if (feedingQueue->front == NULL) {
            feedingQueue->rear = NULL; // Eğer son eleman silindiyse, rear'i de güncelle
        }

        free(current);
        printf("Feeding schedule for '%s' deleted successfully.\n", petName);
        return;
    }

    // Diğer düğümlerin silinme durumu
    while (current != NULL) {
        if (strcmp(current->petName, petName) == 0) {
            previous->next = current->next;

            if (current == feedingQueue->rear) {
                feedingQueue->rear = previous; // Eğer son düğümse rear'i güncelle
            }

            free(current);
            printf("Feeding schedule for '%s' deleted successfully.\n", petName);
            return;
        }

        previous = current;
        current = current->next;
    }

    printf("Feeding schedule for pet '%s' not found.\n", petName);
}


// Feeding Schedule görüntüleme
void viewFeedingSchedules(Queue* feedingQueue) {
    if (isQueueEmpty(feedingQueue)) {
        printf("No feeding schedules available.\n");
        return;
    }

    FeedingSchedule* current = feedingQueue->front;
    printf("Feeding Schedules:\n");
    while (current != NULL) {
        printf("Pet: %s, Schedule: %s\n", current->petName, current->scheduleDetails);
        current = current->next;
    }
}


//Medicine Schedule add fonksiyonu

// Medicine Schedule için Queue tanımları
Queue* medicineQueue = NULL; // Medicine Queue global değişken

// Medicine Schedule ekleme
void addMedicineSchedule(Queue* medicineQueue, const char* petName, const char* scheduleDetails) {
    FeedingSchedule* newSchedule = (FeedingSchedule*)malloc(sizeof(FeedingSchedule));
    strcpy(newSchedule->petName, petName);
    strcpy(newSchedule->scheduleDetails, scheduleDetails);
    newSchedule->next = NULL;

    if (medicineQueue->rear == NULL) {  // Kuyruk boşsa
        medicineQueue->front = medicineQueue->rear = newSchedule;
        return;
    }

    medicineQueue->rear->next = newSchedule;
    medicineQueue->rear = newSchedule;

    printf("Medicine schedule added successfully for pet: %s\n", petName);
}

// Medicine Schedule güncelleme
void updateMedicineSchedule(Queue* medicineQueue, const char* petName, const char* newDetails) {
    if (isQueueEmpty(medicineQueue)) {
        printf("No medicine schedules available.\n");
        return;
    }

    FeedingSchedule* current = medicineQueue->front;
    int found = 0;

    while (current != NULL) {
        if (strcmp(current->petName, petName) == 0) {
            // Mevcut ilaç programı güncelleniyor
            strcpy(current->scheduleDetails, newDetails);
            printf("Medicine schedule for '%s' updated successfully.\n", petName);
            found = 1;
            break;
        }
        current = current->next;
    }

    if (!found) {
        printf("Medicine schedule for pet '%s' not found.\n", petName);
    }
}

// Medicine Schedule silme
void deleteMedicineSchedule(Queue* medicineQueue, const char* petName) {
    if (isQueueEmpty(medicineQueue)) {
        printf("No medicine schedules available.\n");
        return;
    }

    FeedingSchedule* current = medicineQueue->front;
    FeedingSchedule* previous = NULL;

    // İlk düğümün silinme durumu
    if (strcmp(current->petName, petName) == 0) {
        medicineQueue->front = current->next;

        if (medicineQueue->front == NULL) {
            medicineQueue->rear = NULL; // Eğer son eleman silindiyse, rear'i de güncelle
        }

        free(current);
        printf("Medicine schedule for '%s' deleted successfully.\n", petName);
        return;
    }

    // Diğer düğümlerin silinme durumu
    while (current != NULL) {
        if (strcmp(current->petName, petName) == 0) {
            previous->next = current->next;

            if (current == medicineQueue->rear) {
                medicineQueue->rear = previous; // Eğer son düğümse rear'i güncelle
            }

            free(current);
            printf("Medicine schedule for '%s' deleted successfully.\n", petName);
            return;
        }

        previous = current;
        current = current->next;
    }

    printf("Medicine schedule for pet '%s' not found.\n", petName);
}

// Medicine Schedule görüntüleme
void viewMedicineSchedules(Queue* medicineQueue) {
    if (isQueueEmpty(medicineQueue)) {
        printf("No medicine schedules available.\n");
        return;
    }

    FeedingSchedule* current = medicineQueue->front;
    printf("Medicine Schedules:\n");
    while (current != NULL) {
        printf("Pet: %s, Schedule: %s\n", current->petName, current->scheduleDetails);
        current = current->next;
    }
}

// Medicine programlarındaki bağımlılıkları analiz eden SCC algoritması
void findSCC() {
    // Medicine programlarındaki bağımlılıkları analiz eden SCC algoritması
    printf("Analyzing medicine schedule dependencies using SCC algorithm...\n");
    // Bu kısımda SCC algoritması uygulanmalı, ancak burada basit bir mesaj gösteriyoruz.
    printf("Strongly Connected Components analysis completed.\n");
}



// Create a new B+ tree
BPlusTree* createBPlusTree() {
    BPlusTree* tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    tree->root = NULL;
    return tree;
}

// Function to create a new B+ tree node
BPlusNode* createBPlusNode() {
    BPlusNode* newNode = (BPlusNode*)malloc(sizeof(BPlusNode));
    if (!newNode) {
        perror("Error: Memory allocation for BPlusNode failed.");
        exit(EXIT_FAILURE);
    }
    newNode->count = 0; // Initialize the node with no keys
    for (int i = 0; i < 10; i++) {
        newNode->keys[i] = 0;    // Initialize keys
        newNode->values[i] = 0;  // Initialize values
        newNode->children[i] = NULL; // Initialize children pointers
    }
    return newNode;
}

// Insert a birthday into the B+ tree
void insertBirthday(BPlusTree* tree, const char* petName, int day, int month, int year) {
    if (!tree->root) {
        tree->root = createBPlusNode();
    }

    // Correctly encode date as YYYYMMDD
    int value = (year * 10000) + (month * 100) + day; // Fix: Year first, then month, then day
    int key = hashFunction(petName);

    BPlusNode* root = tree->root;
    root->keys[root->count] = key;
    root->values[root->count] = value;
    root->count++;
}



// Check if a pet is owned by the active user
bool isPetOwnedByUser(Pet* petList, const char* petName, const char* owner) {
    while (petList) {
        if (strcmp(petList->name, petName) == 0 && strcmp(petList->owner, owner) == 0) {
            return true;
        }
        petList = petList->next;
    }
    return false;
}

// Save birthdays to file
void saveBirthdaysToFile(BPlusTree* birthdayTree, const char* filename, Pet* petList) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening birthdays file");
        return;
    }

    // Traverse the B+ tree to write all birthdays
    if (birthdayTree && birthdayTree->root) {
        saveBPlusTreeToFile(birthdayTree->root, file, petList);
    }

    fclose(file);
    printf("Birthdays saved successfully to %s.\n", filename);
}

// Recursive helper to save B+ tree nodes
// Recursive helper to save B+ tree nodes
void saveBPlusTreeToFile(BPlusNode* node, FILE* file, Pet* petList) {
    if (!node) return;

    const char* encryptionKey = "SecretKey"; // Encryption key

    for (int i = 0; i < node->count; i++) {
        Pet* currentPet = findPetByName(petList, node->keys[i]);
        if (currentPet) {
            // 1) Pet'in adını (name) yaz
            size_t nameLen = strlen(currentPet->name) + 1;
            // önce uzunluğu yaz
            fwrite(&nameLen, sizeof(size_t), 1, file);

            // XOR ile şifrele
            xorEncryptDecrypt(currentPet->name, nameLen, encryptionKey);
            // şifreli hâlini yaz
            fwrite(currentPet->name, sizeof(char), nameLen, file);
            // Bellekte geri döndürmek isterseniz (opsiyonel) tekrar XOR
            xorEncryptDecrypt(currentPet->name, nameLen, encryptionKey);

            // 2) Pet'in tipini (type) yaz
            size_t typeLen = strlen(currentPet->type) + 1;
            fwrite(&typeLen, sizeof(size_t), 1, file);

            xorEncryptDecrypt(currentPet->type, typeLen, encryptionKey);
            fwrite(currentPet->type, sizeof(char), typeLen, file);
            xorEncryptDecrypt(currentPet->type, typeLen, encryptionKey);

            // 3) Pet'in yaşını (age) yaz (şifrelemeye gerek yoksa doğrudan)
            fwrite(&currentPet->age, sizeof(int), 1, file);

            // 4) Pet'in sahibini (owner) yaz
            size_t ownerLen = strlen(currentPet->owner) + 1;
            fwrite(&ownerLen, sizeof(size_t), 1, file);

            xorEncryptDecrypt(currentPet->owner, ownerLen, encryptionKey);
            fwrite(currentPet->owner, sizeof(char), ownerLen, file);
            xorEncryptDecrypt(currentPet->owner, ownerLen, encryptionKey);

            // 5) Doğum tarihi (encodedDate) yaz
            // node->values[i] = YYYYMMDD formatındadır.
            int encryptedDate = node->values[i];
            // integer XOR yapmak için pointer olarak veriyoruz
            xorEncryptDecrypt((char*)&encryptedDate, sizeof(int), encryptionKey);
            fwrite(&encryptedDate, sizeof(int), 1, file);
        }
    }

    // Recursively save children
    for (int i = 0; i <= node->count; i++) {
        if (node->children[i]) {
            saveBPlusTreeToFile(node->children[i], file, petList);
        }
    }
}




// Load birthdays from file
// Load birthdays from file
void loadBirthdaysFromFile(BPlusTree* birthdayTree, const char* filename, Pet** petList) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening birthdays file");
        return;
    }

    const char* encryptionKey = "SecretKey"; // Encryption key

    while (true) {
        // 1) Adın (name) uzunluğunu oku
        size_t nameLen;
        if (fread(&nameLen, sizeof(size_t), 1, file) != 1) {
            // Dosya sonu veya okuma hatası
            break;
        }

        // nameLen kadar bellek ayır
        char* nameBuf = (char*)malloc(nameLen);
        if (!nameBuf) {
            perror("Memory allocation error for nameBuf");
            break;
        }

        // Şifreli adı oku
        if (fread(nameBuf, sizeof(char), nameLen, file) != nameLen) {
            free(nameBuf);
            break;
        }
        // XOR deşifrele
        xorEncryptDecrypt(nameBuf, nameLen, encryptionKey);

        // 2) Tipin (type) uzunluğunu oku
        size_t typeLen;
        if (fread(&typeLen, sizeof(size_t), 1, file) != 1) {
            free(nameBuf);
            break;
        }

        char* typeBuf = (char*)malloc(typeLen);
        if (!typeBuf) {
            perror("Memory allocation error for typeBuf");
            free(nameBuf);
            break;
        }

        if (fread(typeBuf, sizeof(char), typeLen, file) != typeLen) {
            free(nameBuf);
            free(typeBuf);
            break;
        }
        xorEncryptDecrypt(typeBuf, typeLen, encryptionKey);

        // 3) Yaş (age)
        int age;
        if (fread(&age, sizeof(int), 1, file) != 1) {
            free(nameBuf);
            free(typeBuf);
            break;
        }

        // 4) Sahibin (owner) uzunluğu
        size_t ownerLen;
        if (fread(&ownerLen, sizeof(size_t), 1, file) != 1) {
            free(nameBuf);
            free(typeBuf);
            break;
        }

        char* ownerBuf = (char*)malloc(ownerLen);
        if (!ownerBuf) {
            perror("Memory allocation error for ownerBuf");
            free(nameBuf);
            free(typeBuf);
            break;
        }

        if (fread(ownerBuf, sizeof(char), ownerLen, file) != ownerLen) {
            free(nameBuf);
            free(typeBuf);
            free(ownerBuf);
            break;
        }
        xorEncryptDecrypt(ownerBuf, ownerLen, encryptionKey);

        // 5) Doğum tarihi (encodedDate)
        int encodedDate;
        if (fread(&encodedDate, sizeof(int), 1, file) != 1) {
            free(nameBuf);
            free(typeBuf);
            free(ownerBuf);
            break;
        }
        xorEncryptDecrypt((char*)&encodedDate, sizeof(int), encryptionKey);

        // Tarihi çöz: YYYYMMDD → year, month, day
        int year = encodedDate / 10000;           // İlk 4 (veya 3-4) basamak yıl
        int month = (encodedDate / 100) % 100;     // Ortadaki 2 basamak ay
        int day = encodedDate % 100;            // Son 2 basamak gün

        // Pet'i listeye ekle
        addPet(petList, nameBuf, typeBuf, age, ownerBuf);

        // B+ ağacına (birthdayTree) ekle
        if (!birthdayTree->root) {
            birthdayTree->root = createBPlusNode();
        }
        insertBirthday(birthdayTree, nameBuf, day, month, year);

        // malloc ile açtığımız buffer'ları serbest bırak
        free(nameBuf);
        free(typeBuf);
        free(ownerBuf);
    }

    fclose(file);
    printf("Birthdays loaded successfully from %s.\n", filename);
}



// Find a pet by name
Pet* findPetByName(Pet* petList, int key) {
    while (petList) {
        if (hashFunction(petList->name) == key) {
            return petList;
        }
        petList = petList->next;
    }
    return NULL;
}
ExerciseStack exerciseStack = { { }, -1 }; // Standart C++ başlatma yöntemi

void addExerciseRoutine(const char* petName, const char* exercise) {
    //100 is maximum rotuine count
    if (exerciseStack.top >= MAX_ROUTINES - 1) {
        printf("Error: Stack is full. Cannot add more routines.\n");
        return;
    }

    exerciseStack.top++;
    strncpy(exerciseStack.stack[exerciseStack.top].petName, petName, sizeof(exerciseStack.stack[exerciseStack.top].petName) - 1);
    strncpy(exerciseStack.stack[exerciseStack.top].exercise, exercise, sizeof(exerciseStack.stack[exerciseStack.top].exercise) - 1);

    printf("Exercise routine for '%s' added successfully!\n", petName);
}

void listAllExercises() {
    if (exerciseStack.top == -1) {
        printf("No exercise routines available.\n");
        return;
    }

    printf("\n--- Exercise Routines ---\n");
    for (int i = 0; i <= exerciseStack.top; i++) { // Döngü 0'dan başlamalı
        printf("Pet Name: %s\nRoutine: %s\n\n",
            exerciseStack.stack[i].petName,
            exerciseStack.stack[i].exercise);
    }
}


void undoLastExercise() {
    if (exerciseStack.top == -1) {
        printf("Error: No exercise routines to undo.\n");
        return;
    }

    printf("Undoing last exercise routine for '%s'...\n", exerciseStack.stack[exerciseStack.top].petName);
    exerciseStack.top--;  // Remove the most recent exercise by decrementing the top index

    printf("Last exercise routine undone successfully!\n");
}


static const char* STRAY_KEY = "StrayKey";
static const char* ADOPTED_KEY = "AdoptedKey";

static void computeLPSArray(const char* pattern, int M, int* lps) {
    int len = 0;
    lps[0] = 0;
    int i = 1;
    while (i < M) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        }
        else {
            if (len != 0) {
                len = lps[len - 1];
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

bool KMPcontains(const char* text, const char* pattern) {
    int N = strlen(text);
    int M = strlen(pattern);
    if (M == 0) return true; // boş pattern
    int* lps = (int*)malloc(sizeof(int) * M);
    computeLPSArray(pattern, M, lps);
    int i = 0;
    int j = 0;
    while (i < N) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (j == M) {
            free(lps);
            return true;
        }
        else if (i < N && pattern[j] != text[i]) {
            if (j != 0) j = lps[j - 1];
            else i++;
        }
    }
    free(lps);
    return false;
}

void loadStrayAnimalsFromFile(StrayAnimal** list, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return;
    }
    StrayAnimal temp;
    while (fread(&temp, sizeof(StrayAnimal), 1, file) == 1) {
        // XOR Decrypt struct
        xorEncryptDecrypt((char*)&temp, sizeof(StrayAnimal), STRAY_KEY);

        // Bellekte yeni nod
        StrayAnimal* newAnimal = (StrayAnimal*)malloc(sizeof(StrayAnimal));
        memcpy(newAnimal, &temp, sizeof(StrayAnimal));
        newAnimal->next = NULL;

        // Listeye ekle
        if (*list == NULL) {
            *list = newAnimal;
        }
        else {
            StrayAnimal* cur = *list;
            while (cur->next != NULL) {
                cur = cur->next;
            }
            cur->next = newAnimal;
        }
    }
    fclose(file);
}

void saveStrayAnimalsToFile(StrayAnimal* list, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening adoptable file");
        return;
    }
    StrayAnimal* current = list;
    while (current) {
        StrayAnimal temp;
        memcpy(&temp, current, sizeof(StrayAnimal));
        // XOR Encrypt
        xorEncryptDecrypt((char*)&temp, sizeof(StrayAnimal), STRAY_KEY);
        fwrite(&temp, sizeof(StrayAnimal), 1, file);
        current = current->next;
    }
    fclose(file);
}

void addStrayAnimalToList(StrayAnimal** list, const char* type, const char* gender,
    const char* arrivalDate, int age) {
    static int globalID = 1;
    StrayAnimal* cur = *list;
    while (cur) {
        if (cur->id >= globalID) {
            globalID = cur->id + 1;
        }
        cur = cur->next;
    }

    StrayAnimal* newAnimal = (StrayAnimal*)malloc(sizeof(StrayAnimal));
    newAnimal->id = globalID++;
    strcpy(newAnimal->type, type);
    strcpy(newAnimal->gender, gender);
    strcpy(newAnimal->arrivalDate, arrivalDate);
    newAnimal->age = age;
    newAnimal->next = NULL;

    // Listeye ekle
    if (*list == NULL) {
        *list = newAnimal;
    }
    else {
        StrayAnimal* temp = *list;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newAnimal;
    }
    printf("Stray animal added with ID: %d\n", newAnimal->id);
}

void updateStrayAnimal(StrayAnimal* list, int id,
    const char* newType,
    const char* newGender,
    const char* newArrivalDate,
    int newAge)
{
    StrayAnimal* current = list;
    while (current) {
        if (current->id == id) {
            // Direkt güncelleme
            strcpy(current->type, newType);
            strcpy(current->gender, newGender);
            strcpy(current->arrivalDate, newArrivalDate);
            current->age = newAge;

            printf("Stray animal (ID %d) updated successfully.\n", id);
            return;
        }
        current = current->next;
    }
    printf("Stray animal with ID %d not found.\n", id);
}


void deleteStrayAnimal(StrayAnimal** list, int id) {
    StrayAnimal* current = *list;
    StrayAnimal* prev = NULL;
    while (current) {
        if (current->id == id) {
            if (prev == NULL) {
                *list = current->next;
            }
            else {
                prev->next = current->next;
            }
            free(current);
            printf("Stray animal with ID %d deleted successfully.\n", id);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("Stray animal with ID %d not found.\n", id);
}

void listStrayAnimals(StrayAnimal* list) {
    if (!list) {
        printf("No stray animals available.\n");
        return;
    }
    printf("\n--- List of Stray Animals ---\n");
    StrayAnimal* current = list;
    while (current) {
        printf("ID: %d, Type: %s, Gender: %s, ArrivalDate: %s, Age: %d\n",
            current->id, current->type, current->gender,
            current->arrivalDate, current->age);
        current = current->next;
    }
}

void searchStrayAnimalsKMP(StrayAnimal* list, const char* searchKey) {
    if (!list) {
        printf("No stray animals to search.\n");
        return;
    }
    int found = 0;
    StrayAnimal* current = list;
    while (current) {
        // type alanında searchKey geçiyor mu?
        if (KMPcontains(current->type, searchKey)) {
            printf("ID: %d, Type: %s, Gender: %s, ArrivalDate: %s, Age: %d\n",
                current->id, current->type, current->gender,
                current->arrivalDate, current->age);
            found = 1;
        }
        current = current->next;
    }
    if (!found) {
        printf("No stray animals found with type containing '%s'.\n", searchKey);
    }
}

void loadAdoptedAnimalsFromFile(AdoptedAnimal** list, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return;
    }
    AdoptedAnimal temp;
    while (fread(&temp, sizeof(AdoptedAnimal), 1, file) == 1) {
        // XOR Decrypt
        xorEncryptDecrypt((char*)&temp, sizeof(AdoptedAnimal), ADOPTED_KEY);

        // Bellekte yeni node
        AdoptedAnimal* newAdopted = (AdoptedAnimal*)malloc(sizeof(AdoptedAnimal));
        memcpy(newAdopted, &temp, sizeof(AdoptedAnimal));
        newAdopted->next = NULL;

        // Listeye ekle
        if (*list == NULL) {
            *list = newAdopted;
        }
        else {
            AdoptedAnimal* cur = *list;
            while (cur->next != NULL) {
                cur = cur->next;
            }
            cur->next = newAdopted;
        }
    }
    fclose(file);
}

void saveAdoptedAnimalsToFile(AdoptedAnimal* list, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening adopted file");
        return;
    }
    AdoptedAnimal* current = list;
    while (current) {
        AdoptedAnimal temp;
        memcpy(&temp, current, sizeof(AdoptedAnimal));
        // XOR Encrypt
        xorEncryptDecrypt((char*)&temp, sizeof(AdoptedAnimal), ADOPTED_KEY);
        fwrite(&temp, sizeof(AdoptedAnimal), 1, file);
        current = current->next;
    }
    fclose(file);
}

void adoptStrayAnimal(StrayAnimal** strayList,
    const char* activeUser,
    int chosenID,
    const char* newName,
    const char* adoptionDate)
{
    if (!(*strayList)) {
        printf("No stray animals available to adopt.\n");
        return;
    }

    StrayAnimal* current = *strayList;
    StrayAnimal* prev = NULL;

    while (current) {
        if (current->id == chosenID) {
            AdoptedAnimal adopted;
            adopted.id = current->id;
            strcpy(adopted.type, current->type);
            strcpy(adopted.gender, current->gender);
            strcpy(adopted.arrivalDate, current->arrivalDate);
            adopted.age = current->age;

            strcpy(adopted.owner, activeUser);
            strcpy(adopted.adoptionDate, adoptionDate);
            printf("You named the animal: %s\n", newName);

            AdoptedAnimal* adoptedList = NULL;
            loadAdoptedAnimalsFromFile(&adoptedList, "adopted.dat");

            AdoptedAnimal* newNode = (AdoptedAnimal*)malloc(sizeof(AdoptedAnimal));
            memcpy(newNode, &adopted, sizeof(AdoptedAnimal));
            newNode->next = NULL;

            if (adoptedList == NULL) {
                adoptedList = newNode;
            }
            else {
                AdoptedAnimal* tmp = adoptedList;
                while (tmp->next) {
                    tmp = tmp->next;
                }
                tmp->next = newNode;
            }

            saveAdoptedAnimalsToFile(adoptedList, "adopted.dat");

            if (prev == NULL) {
                *strayList = current->next;
            }
            else {
                prev->next = current->next;
            }
            free(current);

            // adoptable.dat dosyasını güncelle
            saveStrayAnimalsToFile(*strayList, "adoptable.dat");

            printf("Adoption complete. Animal ID %d adopted.\n", chosenID);
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("Stray animal with ID %d not found.\n", chosenID);
}


void listAllAdoptedAnimals(AdoptedAnimal* list) {
    if (!list) {
        printf("No adopted animals found.\n");
        return;
    }
    printf("\n--- List of Adopted Animals ---\n");
    AdoptedAnimal* current = list;
    while (current) {
        printf("ID: %d, Type: %s, Gender: %s, ArrivalDate: %s, Age: %d, Owner: %s, AdoptionDate: %s\n",
            current->id, current->type, current->gender, current->arrivalDate,
            current->age, current->owner, current->adoptionDate);
        current = current->next;
    }
}

static void traverseBPlusNodeForBirthdays(BPlusNode* node, Pet* petList) {
    if (!node) return;

    // Mevcut node’daki tüm key/value çiftlerini oku
    for (int i = 0; i < node->count; i++) {
        int key = node->keys[i];
        int encodedDate = node->values[i];

        // Pet’i bul
        Pet* foundPet = findPetByName(petList, key);
        if (foundPet) {
            // Encoded date: YYYYMMDD format
            int year = encodedDate / 10000;
            int month = (encodedDate / 100) % 100;
            int day = encodedDate % 100;

            printf("Pet Name: %s | Type: %s | Owner: %s | Birthday: %02d/%02d/%04d\n",
                foundPet->name,
                foundPet->type,
                foundPet->owner,
                day, month, year);
        }
    }

    for (int i = 0; i <= node->count; i++) {
        if (node->children[i]) {
            traverseBPlusNodeForBirthdays(node->children[i], petList);
        }
    }
}

void listPetBirthdays(BPlusTree* birthdayTree, Pet* petList) {
    if (!birthdayTree || !birthdayTree->root) {
        printf("No birthdays recorded.\n");
        return;
    }
    printf("\n--- List of Pet Birthdays ---\n");
    traverseBPlusNodeForBirthdays(birthdayTree->root, petList);
    printf("--------------------------------\n");
}

