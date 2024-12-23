#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <conio.h>  // Windows için getch()
#else
#include <termios.h> // Linux için getch()
#include <unistd.h>  // Linux için
#endif

#include "methods.h"
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

Queue* feedingQueue = NULL;

typedef struct Menu {
    char* title;
    struct Menu* parent;
    char** items;
    int itemCount;
    struct Menu** subMenus;
} Menu;

char activeUser[50] = "";

void drawHorizontalLine(int width) {
    for (int i = 0; i < width; i++) {
        printf("*");
    }
    printf("\n");
}

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


void navigateFeedingMenu(Menu * feedingMenu, Pet * petList) {
    int selectedIndex = 0;

    while (1) {
        CLEAR_SCREEN();
        int consoleWidth = 50;
        int paddingTop = 5;
        for (int i = 0; i < paddingTop; i++) printf("\n");

        drawFrameWithContent(feedingMenu, selectedIndex, consoleWidth);

        int key = getch();
#ifdef _WIN32
        if (key == 0 || key == 224) {
            key = getch();
            if (key == 72) { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + feedingMenu->itemCount) % feedingMenu->itemCount;
            }
            else if (key == 80) { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % feedingMenu->itemCount;
            }
        }
        else if (key == 13) { // ENTER
#else
        if (key == '\033') {
            getch();
            key = getch();
            if (key == 'A') { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + feedingMenu->itemCount) % feedingMenu->itemCount;
            }
            else if (key == 'B') { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % feedingMenu->itemCount;
            }
        }
        else if (key == '\n') { // ENTER
#endif
            if (strcmp(feedingMenu->items[selectedIndex], "Add Feeding Schedule") == 0) {
                char petName[50], scheduleDetails[100];
                CLEAR_SCREEN();
                printf("Enter pet's name: ");
                scanf("%s", petName);
                printf("Enter feeding schedule details: ");
                scanf(" %[^\n]", scheduleDetails);
                enqueue(feedingQueue, petName, scheduleDetails); // Schedule ekleniyor
                printf("Feeding schedule added! Press any key to return...");
                getch();
            }
            else if (strcmp(feedingMenu->items[selectedIndex], "Update Feeding Schedule") == 0) {
                char petName[50], newDetails[100];
                CLEAR_SCREEN();
                printf("Enter pet's name to update the schedule: ");
                scanf("%s", petName);
                printf("Enter new feeding schedule details: ");
                scanf(" %[^\n]", newDetails);
                updateFeedingSchedule(feedingQueue, petName, newDetails);
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(feedingMenu->items[selectedIndex], "Delete Feeding Schedule") == 0) {
                char petName[50];
                CLEAR_SCREEN();
                printf("Enter pet's name to delete the feeding schedule: ");
                scanf("%s", petName);
                deleteFeedingSchedule(feedingQueue, petName);
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(feedingMenu->items[selectedIndex], "View Feeding Schedule List") == 0) {
                CLEAR_SCREEN();
                if (isQueueEmpty(feedingQueue)) {
                    printf("No feeding schedules available.\n");
                }
                else {
                    viewFeedingSchedules(feedingQueue); // Sıralı listeyi göster
                }
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(feedingMenu->items[selectedIndex], "Add Medicine Schedule") == 0) {
                char petName[50], scheduleDetails[100];
                CLEAR_SCREEN();
                printf("Enter pet's name: ");
                scanf("%s", petName);
                printf("Enter medicine schedule details: ");
                scanf(" %[^\n]", scheduleDetails);
                addMedicineSchedule(medicineQueue, petName, scheduleDetails);
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(feedingMenu->items[selectedIndex], "Update Medicine Schedule") == 0) {
                char petName[50], newDetails[100];
                CLEAR_SCREEN();
                printf("Enter pet's name to update the medicine schedule: ");
                scanf("%s", petName);
                printf("Enter new medicine schedule details: ");
                scanf(" %[^\n]", newDetails);
                updateMedicineSchedule(medicineQueue, petName, newDetails);
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(feedingMenu->items[selectedIndex], "Delete Medicine Schedule") == 0) {
                char petName[50];
                CLEAR_SCREEN();
                printf("Enter pet's name to delete the medicine schedule: ");
                scanf("%s", petName);
                deleteMedicineSchedule(medicineQueue, petName);
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(feedingMenu->items[selectedIndex], "View Medicine Schedule List") == 0) {
                CLEAR_SCREEN();
                viewMedicineSchedules(medicineQueue); // Medicine Schedule listesi
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(feedingMenu->items[selectedIndex], "Analyze Medicine Dependencies") == 0) {
                CLEAR_SCREEN();
                findSCC(); // Medicine Dependencies analizi
                printf("Press any key to return...");
                getch();
            }

            else if (strcmp(feedingMenu->items[selectedIndex], "Back") == 0) {
                return; // Return to the previous menu
            }
        }
        }
    }


