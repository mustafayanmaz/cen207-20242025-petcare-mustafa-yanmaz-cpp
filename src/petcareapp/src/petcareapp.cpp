#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <conio.h>  // Windows için getch()
#else
#include <termios.h> // Linux için getch()
#include <unistd.h>  // Linux için
#endif

#include "petcare.h" // Include UserAuth module

#ifdef _WIN32
#define CLEAR_SCREEN() system("cls")
#else
#define CLEAR_SCREEN() printf("\033[H\033[J")
#endif

#ifndef _WIN32
int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

typedef struct Menu {
    char* title;
    struct Menu* parent;
    char** items;
    int itemCount;
    struct Menu** subMenus;
} Menu;

// Aktif kullanıcıyı takip etmek için global değişken
char activeUser[50] = "";

// Yatay çizgi çizen fonksiyon
void drawHorizontalLine(int width) {
    for (int i = 0; i < width; i++) {
        printf("*");
    }
    printf("\n");
}

// Çerçeve ve içerik çizen fonksiyon
void drawFrameWithContent(Menu* menu, int selectedIndex, int width) {
    drawHorizontalLine(width);

    int padding = (width - 2 - strlen(menu->title)) / 2;
    printf("*");
    for (int i = 0; i < padding; i++) printf(" ");
    printf("%s", menu->title);
    for (int i = 0; i < width - 2 - strlen(menu->title) - padding; i++) printf(" ");
    printf("*\n");

    drawHorizontalLine(width);

    for (int i = 0; i < menu->itemCount; i++) {
        printf("* ");
        if (i == selectedIndex) {
            printf(">>  %s", menu->items[i]);
        }
        else {
            printf("   %s", menu->items[i]);
        }
        int contentWidth = width - 4 - strlen(menu->items[i]) - (i == selectedIndex ? 3 : 0);
        for (int j = 0; j < contentWidth; j++) printf(" ");
        printf("*\n");
    }

    drawHorizontalLine(width);
}

// Kimlik doğrulama menüsü
void navigateUserAuthentication(Menu* authMenu, HashTable* userTable, int* isAuthenticated) {
    int selectedIndex = 0;

    while (!*isAuthenticated) {
        CLEAR_SCREEN();
        int consoleWidth = 50;
        int paddingTop = 5;
        for (int i = 0; i < paddingTop; i++) printf("\n");

        drawFrameWithContent(authMenu, selectedIndex, consoleWidth);

        int key = getch();
#ifdef _WIN32
        if (key == 0 || key == 224) {
            key = getch();
            if (key == 72) { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + authMenu->itemCount) % authMenu->itemCount;
            }
            else if (key == 80) { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % authMenu->itemCount;
            }
        }
        else if (key == 13) { // ENTER
#else
        if (key == '\033') {
            getch();
            key = getch();
            if (key == 'A') { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + authMenu->itemCount) % authMenu->itemCount;
            }
            else if (key == 'B') { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % authMenu->itemCount;
            }
        }
        else if (key == '\n') { // ENTER
#endif
            if (strcmp(authMenu->items[selectedIndex], "Login") == 0) {
                char username[50], password[50];
                CLEAR_SCREEN();
                printf("Enter Username: ");
                scanf("%s", username);
                printf("Enter Password: ");
                scanf("%s", password);
                if (authenticateUser(userTable, username, password)) {
                    printf("Login successful! Press any key to continue...");
                    *isAuthenticated = 1;
                    strcpy(activeUser, username); // Aktif kullanıcıyı kaydet
                }
                else {
                    printf("Login failed! Invalid credentials. Press any key to return...");
                }
                getch();
            }
            else if (strcmp(authMenu->items[selectedIndex], "Register") == 0) {
                char username[50], password[50];
                CLEAR_SCREEN();
                printf("Enter Username: ");
                scanf("%s", username);
                printf("Enter Password: ");
                scanf("%s", password);
                addUser(userTable, username, password);
                printf("User registered successfully! Press any key to return...");
                getch();
            }
            else if (strcmp(authMenu->items[selectedIndex], "Guest Mode") == 0) {
                printf("Guest mode activated! Press any key to continue...");
                *isAuthenticated = 1;
                strcpy(activeUser, "Guest"); // Misafir kullanıcı
                getch();
            }
            else if (strcmp(authMenu->items[selectedIndex], "Exit") == 0) {
                CLEAR_SCREEN();
                printf("Exiting program...\n");
                saveUsersToFile(userTable, "users.dat");
                freeHashTable(userTable);
                exit(0);
            }
        }
        }
    }

