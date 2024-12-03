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
            else if (strcmp(petsMenu->items[selectedIndex], "Search By Name or Type") == 0) {
                char searchKey[50];
                int searchMethod = 0;

                CLEAR_SCREEN();
                printf("Enter Search Key (Name or Type): ");
                scanf("%s", searchKey);

                CLEAR_SCREEN();
                printf("Choose search method:\n");
                printf("1. BFS (Breadth-First Search)\n");
                printf("2. DFS (Depth-First Search)\n");
                printf("Enter your choice (1 or 2): ");
                scanf("%d", &searchMethod);

                CLEAR_SCREEN();
                if (searchMethod == 1) {
                    bfsSearch(*petList, searchKey);
                }
                else if (searchMethod == 2) {
                    dfsSearch(*petList, searchKey);
                }
                else {
                    printf("Invalid choice. Returning to menu...\n");
                }

                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(petsMenu->items[selectedIndex], "Back") == 0) {
                return;
            }
        }
        }
    }

// Global pointer for the B+ tree (for pet birthdays)
BPlusTree* birthdayTree = NULL;

void navigateAdaptationMenu(Menu * adaptationMenu, Pet * petList) {
    int selectedIndex = 0;

    while (1) {
        CLEAR_SCREEN();
        int consoleWidth = 50;
        int paddingTop = 5;
        for (int i = 0; i < paddingTop; i++) printf("\n");

        drawFrameWithContent(adaptationMenu, selectedIndex, consoleWidth);

        int key = getch();
#ifdef _WIN32
        if (key == 0 || key == 224) {
            key = getch();
            if (key == 72) { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + adaptationMenu->itemCount) % adaptationMenu->itemCount;
            }
            else if (key == 80) { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % adaptationMenu->itemCount;
            }
        }
        else if (key == 13) { // ENTER
#else
        if (key == '\033') {
            getch();
            key = getch();
            if (key == 'A') { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + adaptationMenu->itemCount) % adaptationMenu->itemCount;
            }
            else if (key == 'B') { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % adaptationMenu->itemCount;
            }
        }
        else if (key == '\n') { // ENTER
#endif
            if (strcmp(adaptationMenu->items[selectedIndex], "Record Pet Birthday") == 0) {
                char petName[50];
                int birthdayDay, birthdayMonth, birthdayYear;

                CLEAR_SCREEN();
                printf("Enter pet's name: ");
                scanf("%s", petName);

                // Check if the pet exists and belongs to the active user
                if (!isPetOwnedByUser(petList, petName, activeUser)) {
                    printf("Error: Pet not found or does not belong to you.\n");
                    getch();
                    continue;
                }

                printf("Enter Birthday (day month year, e.g., 15 8 2020): ");
                scanf("%d %d %d", &birthdayDay, &birthdayMonth, &birthdayYear);

                // Insert into B+ tree
                if (birthdayTree == NULL) {
                    birthdayTree = createBPlusTree();
                }
                insertBirthday(birthdayTree, petName, birthdayDay, birthdayMonth, birthdayYear);

                // Save birthdays to file
                saveBirthdaysToFile(birthdayTree, "birthdays.data", petList);

                printf("Birthday recorded successfully! Press any key to return...");
                getch();
            }
            else if (strcmp(adaptationMenu->items[selectedIndex], "Back") == 0) {
                return; // Return to main menu
            }
        }
        }
    }




