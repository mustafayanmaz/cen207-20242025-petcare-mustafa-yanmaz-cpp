#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <conio.h>  // Windows için getch()
#else
#include <termios.h> // Linux için getch()
#include <unistd.h>  // Linux için
#endif

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
    // Çerçevenin üst kısmı
    drawHorizontalLine(width);

    // Başlık
    int padding = (width - 2 - strlen(menu->title)) / 2;
    printf("*");
    for (int i = 0; i < padding; i++) printf(" ");
    printf("%s", menu->title);
    for (int i = 0; i < width - 2 - strlen(menu->title) - padding; i++) printf(" ");
    printf("*\n");

    drawHorizontalLine(width);

    // Menü elemanları
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

    // Çerçevenin alt kısmı
    drawHorizontalLine(width);
}

void navigateMenu(Menu* currentMenu) {
    int selectedIndex = 0;

    while (1) {
        CLEAR_SCREEN();

        int consoleWidth = 50; // Sabit genişlik, terminale göre ayarlanabilir
        int paddingTop = 5;    // Ekranın ortasına yerleştirme için üst boşluk
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
                navigateMenu(currentMenu->subMenus[selectedIndex]);
            }
            else if (strcmp(currentMenu->items[selectedIndex], "Back") == 0) {
                return;
            }
            else if (strcmp(currentMenu->items[selectedIndex], "Exit") == 0) {
                CLEAR_SCREEN();
                printf("Exiting program...\n");
                exit(0);
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

    navigateMenu(&mainMenu);

    return 0;
}