// Manage Pets menüsü
void navigatePetsMenu(Menu * petsMenu, Pet * *petList, int isAuthenticated) {
    int selectedIndex = 0;

    while (1) {
        CLEAR_SCREEN();
        int consoleWidth = 50;
        int paddingTop = 5;
        for (int i = 0; i < paddingTop; i++) printf("\n");

        drawFrameWithContent(petsMenu, selectedIndex, consoleWidth);

        int key = getch();
#ifdef _WIN32
        if (key == 0 || key == 224) {
            key = getch();
            if (key == 72) { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + petsMenu->itemCount) % petsMenu->itemCount;
            }
            else if (key == 80) { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % petsMenu->itemCount;
            }
        }
        else if (key == 13) { // ENTER
#else
        if (key == '\033') {
            getch();
            key = getch();
            if (key == 'A') { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + petsMenu->itemCount) % petsMenu->itemCount;
            }
            else if (key == 'B') { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % petsMenu->itemCount;
            }
        }
        else if (key == '\n') { // ENTER
#endif
            if (strcmp(petsMenu->items[selectedIndex], "Add Pet") == 0) {
                char name[50], type[50];
                int age;
                CLEAR_SCREEN();
                printf("Enter pet's name: ");
                scanf("%s", name);
                printf("Enter pet's type: ");
                scanf("%s", type);
                printf("Enter pet's age: ");
                scanf("%d", &age);
                addPet(petList, name, type, age, activeUser);
                printf("Pet added successfully! Press any key to continue...");
                getch();
            }
            else if (strcmp(petsMenu->items[selectedIndex], "Update Pet") == 0) {
                char name[50];
                CLEAR_SCREEN();
                printf("Enter the name of the pet to update: ");
                scanf("%s", name);
                updatePet(*petList, name, activeUser);
                printf("Pet updated successfully! Press any key to continue...");
                getch();
            }
            else if (strcmp(petsMenu->items[selectedIndex], "Delete") == 0) {
                char name[50];
                CLEAR_SCREEN();
                printf("Enter the name of the pet to delete: ");
                scanf("%s", name);
                deletePet(petList, name, activeUser);
                getch();
            }
            else if (strcmp(petsMenu->items[selectedIndex], "List All Pets") == 0) {
                CLEAR_SCREEN();
                listAllPets(*petList);
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(petsMenu->items[selectedIndex], "Back") == 0) {
                return;
            }
        }
        }
    }

// Ana menü
void navigateMainMenu(Menu * mainMenu, HashTable * userTable, int* isAuthenticated) {
    int selectedIndex = 0;
    static Pet* petList = NULL;
    loadPetsFromFile(&petList, "pets.dat");

    while (1) {
        CLEAR_SCREEN();
        int consoleWidth = 50;
        int paddingTop = 5;
        for (int i = 0; i < paddingTop; i++) printf("\n");

        drawFrameWithContent(mainMenu, selectedIndex, consoleWidth);

        int key = getch();
#ifdef _WIN32
        if (key == 0 || key == 224) {
            key = getch();
            if (key == 72) { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + mainMenu->itemCount) % mainMenu->itemCount;
            }
            else if (key == 80) { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % mainMenu->itemCount;
            }
        }
        else if (key == 13) { // ENTER
#else
        if (key == '\033') {
            getch();
            key = getch();
            if (key == 'A') { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + mainMenu->itemCount) % mainMenu->itemCount;
            }
            else if (key == 'B') { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % mainMenu->itemCount;
            }
        }
        else if (key == '\n') { // ENTER
#endif
            if (strcmp(mainMenu->items[selectedIndex], "Manage Pets") == 0) {
                navigatePetsMenu(mainMenu->subMenus[0], &petList, *isAuthenticated);
            }
            //COMMENT GAY ALİ
            else if (strcmp(mainMenu->items[selectedIndex], "Exit") == 0) {
                CLEAR_SCREEN();
                printf("Exiting program...\n");
                savePetsToFile(petList, "pets.dat");
                saveUsersToFile(userTable, "users.dat");
                freePetList(petList);
                freeHashTable(userTable);
                exit(0);
            }
        }
        }
    }

// Programın ana fonksiyonu
int main() {
    int isAuthenticated = 0;
    HashTable* userTable = createHashTable();
    loadUsersFromFile(userTable, "users.dat");

    // Menü elemanları
    char* authItems[] = { "Login", "Register", "Guest Mode", "Exit" };
    char* petItems[] = { "Add Pet", "Update Pet", "Delete", "List All Pets", "Back" };
    char* feedingItems[] = { "Manage Feeding Schedule", "Manage Medication Reminders", "Back" };
    char* vetItems[] = { "Schedule Vet Appointment", "View Vet Appointments", "Back" };
    char* exerciseItems[] = { "Set Exercise Routine", "Set Grooming Schedule", "Back" };
    char* birthdayItems[] = { "Record Pet Birthday", "Record Adoption Anniversary", "Back" };
    char* mainMenuItems[] = {
        "Manage Pets",
        "Feeding and Medication Schedules",
        "Veterinary Appointment Tracking",
        "Pet Exercise and Grooming Reminders",
        "Pet Birthday and Adoption Anniversary",
        "Exit"
    };

    // Menü yapıları
    Menu authMenu = { "User Authentication", NULL, authItems, 4, NULL };
    Menu petsMenu = { "Manage Pets", NULL, petItems, 5, NULL };
    Menu feedingMenu = { "Feeding and Medication Schedules", NULL, feedingItems, 3, NULL };
    Menu vetMenu = { "Veterinary Appointment Tracking", NULL, vetItems, 3, NULL };
    Menu exerciseMenu = { "Pet Exercise and Grooming Reminders", NULL, exerciseItems, 3, NULL };
    Menu birthdayMenu = { "Pet Birthday and Adoption Anniversary", NULL, birthdayItems, 3, NULL };

    // Ana menü ve alt menüler
    Menu* mainSubMenus[] = { &petsMenu, &feedingMenu, &vetMenu, &exerciseMenu, &birthdayMenu, NULL };
    Menu mainMenu = { "Main Menu", NULL, mainMenuItems, 6, mainSubMenus };

    // Aktif kullanıcıyı takip etmek için global değişken
    extern char activeUser[50];

    // 1. User Authentication Menüsüne Git
    navigateUserAuthentication(&authMenu, userTable, &isAuthenticated);

    // 2. Kullanıcı doğrulandıysa ana menüye git
    if (isAuthenticated) {
        navigateMainMenu(&mainMenu, userTable, &isAuthenticated);
    }
    

    return 0;
}