void navigateVetMenu(Menu * vetMenu, const char* activeUser, Pet * petList) {
    int selectedIndex = 0;

    while (1) {
        CLEAR_SCREEN();
        int consoleWidth = 50;
        int paddingTop = 5;
        for (int i = 0; i < paddingTop; i++) printf("\n");

        drawFrameWithContent(vetMenu, selectedIndex, consoleWidth);

        int key = getch();
#ifdef _WIN32
        if (key == 0 || key == 224) {
            key = getch();
            if (key == 72) { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + vetMenu->itemCount) % vetMenu->itemCount;
            }
            else if (key == 80) { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % vetMenu->itemCount;
            }
        }
        else if (key == 13) { // ENTER
#else
        if (key == '\033') {
            getch();
            key = getch();
            if (key == 'A') { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + vetMenu->itemCount) % vetMenu->itemCount;
            }
            else if (key == 'B') { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % vetMenu->itemCount;
            }
        }
        else if (key == '\n') { // ENTER
#endif
            if (strcmp(vetMenu->items[selectedIndex], "Add Appointment") == 0) {
                char petName[50], description[100];
                int day, month;
                CLEAR_SCREEN();
                printf("Enter pet's name: ");
                scanf("%s", petName);
                printf("Enter day and month (e.g., 15 11): ");
                scanf("%d %d", &day, &month);
                printf("Enter description: ");
                scanf(" %[^\n]", description);
                addAppointment(petName, description, day, month, activeUser, petList);
                saveAppointmentsToFile();
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(vetMenu->items[selectedIndex], "Update Appointment") == 0) {
                char petName[50], newDescription[100];
                int oldDay, oldMonth, newDay, newMonth;

                loadAppointmentsFromFile(); // Randevuları dosyadan yükle
                CLEAR_SCREEN();

                // Kullanıcıdan gerekli bilgileri al
                printf("Enter pet's name: ");
                scanf("%s", petName);

                printf("Enter current day and month (e.g., 15 11): ");
                scanf("%d %d", &oldDay, &oldMonth);

                printf("Enter new day and month (e.g., 20 11): ");
                scanf("%d %d", &newDay, &newMonth);

                printf("Enter new description: ");
                scanf(" %[^\n]", newDescription);

                // Güncellenmiş `updateAppointment` fonksiyonunu çağır
                if (updateAppointment(petName, oldDay, oldMonth, newDay, newMonth, newDescription, activeUser)) {
                    saveAppointmentsToFile(); // Güncellemeden sonra dosyaya kaydet
                }

                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(vetMenu->items[selectedIndex], "Cancel Appointment") == 0) {
                char petName[50];
                loadAppointmentsFromFile();
                int day, month;
                CLEAR_SCREEN();
                printf("Enter pet's name: ");
                scanf("%s", petName);
                printf("Enter day and month (e.g., 15 11): ");
                scanf("%d %d", &day, &month);
                cancelAppointment(petName, day, month, activeUser);
                printf("Press any key to return...");
                getch();
                saveAppointmentsToFile();
            }
            else if (strcmp(vetMenu->items[selectedIndex], "View Appointments List") == 0) {
                int month;
                loadAppointmentsFromFile();
                CLEAR_SCREEN();
                printf("Enter month to view appointments: ");
                scanf("%d", &month);
                viewAppointments(month);
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(vetMenu->items[selectedIndex], "Back") == 0) {
                return; // Return to main menu
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
            else if (strcmp(mainMenu->items[selectedIndex], "Veterinary Appointment Tracking") == 0) {
                navigateVetMenu(mainMenu->subMenus[1], activeUser, petList);
            }
            else if (strcmp(mainMenu->items[selectedIndex], "Pet Birthday and Adoption Anniversary") == 0) {
                navigateAdaptationMenu(mainMenu->subMenus[4], petList);
            }
            
            else if (strcmp(mainMenu->items[selectedIndex], "Exit") == 0) {
                CLEAR_SCREEN();
                printf("Exiting program...\n");
                savePetsToFile(petList, "pets.dat");
                saveUsersToFile(userTable, "users.dat");
                saveAppointmentsToFile();
                saveAppointmentsToFile();
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
    loadAppointmentsFromFile();
    // Menü elemanları
    char* authItems[] = { "Login", "Register", "Guest Mode", "Exit" };
    char* petItems[] = { "Add Pet", "Update Pet", "Delete", "List All Pets", "Search By Name or Type", "Back" };
    char* feedingItems[] = { "Add Feeding Schedule","Update Feeding Schedule","Cancel Feeding Schedule", "View Feeding Schedule List","------------------------------------------","Add Medicine Schedule","Update Medicine Schedule","Cancel Medicine Schedule", "View Medicine Schedule List", "Back" };
    char* vetItems[] = { "Add Appointment","Update Appointment","Cancel Appointment", "View Appointments List", "Back" };
    char* exerciseItems[] = { "Add Exercise Routine","List Exercises","------------------------------------------", "Set Grooming Schedule","Update Grooming Schedule","Delete Grooming Schedule", "View Exercise and Grooming Schedules", "Back" };
    char* birthdayItems[] = { "Record Pet Birthday", "------------------------------------------","Add stray animals","Update stray animals","Delete stray animals","Search stray animals ","Adopt stray animals" ,"Back" };

    char* mainMenuItems[] = {
        "Manage Pets",
        "Veterinary Appointment Tracking",
        "Feeding and Medication Schedules",
        "Pet Exercise and Grooming Reminders",
        "Pet Birthday and Adoption Anniversary",
        "Exit"
    };

    // Menü yapıları
    Menu authMenu = { "User Authentication", NULL, authItems, 4, NULL };
    Menu petsMenu = { "Manage Pets", NULL, petItems, 6, NULL };
    Menu feedingMenu = { "Feeding and Medication Schedules", NULL, feedingItems, 10, NULL };
    Menu vetMenu = { "Veterinary Appointment Tracking", NULL, vetItems, 5, NULL };
    Menu exerciseMenu = { "Pet Exercise and Grooming Reminders", NULL, exerciseItems, 9, NULL };
    Menu birthdayMenu = { "Pet Birthday and Adoption Anniversary", NULL, birthdayItems, 8, NULL };

    // Ana menü ve alt menüler
    Menu* mainSubMenus[] = { &petsMenu, &vetMenu, &feedingMenu, &exerciseMenu, &birthdayMenu, NULL };
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