// Global pointer for the B+ tree (for pet birthdays)
/* --------------- Adaptation (Birthday / Stray / Adoption) Menüsü --------------- */

// Global pointer for the B+ tree (for pet birthdays)
BPlusTree* birthdayTree = NULL;

// Bu menüde: Record Pet Birthday + Stray Animals + Adoption
void navigateAdaptationMenu(Menu * adaptationMenu, Pet * petList) {
    int selectedIndex = 0;

    // Stray Animals listesi (adoptable.dat)
    static StrayAnimal* strayList = NULL;
    loadStrayAnimalsFromFile(&strayList, "adoptable.dat");

    // Adopted Animals listesi
    static AdoptedAnimal* adoptedList = NULL;
    loadAdoptedAnimalsFromFile(&adoptedList, "adopted.dat");

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
            if (key == 72) {
                selectedIndex = (selectedIndex - 1 + adaptationMenu->itemCount) % adaptationMenu->itemCount;
            }
            else if (key == 80) {
                selectedIndex = (selectedIndex + 1) % adaptationMenu->itemCount;
            }
        }
        else if (key == 13) { // ENTER
#else
        if (key == '\033') {
            getch();
            key = getch();
            if (key == 'A') {
                selectedIndex = (selectedIndex - 1 + adaptationMenu->itemCount) % adaptationMenu->itemCount;
            }
            else if (key == 'B') {
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
                // Check ownership
                if (!isPetOwnedByUser(petList, petName, activeUser)) {
                    printf("Error: Pet not found or does not belong to you.\n");
                    getch();
                    continue;
                }
                printf("Enter Birthday (day month year, e.g., 15 8 2020): ");
                scanf("%d %d %d", &birthdayDay, &birthdayMonth, &birthdayYear);
                if (birthdayTree == NULL) {
                    birthdayTree = createBPlusTree();
                }
                insertBirthday(birthdayTree, petName, birthdayDay, birthdayMonth, birthdayYear);
                saveBirthdaysToFile(birthdayTree, "birthdays.data", petList);
                printf("Birthday recorded successfully! Press any key to return...");
                getch();
            }
            else if (strcmp(adaptationMenu->items[selectedIndex], "Add stray animals") == 0) {
                CLEAR_SCREEN();
                // type, gender, arrival date, age
                char type[50], gender[10], arrivalDate[20];
                int age;
                printf("Enter stray animal's type: ");
                scanf("%s", type);
                printf("Enter stray animal's gender: ");
                scanf("%s", gender);
                printf("Enter arrival date (dd/mm/yyyy): ");
                scanf("%s", arrivalDate);
                printf("Enter age: ");
                scanf("%d", &age);

                addStrayAnimalToList(&strayList, type, gender, arrivalDate, age);
                saveStrayAnimalsToFile(strayList, "adoptable.dat");
                printf("Stray animal added successfully! Press any key to continue...");
                getch();
            }
            else if (strcmp(adaptationMenu->items[selectedIndex], "Update stray animals") == 0) {
                CLEAR_SCREEN();
                listStrayAnimals(strayList); // Mevcut sokak hayvanlarını göster
                printf("Enter the ID of the stray animal to update: ");
                int id;
                scanf("%d", &id);

                // Güncellenecek değerleri menü tarafında al
                char newType[50], newGender[10], newArrivalDate[20];
                int newAge;

                printf("Enter new type: ");
                scanf("%s", newType);

                printf("Enter new gender: ");
                scanf("%s", newGender);

                printf("Enter new arrival date (dd/mm/yyyy): ");
                scanf("%s", newArrivalDate);

                printf("Enter new age: ");
                scanf("%d", &newAge);

                // Şimdi güncelleme fonksiyonunu çağır
                updateStrayAnimal(strayList, id, newType, newGender, newArrivalDate, newAge);

                // Değişiklikleri dosyaya kaydet
                saveStrayAnimalsToFile(strayList, "adoptable.dat");

                printf("Press any key to continue...");
                getch();
            }

            else if (strcmp(adaptationMenu->items[selectedIndex], "Delete stray animals") == 0) {
                CLEAR_SCREEN();
                listStrayAnimals(strayList);
                printf("Enter the ID of the stray animal to delete: ");
                int id;
                scanf("%d", &id);
                deleteStrayAnimal(&strayList, id);
                saveStrayAnimalsToFile(strayList, "adoptable.dat");
                printf("Press any key to continue...");
                getch();
            }
            else if (strcmp(adaptationMenu->items[selectedIndex], "Search stray animals") == 0) {
                CLEAR_SCREEN();
                char searchKey[50];
                printf("Enter the animal type to search for: ");
                scanf("%s", searchKey);
                searchStrayAnimalsKMP(strayList, searchKey);
                printf("Press any key to continue...");
                getch();
            }
            else if (strcmp(adaptationMenu->items[selectedIndex], "Adopt stray animals") == 0) {
                CLEAR_SCREEN();

                // Sokak hayvanlarını listeleyelim (isterseniz adoptStrayAnimal içinde de listeleyebilirsiniz,
                // ama bu kod düzeninde girişler menü tarafında yapıldığından burada göstermek mantıklı)
                listStrayAnimals(strayList);

                // Kullanıcıdan ID veya 'q' girmesini iste
                printf("Select an ID to adopt (or 'q' to quit): ");
                char choice[10];
                scanf("%s", choice);

                // Kullanıcı 'q' dediyse iptal
                if (strcmp(choice, "q") == 0) {
                    printf("Adoption cancelled.\n");
                    printf("Press any key to continue...");
                    getch(); // beklet
                    return;
                }

                // Değilse chosenID al
                int chosenID = atoi(choice);

                // Yeni isim al
                char newName[50];
                printf("Enter a name you want to give this animal: ");
                scanf("%s", newName);

                // Adoption tarihi al
                char adoptionDate[20];
                printf("Enter adoption date (dd/mm/yyyy): ");
                scanf("%s", adoptionDate);

                // Ardından adoptStrayAnimal fonksiyonunu çağır:
                adoptStrayAnimal(&strayList, activeUser, chosenID, newName, adoptionDate);

                // Güvenlik için tekrar kaydedebiliriz (gerçi adoptStrayAnimal içinde de yapıldı)
                saveStrayAnimalsToFile(strayList, "adoptable.dat");

                printf("Press any key to continue...");
                getch();
            }

            else if (strcmp(adaptationMenu->items[selectedIndex], "List all adopted animals") == 0) {
                CLEAR_SCREEN();
                // Adopted listesi read -> ekrana bas
                free(adoptedList);
                adoptedList = NULL;
                loadAdoptedAnimalsFromFile(&adoptedList, "adopted.dat");
                listAllAdoptedAnimals(adoptedList);
                printf("Press any key to continue...");
                getch();
            }
            else if (strcmp(adaptationMenu->items[selectedIndex], "List all adoptable animals") == 0) {
                CLEAR_SCREEN();
                listStrayAnimals(strayList);
                printf("Press any key to continue...");
                getch();
            }
            else if (strcmp(adaptationMenu->items[selectedIndex], "List Pet Birthdays") == 0) {
                CLEAR_SCREEN();

                // B+ ağacı yüklü değilse veya henüz yoksa yükleyelim (opsiyonel).
                if (!birthdayTree) {
                    birthdayTree = createBPlusTree();
                }
                // Tekrar dosyadan yükleyerek en güncel datayı almak isteyebiliriz:
                // (Eğer otomatik yükleniyorsa, bu adım opsiyonel olabilir.)


                listPetBirthdays(birthdayTree, petList);

                printf("Press any key to continue...");
                getch();
            }

            else if (strcmp(adaptationMenu->items[selectedIndex], "Back") == 0) {
                // Çıkarken stray ve adopted listesi kaydedilmiş olsun
                saveStrayAnimalsToFile(strayList, "adoptable.dat");
                saveAdoptedAnimalsToFile(adoptedList, "adopted.dat");
                return;
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
void navigateExerciseMenu(Menu * exerciseMenu, Pet * petList, char* activeUser) {
    int selectedIndex = 0;


    while (1) {
        CLEAR_SCREEN();
        int consoleWidth = 50;
        int paddingTop = 5;
        for (int i = 0; i < paddingTop; i++) printf("\n");

        drawFrameWithContent(exerciseMenu, selectedIndex, consoleWidth);

        int key = getch();
#ifdef _WIN32
        if (key == 0 || key == 224) {
            key = getch();
            if (key == 72) { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + exerciseMenu->itemCount) % exerciseMenu->itemCount;
            }
            else if (key == 80) { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % exerciseMenu->itemCount;
            }
        }
        else if (key == 13) { // ENTER
#else
        if (key == '\u001b') {
            getch();
            key = getch();
            if (key == 'A') { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + exerciseMenu->itemCount) % exerciseMenu->itemCount;
            }
            else if (key == 'B') { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % exerciseMenu->itemCount;
            }
        }
        else if (key == '\n') { // ENTER
#endif
            if (strcmp(exerciseMenu->items[selectedIndex], "Add Exercise Routine") == 0) {
                CLEAR_SCREEN();
                char petName[50], exercise[100];
                printf("Enter pet's name: ");
                scanf("%s", petName);

                if (!isPetOwnedByUser(petList, petName, activeUser)) {
                    printf("Error: Pet not found or does not belong to you.\n");
                    getch();
                    continue;
                }

                printf("Enter exercise routine: ");
                scanf(" %99[^\n]", exercise);

                addExerciseRoutine(petName, exercise);
                printf(" Press any key to return...");
                getch();
            }
            else if (strcmp(exerciseMenu->items[selectedIndex], "List Exercises") == 0) {
                CLEAR_SCREEN();
                listAllExercises();
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(exerciseMenu->items[selectedIndex], "Undo Last Exercises") == 0) {
                CLEAR_SCREEN();
                undoLastExercise();
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(exerciseMenu->items[selectedIndex], "Add Grooming Routine") == 0) {
                CLEAR_SCREEN();
                char petName[50], exercise[100];
                printf("Enter pet's name: ");
                scanf("%s", petName);

                if (!isPetOwnedByUser(petList, petName, activeUser)) {
                    printf("Error: Pet not found or does not belong to you.\n");
                    getch();
                    continue;
                }

                printf("Enter grooming routine: ");
                scanf(" %99[^\n]", exercise);

                // addGroomingRoutine(petName, exercise);
                printf(" Press any key to return...");
                getch();
            }
            else if (strcmp(exerciseMenu->items[selectedIndex], "List Groomings") == 0) {
                CLEAR_SCREEN();
                //  listAllGroomings();
                printf("Press any key to return...");
                getch();
            }
            else if (strcmp(exerciseMenu->items[selectedIndex], "Back") == 0) {
                return; // Return to the main menu
            }
        }
        }
    }

void aboutMenu(char text[]) {
    CLEAR_SCREEN();
    int freq[256] = { 0 };

    // Frekansları hesapla
    for (int i = 0; text[i] != '\0'; ++i)
        freq[(int)text[i]]++;

    // Karakter ve frekans dizileri oluştur
    char data[256];
    int frequencies[256], size = 0;
    for (int i = 0; i < 256; ++i) {
        if (freq[i]) {
            data[size] = (char)i;
            frequencies[size] = freq[i];
            size++;
        }
    }

    // Huffman kodları oluştur
    char codes[256][MAX_TREE_HT];
    HuffmanCodes(data, frequencies, size, codes);

    // Metni sıkıştır
    char compressed[1024];
    compress(text, codes, compressed);
    printf("\nCompressed Text: %s\n", compressed);

    // Metni çöz
    char decompressed[1024];
    MinHeapNode* root = buildHuffmanTree(data, frequencies, size);
    decompress(root, compressed, decompressed);
    printf("Decompressed Text: %s\n", decompressed);
    getch();
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
            else if (strcmp(mainMenu->items[selectedIndex], "Feeding and Medication Schedules") == 0) {
                navigateFeedingMenu(mainMenu->subMenus[2], petList);
            }
            else if (strcmp(mainMenu->items[selectedIndex], "Exercise and Grooming Menu") == 0) {

                navigateExerciseMenu(mainMenu->subMenus[3], petList, activeUser);

            }
            else if (strcmp(mainMenu->items[selectedIndex], "Pet Birthday and Adoption Anniversary") == 0) {
                navigateAdaptationMenu(mainMenu->subMenus[4], petList);
            }

            else if (strcmp(mainMenu->items[selectedIndex], "About") == 0) {
                aboutMenu("This is our about section \n Mustafa , Ali Ufuktan , Omer Faruk and me (Onur) did this project ");
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

    // Feeding queue başlatılır
    feedingQueue = createQueue();
    // Medicine Queue başlatılır
    medicineQueue = createQueue();


    int isAuthenticated = 0;
    HashTable* userTable = createHashTable();
    loadUsersFromFile(userTable, "users.dat");
    loadAppointmentsFromFile();
    // Menü elemanları
    char* authItems[] = { "Login", "Register", "Guest Mode", "Exit" };
    char* petItems[] = { "Add Pet", "Update Pet", "Delete", "List All Pets", "Search By Name or Type", "Back" };
    char* feedingItems[] = { "Add Feeding Schedule","Update Feeding Schedule","Delete Feeding Schedule", "View Feeding Schedule List","------------------------------------------","Add Medicine Schedule","Update Medicine Schedule","Delete Medicine Schedule", "View Medicine Schedule List", "Analyze Medicine Dependencies", "Back" };
    char* vetItems[] = { "Add Appointment","Update Appointment","Cancel Appointment", "View Appointments List", "Back" };
    char* exerciseItems[] = { "Add Exercise Routine","List Exercises","Undo Last Exercises","------------------------------------------","Add Grooming Routine","List Groomings", "Back" };
    char* birthdayItems[] = { "Record Pet Birthday","List Pet Birthdays","Add stray animals","Update stray animals","Delete stray animals","Search stray animals","Adopt stray animals","List all adoptable animals","List all adopted animals" ,"Back" };

    char* mainMenuItems[] = {
        "Manage Pets",
        "Veterinary Appointment Tracking",
        "Feeding and Medication Schedules",
        "Exercise and Grooming Menu",
        "Pet Birthday and Adoption Anniversary",
        "About",
        "Exit"
    };

    // Menü yapıları
    Menu authMenu = { "User Authentication", NULL, authItems, 4, NULL };
    Menu petsMenu = { "Manage Pets", NULL, petItems, 6, NULL };
    Menu feedingMenu = { "Feeding and Medication Schedules", NULL, feedingItems, 11, NULL };
    Menu vetMenu = { "Veterinary Appointment Tracking", NULL, vetItems, 5, NULL };
    Menu exerciseMenu = { "Exercise and Grooming Menu", NULL, exerciseItems, 7, NULL };
    Menu birthdayMenu = { "Pet Birthday and Adoption Anniversary", NULL, birthdayItems, 10, NULL };

    // Ana menü ve alt menüler
    Menu* mainSubMenus[] = { &petsMenu, &vetMenu, &feedingMenu, &exerciseMenu, &birthdayMenu, NULL };
    Menu mainMenu = { "Main Menu", NULL, mainMenuItems, 7, mainSubMenus };

    // Aktif kullanıcıyı takip etmek için global değişken
    extern char activeUser[50];

    // 1. User Authentication Menüsüne Git
    navigateUserAuthentication(&authMenu, userTable, &isAuthenticated);

    // 2. Kullanıcı doğrulandıysa ana menüye git
    if (isAuthenticated) {
        navigateMainMenu(&mainMenu, userTable, &isAuthenticated);
    }

    feedingQueue = createQueue(); // Feeding Queue başlatılıyor
    medicineQueue = createQueue(); // Medicine Queue başlatılıyor

    return 0;
}

