#include "petcare.h"

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
