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


//XOR
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

    printf("Saving users to file '%s'\n", filename);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        User* current = table->buckets[i];
        while (current) {
            // Kullanıcı adı ve şifreyi şifrele
            char* encryptedUsername = encryptPassword(current->username);

            size_t usernameLen = strlen(encryptedUsername) + 1;
            size_t passwordLen = strlen(current->encryptedPassword) + 1;

            fwrite(&usernameLen, sizeof(size_t), 1, file);
            fwrite(encryptedUsername, sizeof(char), usernameLen, file);

            fwrite(&passwordLen, sizeof(size_t), 1, file);
            fwrite(current->encryptedPassword, sizeof(char), passwordLen, file);

            // Debug mesajı:
            printf("DEBUG: Saving - Encrypted Username: '%s', EncryptedPassword: '%s'\n",
                encryptedUsername, current->encryptedPassword);

            free(encryptedUsername); // Şifrelenen kullanıcı adını serbest bırak
            current = current->next;
        }
    }

    fclose(file);
    printf("Users saved successfully.\n");
}



void loadUsersFromFile(HashTable* table, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    printf("Loading users from file '%s'\n", filename);
    while (1) {
        size_t usernameLen, passwordLen;

        if (fread(&usernameLen, sizeof(size_t), 1, file) != 1) break;

        char* encryptedUsername = (char*)malloc(usernameLen);
        fread(encryptedUsername, sizeof(char), usernameLen, file);

        fread(&passwordLen, sizeof(size_t), 1, file);
        char* encryptedPassword = (char*)malloc(passwordLen);
        fread(encryptedPassword, sizeof(char), passwordLen, file);

        // Kullanıcı adını çöz
        char* decryptedUsername = encryptPassword(encryptedUsername);

       /* // Debug mesajı:
        printf("DEBUG: Loading - Decrypted Username: '%s', EncryptedPassword: '%s'\n",
            decryptedUsername, encryptedPassword);*/

        // Yeni kullanıcı ekleme
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
    printf("Users loaded successfully.\n");
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
