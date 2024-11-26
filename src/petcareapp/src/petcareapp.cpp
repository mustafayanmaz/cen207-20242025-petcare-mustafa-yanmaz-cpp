#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp için
#ifdef _WIN32
#include <conio.h>  // Windows için getch()
#else
#include <termios.h> // Linux için getch()
#include <unistd.h>  // Linux için
#endif

// Ok tuşu kodları
#ifdef _WIN32
#define UP_ARROW 72
#define DOWN_ARROW 80
#define ENTER_KEY 13
#else
#define UP_ARROW 'A'
#define DOWN_ARROW 'B'
#define ENTER_KEY 10
#endif
#define BACK_KEY 27 // ESC tuşu

#ifdef _WIN32
#define CLEAR_SCREEN() system("cls")
#else
#define CLEAR_SCREEN() printf("\033[H\033[J")
#endif

// Linux için getch() fonksiyonu
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
    char* title;          // Menü başlığı
    struct Menu* parent;  // Üst menüye dönüş
    char** items;         // Menü elemanları
    int itemCount;        // Eleman sayısı
    struct Menu** subMenus; // Alt menüler
} Menu;

// Menüde gezinme fonksiyonu
void navigateMenu(Menu* currentMenu) {
    int selectedIndex = 0;

    while (1) {
        CLEAR_SCREEN(); // Terminali temizle
        printf("Menu: %s\n", currentMenu->title);
        printf("Use UP/DOWN to navigate, ENTER to select, ESC/Back to go back.\n\n");

        // Mevcut menü elemanlarını listele
        for (int i = 0; i < currentMenu->itemCount; i++) {
            if (i == selectedIndex) {
                printf(">> %s\n", currentMenu->items[i]); // Seçili eleman
            }
            else {
                printf("   %s\n", currentMenu->items[i]); // Diğer elemanlar
            }
        }

        // Kullanıcıdan tuş al
        int key = getch();
#ifdef _WIN32
        if (key == 0 || key == 224) {
            key = getch();
            if (key == UP_ARROW) {
                selectedIndex = (selectedIndex - 1 + currentMenu->itemCount) % currentMenu->itemCount;
            }
            else if (key == DOWN_ARROW) {
                selectedIndex = (selectedIndex + 1) % currentMenu->itemCount;
            }
        }
        else if (key == ENTER_KEY) {
#else
        if (key == '\033') { // Linux için ok tuşları
            getch();         // '['
            key = getch();   // Kod
            if (key == UP_ARROW) {
                selectedIndex = (selectedIndex - 1 + currentMenu->itemCount) % currentMenu->itemCount;
            }
            else if (key == DOWN_ARROW) {
                selectedIndex = (selectedIndex + 1) % currentMenu->itemCount;
            }
        }
        else if (key == ENTER_KEY) {
#endif
            // Alt menüye geçiş veya işlem
            if (currentMenu->subMenus && selectedIndex < currentMenu->itemCount - 1 && currentMenu->subMenus[selectedIndex]) {
                navigateMenu(currentMenu->subMenus[selectedIndex]);
            }
            else if (strcmp(currentMenu->items[selectedIndex], "Back") == 0) {
                return; // Üst menüye dön
            }
            else if (strcmp(currentMenu->items[selectedIndex], "Exit") == 0) {
                CLEAR_SCREEN();
                printf("Exiting program...\n");
                exit(0); // Programdan çık
            }
            else {
                // Alt menü yok, işlem yapılabilir
                CLEAR_SCREEN();
                printf("You selected: %s\n", currentMenu->items[selectedIndex]);
                printf("\nPress any key to return...");
                getch();
            }
        }
        else if (key == BACK_KEY) {
            if (currentMenu->parent) {
                return; // Üst menüye dön
            }
        }
        }
    }

int main() {
    // Alt menü elemanları
    char* authItems[] = { "Login", "Register", "Guest Mode", "Back" };
    char* feedingItems[] = { "Manage Feeding Schedule", "Manage Medication Reminders", "Back" };
    char* vetItems[] = { "Schedule Vet Appointment", "View Vet Appointments", "Back" };
    char* exerciseItems[] = { "Set Exercise Routine", "Set Grooming Schedule", "Back" };
    char* birthdayItems[] = { "Record Pet Birthday", "Record Adoption Anniversary", "Back" };

    // Ana menü elemanları
    char* mainMenuItems[] = {
        "User Authentication",
        "Feeding and Medication Schedules",
        "Veterinary Appointment Tracking",
        "Pet Exercise and Grooming Reminders",
        "Pet Birthday and Adoption Anniversary",
        "Exit"
    };

    // Menü yapısı
    Menu authMenu = { "User Authentication", NULL, authItems, 4, NULL };
    Menu feedingMenu = { "Feeding and Medication Schedules", NULL, feedingItems, 3, NULL };
    Menu vetMenu = { "Veterinary Appointment Tracking", NULL, vetItems, 3, NULL };
    Menu exerciseMenu = { "Pet Exercise and Grooming Reminders", NULL, exerciseItems, 3, NULL };
    Menu birthdayMenu = { "Pet Birthday and Adoption Anniversary", NULL, birthdayItems, 3, NULL };

    // Ana menüye alt menüleri bağla
    Menu* mainSubMenus[] = { &authMenu, &feedingMenu, &vetMenu, &exerciseMenu, &birthdayMenu, NULL };
    Menu mainMenu = { "Main Menu", NULL, mainMenuItems, 6, mainSubMenus };

    // Alt menülere ana menüyü bağla
    authMenu.parent = &mainMenu;
    feedingMenu.parent = &mainMenu;
    vetMenu.parent = &mainMenu;
    exerciseMenu.parent = &mainMenu;
    birthdayMenu.parent = &mainMenu;

    // Menü gezintisini başlat
    navigateMenu(&mainMenu);

    return 0;
}
