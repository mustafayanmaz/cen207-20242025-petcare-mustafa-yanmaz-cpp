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

void navigateMenu(Menu* currentMenu, HashTable* userTable) {
    int selectedIndex = 0;

    while (1) {
        CLEAR_SCREEN();

        int consoleWidth = 50;
        int paddingTop = 5;
        for (int i = 0; i < paddingTop; i++) printf("\n");

        drawFrameWithContent(currentMenu, selectedIndex, consoleWidth);

        int key = getch();
#ifdef _WIN32
        if (key == 0 || key == 224) {
            key = getch();
            if (key == 72) { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + currentMenu->itemCount) % currentMenu->itemCount;
            }
            else if (key == 80) { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % currentMenu->itemCount;
            }
        }
        else if (key == 13) { // ENTER
#else
        if (key == '\033') {
            getch();
            key = getch();
            if (key == 'A') { // UP_ARROW
                selectedIndex = (selectedIndex - 1 + currentMenu->itemCount) % currentMenu->itemCount;
            }
            else if (key == 'B') { // DOWN_ARROW
                selectedIndex = (selectedIndex + 1) % currentMenu->itemCount;
            }
        }
        else if (key == '\n') { // ENTER
#endif
            if (currentMenu->subMenus && selectedIndex < currentMenu->itemCount - 1 && currentMenu->subMenus[selectedIndex]) {
                navigateMenu(currentMenu->subMenus[selectedIndex], userTable);
            }
            else if (strcmp(currentMenu->items[selectedIndex], "Back") == 0) {
                return;
            }
            else if (strcmp(currentMenu->items[selectedIndex], "Exit") == 0) {
                CLEAR_SCREEN();
                printf("Exiting program...\n");
                saveUsersToFile(userTable, "users.dat");
                freeHashTable(userTable);
                exit(0);
            }
            else if (strcmp(currentMenu->items[selectedIndex], "Register") == 0) {
                char username[50], password[50];
                CLEAR_SCREEN();
                printf("Enter Username: ");
                scanf("%s", username);
                printf("Enter Password: ");
                scanf("%s", password);
                addUser(userTable, username, password);
               
                printf("User registered successfully!\nPress any key to return...");
                getch();
            }
            else if (strcmp(currentMenu->items[selectedIndex], "Login") == 0) {
                char username[50], password[50];
                CLEAR_SCREEN();
                printf("Enter Username: ");
                scanf("%s", username);
                printf("Enter Password: ");
                scanf("%s", password);
                if (authenticateUser(userTable, username, password)) {
                    printf("Login successful!\nPress any key to continue...");
                }
                else {
                    printf("Login failed! Invalid credentials.\nPress any key to return...");
                }
                getch();
            }
            else {
                CLEAR_SCREEN();
                printf("You selected: %s\n", currentMenu->items[selectedIndex]);
                printf("\nPress any key to return...");
                getch();
            }
        }
        }
    }

int main() {
    HashTable* userTable = createHashTable();
    loadUsersFromFile(userTable, "users.dat");

    char* authItems[] = { "Login", "Register", "Guest Mode", "Back" };
    char* feedingItems[] = { "Manage Feeding Schedule", "Manage Medication Reminders", "Back" };
    char* vetItems[] = { "Schedule Vet Appointment", "View Vet Appointments", "Back" };
    char* exerciseItems[] = { "Set Exercise Routine", "Set Grooming Schedule", "Back" };
    char* birthdayItems[] = { "Record Pet Birthday", "Record Adoption Anniversary", "Back" };

    char* mainMenuItems[] = {
        "User Authentication",
        "Feeding and Medication Schedules",
        "Veterinary Appointment Tracking",
        "Pet Exercise and Grooming Reminders",
        "Pet Birthday and Adoption Anniversary",
        "Exit"
    };

    Menu authMenu = { "User Authentication", NULL, authItems, 4, NULL };
    Menu feedingMenu = { "Feeding and Medication Schedules", NULL, feedingItems, 3, NULL };
    Menu vetMenu = { "Veterinary Appointment Tracking", NULL, vetItems, 3, NULL };
    Menu exerciseMenu = { "Pet Exercise and Grooming Reminders", NULL, exerciseItems, 3, NULL };
    Menu birthdayMenu = { "Pet Birthday and Adoption Anniversary", NULL, birthdayItems, 3, NULL };

    Menu* mainSubMenus[] = { &authMenu, &feedingMenu, &vetMenu, &exerciseMenu, &birthdayMenu, NULL };
    Menu mainMenu = { "Main Menu", NULL, mainMenuItems, 6, mainSubMenus };

    authMenu.parent = &mainMenu;
    feedingMenu.parent = &mainMenu;
    vetMenu.parent = &mainMenu;
    exerciseMenu.parent = &mainMenu;
    birthdayMenu.parent = &mainMenu;

    navigateMenu(&mainMenu, userTable);

    return 0;
}
