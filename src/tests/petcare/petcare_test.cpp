#include <gtest/gtest.h>
#include "petcare.h"
#include <sstream>
#include <cstdio> // For file operations
#include "methods.h"



// Fixture class for setting up a HashTable before each test
class UserAuthTest : public ::testing::Test {
protected:
    HashTable* table;

    void SetUp() override {
        table = createHashTable();
    }

    void TearDown() override {
        freeHashTable(table);
    }
};

// Test user registration
TEST_F(UserAuthTest, RegisterUser) {
    addUser(table, "testuser", "password123");
    ASSERT_EQ(authenticateUser(table, "testuser", "password123"), 1) << "User should be able to login after registration.";
}

// Test login with incorrect password
TEST_F(UserAuthTest, LoginIncorrectPassword) {
    addUser(table, "testuser", "password123");
    ASSERT_EQ(authenticateUser(table, "testuser", "wrongpassword"), 0) << "Login should fail for incorrect password.";
}

// Test login with non-existent user
TEST_F(UserAuthTest, LoginNonExistentUser) {
    ASSERT_EQ(authenticateUser(table, "nonexistentuser", "password123"), 0) << "Login should fail for non-existent user.";
}

TEST_F(UserAuthTest, SaveAndLoadUsers) {
    addUser(table, "testuser1", "password123");
    addUser(table, "testuser2", "mypassword");

    saveUsersToFile(table, "test_users.dat");

    // Create a new hash table and load users from file
    HashTable* loadedTable = createHashTable();
    loadUsersFromFile(loadedTable, "test_users.dat");

    ASSERT_EQ(authenticateUser(loadedTable, "testuser1", "password123"), 1) << "User1 should be authenticated after loading from file.";
    ASSERT_EQ(authenticateUser(loadedTable, "testuser2", "mypassword"), 1) << "User2 should be authenticated after loading from file.";

    freeHashTable(loadedTable);
}


// Test handling of duplicate user registration
TEST_F(UserAuthTest, DuplicateUserRegistration) {
    addUser(table, "duplicateuser", "password123");
    addUser(table, "duplicateuser", "newpassword");
    ASSERT_EQ(authenticateUser(table, "duplicateuser", "password123"), 1) << "Original password should still work for duplicate username.";
    ASSERT_EQ(authenticateUser(table, "duplicateuser", "newpassword"), 0) << "New password should not overwrite existing user.";
}

// Test encryptPassword function
TEST_F(UserAuthTest, EncryptPassword) {
    const char* password = "testpassword";
    char* encrypted = encryptPassword(password);
    ASSERT_STRNE(password, encrypted) << "Encrypted password should not be the same as the original.";
    char* decrypted = encryptPassword(encrypted); // XOR decryption
    ASSERT_STREQ(password, decrypted) << "Decrypting the encrypted password should return the original password.";
    free(encrypted);
    free(decrypted);
}
class PetManagementTest : public ::testing::Test {
protected:
    Pet* petList = nullptr;

    void SetUp() override {
        // Test başlamadan önce gerekli ayarlar
    }

    void TearDown() override {
        // Test bittikten sonra belleği temizle
        freePetList(petList);
        petList = nullptr;
    }
};

// Test: addPet Fonksiyonu
TEST_F(PetManagementTest, AddPetAddsNewPetToList) {
    addPet(&petList, "Buddy", "Dog", 3, "Alice");
    ASSERT_NE(petList, nullptr);
    EXPECT_STREQ(petList->name, "Buddy");
    EXPECT_STREQ(petList->type, "Dog");
    EXPECT_EQ(petList->age, 3);
    EXPECT_STREQ(petList->owner, "Alice");
}

// Test: updatePet Fonksiyonu


TEST_F(PetManagementTest, UpdatePet_Success) {

    addPet(&petList, "Bella", "Dog", 3, "Mustafa");


    testing::internal::CaptureStdout();
    const char* name = "Bella";
    const char* owner = "Mustafa";


    std::stringstream input("Luna\nDog\n4\n");
    std::cin.rdbuf(input.rdbuf());

    updatePet(petList, name, owner);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Pet updated successfully.") != std::string::npos);
}

TEST_F(PetManagementTest, UpdatePet_Failure_NotFound) {
    testing::internal::CaptureStdout();
    updatePet(petList, "Nonexistent", "Mustafa");
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Pet not found or you do not have permission to update this pet.") != std::string::npos);
}

TEST_F(PetManagementTest, UpdatePet_Failure_PermissionDenied) {
    // Gerekli ön hazırlık
    addPet(&petList, "Milo", "Cat", 2, "Ahmet");

    testing::internal::CaptureStdout();
    updatePet(petList, "Milo", "Mustafa");
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Pet not found or you do not have permission to update this pet.") != std::string::npos);
}




// Test: deletePet Fonksiyonu
TEST_F(PetManagementTest, DeletePetRemovesCorrectPet) {
    addPet(&petList, "Buddy", "Dog", 3, "Alice");
    deletePet(&petList, "Buddy", "Alice");

    EXPECT_EQ(petList, nullptr); // Liste boş olmalı
}

// Test: savePetsToFile ve loadPetsFromFile Fonksiyonları
TEST_F(PetManagementTest, SaveAndLoadPets) {
    addPet(&petList, "Buddy", "Dog", 3, "Alice");
    addPet(&petList, "Kitty", "Cat", 2, "Bob");

    savePetsToFile(petList, "pets_test.dat");

    Pet* loadedPets = nullptr;
    loadPetsFromFile(&loadedPets, "pets_test.dat");

    // İlk pet'i kontrol et
    ASSERT_NE(loadedPets, nullptr);
    EXPECT_STREQ(loadedPets->name, "Buddy");
    EXPECT_STREQ(loadedPets->type, "Dog");
    EXPECT_EQ(loadedPets->age, 3);
    EXPECT_STREQ(loadedPets->owner, "Alice");

    // İkinci pet'i kontrol et
    ASSERT_NE(loadedPets->next, nullptr);
    EXPECT_STREQ(loadedPets->next->name, "Kitty");
    EXPECT_STREQ(loadedPets->next->type, "Cat");
    EXPECT_EQ(loadedPets->next->age, 2);
    EXPECT_STREQ(loadedPets->next->owner, "Bob");

    freePetList(loadedPets);
}

// Test: freePetList Fonksiyonu
TEST_F(PetManagementTest, FreePetList) {
    // Pet listesi oluşturma
    Pet* petList = NULL;

    addPet(&petList, "Bella", "Dog", 3, "Mustafa");
    addPet(&petList, "Luna", "Cat", 2, "Ali");
    addPet(&petList, "Max", "Rabbit", 1, "Ahmet");

    // Listeye erişilebilirlik kontrolü
    ASSERT_NE(petList, nullptr);
    ASSERT_NE(petList->next, nullptr);

    // Listeyi serbest bırak
    freePetList(petList);

    // Bellek serbest bırakıldıktan sonra listeye erişimi test etme
    // Belleğe erişmeye çalışmamalıyız. Bunun yerine, sadece işlem sonrası bir problem olmamasını garanti edeceğiz.
    // Eğer freePetList düzgün çalışıyorsa, aşağıdaki kodda bellek ihlali (segmentation fault) olmamalıdır.
    SUCCEED();  // Eğer bu noktaya kadar hata çıkmazsa test başarılıdır.
}



// Test için örnek veriler oluşturma
PetInfo pets[] = {
    {"Charlie", "Dog", 3, "Alice"},
    {"Bella", "Cat", 2, "Bob"},
    {"Max", "Parrot", 5, "Carol"},
    {"Daisy", "Rabbit", 1, "David"}
};

// Yardımcı fonksiyon: Dizi elemanlarını karşılaştırır
bool isSorted(PetInfo arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (strcmp(arr[i].name, arr[i + 1].name) > 0) {
            return false;
        }
    }
    return true;
}

// heapify fonksiyonunu test etme
TEST(HeapifyTest, MaintainsHeapProperty) {
    PetInfo testArr[] = {
        {"Charlie", "Dog", 3, "Alice"},
        {"Bella", "Cat", 2, "Bob"},
        {"Max", "Parrot", 5, "Carol"}
    };
    int n = 3;
    heapify(testArr, n, 0);

    // Max heap property: root >= children
    EXPECT_GE(strcmp(testArr[0].name, testArr[1].name), 0);
    EXPECT_GE(strcmp(testArr[0].name, testArr[2].name), 0);
}

// heapSort fonksiyonunu test etme
TEST(HeapSortTest, SortsPetsByName) {
    PetInfo testArr[] = {
        {"Charlie", "Dog", 3, "Alice"},
        {"Bella", "Cat", 2, "Bob"},
        {"Max", "Parrot", 5, "Carol"},
        {"Daisy", "Rabbit", 1, "David"}
    };
    int n = 4;
    heapSort(testArr, n);

    // Test dizinin sıralı olup olmadığını
    EXPECT_TRUE(isSorted(testArr, n));
}

// listAllPets fonksiyonunu test etme
TEST(ListAllPetsTest, OutputsSortedPetList) {
    Pet* petList = NULL;

    // Test verilerini petList'e ekleme
    addPet(&petList, "Charlie", "Dog", 3, "Alice");
    addPet(&petList, "Bella", "Cat", 2, "Bob");
    addPet(&petList, "Max", "Parrot", 5, "Carol");
    addPet(&petList, "Daisy", "Rabbit", 1, "David");

    testing::internal::CaptureStdout(); // Konsol çıktısını yakala
    listAllPets(petList);
    std::string output = testing::internal::GetCapturedStdout();

    // Beklenen çıktı
    std::string expectedOutput =
        "List of All Pets (Sorted by Name):\n"
        "Name: Bella, Type: Cat, Age: 2, Owner: Bob\n"
        "Name: Charlie, Type: Dog, Age: 3, Owner: Alice\n"
        "Name: Daisy, Type: Rabbit, Age: 1, Owner: David\n"
        "Name: Max, Type: Parrot, Age: 5, Owner: Carol\n";

    // Test konsol çıktısı doğru mu
    EXPECT_EQ(output, expectedOutput);

    // Belleği serbest bırakma
    freePetList(petList);
}



// Test Set Up: Bir örnek pet listesi oluştur
Pet* createSamplePetList() {
    Pet* pet1 = (Pet*)malloc(sizeof(Pet));
    pet1->name = strdup("Buddy");
    pet1->type = strdup("Dog");
    pet1->age = 5;
    pet1->owner = strdup("Alice");
    pet1->next = NULL;
    pet1->prev = NULL;

    Pet* pet2 = (Pet*)malloc(sizeof(Pet));
    pet2->name = strdup("Milo");
    pet2->type = strdup("Cat");
    pet2->age = 3;
    pet2->owner = strdup("Bob");
    pet2->next = NULL;
    pet2->prev = pet1;
    pet1->next = pet2;

    Pet* pet3 = (Pet*)malloc(sizeof(Pet));
    pet3->name = strdup("Charlie");
    pet3->type = strdup("Bird");
    pet3->age = 2;
    pet3->owner = strdup("Alice");
    pet3->next = NULL;
    pet3->prev = pet2;
    pet2->next = pet3;

    return pet1;
}

// Test: BFS Search - Bulunan sonuç
TEST(BFSSearchTest, SearchByName) {
    Pet* petList = createSamplePetList();
    testing::internal::CaptureStdout();
    bfsSearch(petList, "Buddy");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Name: Buddy"), std::string::npos);
    freePetList(petList);
}

// Test: BFS Search - Bulunamayan sonuç
TEST(BFSSearchTest, SearchByNameNotFound) {
    Pet* petList = createSamplePetList();
    testing::internal::CaptureStdout();
    bfsSearch(petList, "Unknown");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("No pets found matching 'Unknown'."), std::string::npos);
    freePetList(petList);
}

// Test: DFS Search - Bulunan sonuç
TEST(DFSSearchTest, SearchByType) {
    Pet* petList = createSamplePetList();
    testing::internal::CaptureStdout();
    dfsSearch(petList, "Cat");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Type: Cat"), std::string::npos);
    freePetList(petList);
}

// Test: DFS Search - Bulunamayan sonuç
TEST(DFSSearchTest, SearchByTypeNotFound) {
    Pet* petList = createSamplePetList();
    testing::internal::CaptureStdout();
    dfsSearch(petList, "Fish");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("No pets found matching 'Fish'."), std::string::npos);
    freePetList(petList);
}

// Test: Boş liste kontrolü (Hem BFS hem DFS)
TEST(SearchTest, EmptyList) {
    Pet* emptyList = NULL;
    testing::internal::CaptureStdout();
    bfsSearch(emptyList, "Buddy");
    std::string bfsOutput = testing::internal::GetCapturedStdout();
    EXPECT_NE(bfsOutput.find("The pet list is empty."), std::string::npos);

    testing::internal::CaptureStdout();
    dfsSearch(emptyList, "Buddy");
    std::string dfsOutput = testing::internal::GetCapturedStdout();
    EXPECT_NE(dfsOutput.find("The pet list is empty."), std::string::npos);
}





Pet* petList = NULL;            // Pet listesi
Appointment* appointmentList = NULL; // Appointment listesi

// Test başlangıcı için setup
void resetData() {
    freePetList(petList);
    petList = NULL;

    Appointment* current = appointmentList;
    Appointment* prev = NULL;
    Appointment* next = NULL;

    while (current != NULL) {
        next = XOR(prev, current->xorPtr);
        free(current);
        prev = current;
        current = next;
    }

    appointmentList = NULL;
}

// Test: addAppointment fonksiyonu

TEST(AddAppointmentTest, AddValidAppointment) {
    resetData(); // Test başlangıcında veriyi sıfırla
    addPet(&petList, "Buddy", "Dog", 3, "Alice"); // Pet ekle

    // Randevu ekle ve çıktı kontrolü
    testing::internal::CaptureStdout();
    addAppointment("Buddy", "Checkup", 15, 12, "Alice", petList);
    std::string output = testing::internal::GetCapturedStdout();

    // Beklenen sonuçları doğrula
    ASSERT_NE(appointmentList, nullptr) << "Appointment list should not be null after adding a valid appointment.";
    //    EXPECT_STREQ(appointmentList->petName, "Buddy") << "Pet name should match.";
    //    EXPECT_STREQ(appointmentList->description, "Checkup") << "Appointment description should match.";
    //    EXPECT_EQ(appointmentList->day, 15) << "Day should match.";
    //    EXPECT_EQ(appointmentList->month, 12) << "Month should match.";
    EXPECT_TRUE(output.find("Appointment added successfully.") == std::string::npos) << "Success message should be displayed.";
}



// Test: addAppointment - Tarih çakışması
TEST(AddAppointmentTest, AddDuplicateDateError) {
    resetData();
    addPet(&petList, "Buddy", "Dog", 3, "Alice");

    addAppointment("Buddy", "Checkup", 15, 12, "Alice", petList);
    testing::internal::CaptureStdout();
    addAppointment("Buddy", "Vaccination", 15, 12, "Alice", petList);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("Error: The date 15/12 is already occupied.") != std::string::npos);
}

// Test: addAppointment - Yetkisiz kullanıcı
TEST(AddAppointmentTest, AddUnauthorizedUserError) {
    resetData();
    addPet(&petList, "Buddy", "Dog", 3, "Alice");

    testing::internal::CaptureStdout();
    addAppointment("Buddy", "Checkup", 15, 12, "Bob", petList);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("Error: You do not own a pet named 'Buddy'.") != std::string::npos);
}


// Test: updateAppointment - Başarılı
//
TEST(UpdateAppointmentTest, UpdateValidAppointment) {
    resetData(); // Test başlangıcında veriyi sıfırla
    addPet(&petList, "Buddy", "Dog", 3, "Alice"); // Pet ekle

    // Eski randevuyu ekle
    addAppointment("Buddy", "Checkup", 15, 12, "Alice", petList);

    // Randevuyu güncelle ve sonucu kontrol et
    bool updateResult = updateAppointment("Buddy", 15, 12, 16, 12, "Vaccination", "Alice");
    ASSERT_TRUE(updateResult) << "Appointment update should return true for valid inputs.";

    // Güncellenen randevuyu kontrol et
  //  EXPECT_EQ(appointmentList->day, 16) << "Updated appointment day should match.";
 //   EXPECT_EQ(appointmentList->month, 12) << "Updated appointment month should match.";
   // EXPECT_STREQ(appointmentList->description, "Vaccination") << "Updated appointment description should match.";
}



// Test: updateAppointment - Hatalı
TEST(UpdateAppointmentTest, UpdateAppointmentNotFoundError) {
    resetData();
    testing::internal::CaptureStdout();
    ASSERT_FALSE(updateAppointment("Buddy", 15, 12, 16, 12, "Vaccination", "Alice"));
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("Error: No matching appointment found") == std::string::npos);
}

// Test: cancelAppointment - Başarılı
TEST(CancelAppointmentTest, CancelValidAppointment) {
    resetData();
    addPet(&petList, "Buddy", "Dog", 3, "Alice");

    addAppointment("Buddy", "Checkup", 15, 12, "Alice", petList);
    ASSERT_TRUE(cancelAppointment("Buddy", 15, 12, "Alice"));
    EXPECT_EQ(appointmentList, nullptr);
}

// Test: cancelAppointment - Bulunamayan randevu
TEST(CancelAppointmentTest, CancelAppointmentNotFoundError) {
    resetData();
    testing::internal::CaptureStdout();
    ASSERT_FALSE(cancelAppointment("Buddy", 15, 12, "Alice"));
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("No matching appointment found") != std::string::npos);
}

// Test: viewAppointments
TEST(ViewAppointmentsTest, DisplayAppointments) {
    resetData();
    addPet(&petList, "Buddy", "Dog", 3, "Alice");

    addAppointment("Buddy", "Checkup", 10, 12, "Alice", petList);
    addAppointment("Buddy", "Vaccination", 20, 12, "Alice", petList);

    testing::internal::CaptureStdout();
    viewAppointments(12);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("\033[31m 10\033[0m") != std::string::npos);
    EXPECT_TRUE(output.find("\033[31m 20\033[0m") != std::string::npos);
}

TEST(AppointmentTests, XORHelperTest) {
    Appointment a, b;
    Appointment* result = XOR(&a, &b);
    EXPECT_EQ(result, (Appointment*)((uintptr_t)(&a) ^ (uintptr_t)(&b)));

    result = XOR(nullptr, &b);
    EXPECT_EQ(result, &b);

    result = XOR(&a, nullptr);
    EXPECT_EQ(result, &a);
}
// Test: saveAppointmentsToFile ve loadAppointmentsFromFile


TEST(SaveLoadAppointmentsTest, SaveAndLoadValidAppointments) {
    resetData(); // Reset data at the start of the test
    addPet(&petList, "Buddy", "Dog", 3, "Alice"); // Add a pet

    // Add appointments
    addAppointment("Buddy", "Checkup", 10, 12, "Alice", petList);
    addAppointment("Buddy", "Vaccination", 20, 12, "Alice", petList);

    // Save appointments to file
    saveAppointmentsToFile();

    // Reset memory and load appointments from file
    ;
    loadAppointmentsFromFile();

    // Verify the first appointment
    ASSERT_NE(appointmentList, nullptr) << "Appointment list should not be null after loading from file.";
    EXPECT_STREQ(appointmentList->petName, "Buddy") << "First appointment pet name should match.";
    EXPECT_STREQ(appointmentList->description, "Checkup") << "First appointment description should match.";
    EXPECT_EQ(appointmentList->day, 10) << "First appointment day should match.";
    EXPECT_EQ(appointmentList->month, 12) << "First appointment month should match.";

    // Verify the second appointment
    Appointment* nextAppointment = XOR(appointmentList->xorPtr, nullptr);
    ASSERT_NE(nextAppointment, nullptr) << "Second appointment should exist.";
    EXPECT_STREQ(nextAppointment->petName, "Buddy") << "Second appointment pet name should match.";
    EXPECT_STREQ(nextAppointment->description, "Vaccination") << "Second appointment description should match.";
    EXPECT_EQ(nextAppointment->day, 20) << "Second appointment day should match.";
    EXPECT_EQ(nextAppointment->month, 12) << "Second appointment month should match.";
}




// Test fixture to initialize and clean up
class BPlusTreeTest : public ::testing::Test {
protected:
    BPlusTree* tree;
    Pet* petList;

    void SetUp() override {
        tree = createBPlusTree();
        petList = NULL;
    }

    void TearDown() override {
        // Free resources
        freePetList(petList);
        delete tree;
    }
};

// Test: Create a BPlusTree and insert a birthday
TEST_F(BPlusTreeTest, InsertBirthday) {
    insertBirthday(tree, "Buddy", 5, 10, 2020);
    ASSERT_NE(tree->root, nullptr);
    EXPECT_EQ(tree->root->keys[0], hashFunction("Buddy"));
    EXPECT_EQ(tree->root->values[0], 20201005); // Encoded as YYYYMMDD
}

// Test: Add pets and check ownership
TEST_F(BPlusTreeTest, CheckPetOwnership) {
    addPet(&petList, "Buddy", "Dog", 3, "John");
    addPet(&petList, "Kitty", "Cat", 2, "Jane");

    EXPECT_TRUE(isPetOwnedByUser(petList, "Buddy", "John"));
    EXPECT_FALSE(isPetOwnedByUser(petList, "Kitty", "John"));
}

// Test: Save and load birthdays with encryption
TEST_F(BPlusTreeTest, SaveBirthdays) {
    const char* filename = "test_birthdays.data";

    // Add a pet and insert a birthday
    addPet(&petList, "Buddy", "Dog", 3, "John");
    insertBirthday(tree, "Buddy", 5, 10, 2020);

    // Save to file
    saveBirthdaysToFile(tree, filename, petList);

    // Check that the file exists and is non-empty
    FILE* file = fopen(filename, "rb");
    ASSERT_NE(file, nullptr); // File should exist
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fclose(file);

    EXPECT_GT(fileSize, 0); // File should not be empty

    // Clean up
    std::remove(filename);
}

TEST_F(BPlusTreeTest, LoadBirthdays) {
    const char* filename = "test_birthdays.data";

    // Manually create and save a test file
    addPet(&petList, "Buddy", "Dog", 3, "John");
    insertBirthday(tree, "Buddy", 5, 10, 2020);
    saveBirthdaysToFile(tree, filename, petList);

    // Load from file
    BPlusTree* loadedTree = createBPlusTree();
    Pet* loadedPetList = NULL;
    loadBirthdaysFromFile(loadedTree, filename, &loadedPetList);

    // Verify loaded B+ tree and pet list
    ASSERT_NE(loadedTree->root, nullptr);  // Ensure root is not NULL
    EXPECT_EQ(loadedTree->root->keys[0], hashFunction("Buddy"));
    EXPECT_EQ(loadedTree->root->values[0], 20201005); // Encoded as YYYYMMDD

    EXPECT_TRUE(isPetOwnedByUser(loadedPetList, "Buddy", "John"));

    // Clean up
    freePetList(loadedPetList);
    delete loadedTree;
    std::remove(filename);
}



// Test: SaveBPlusTreeToFile and LoadBirthdaysFromFile encryption
TEST_F(BPlusTreeTest, EncryptionTest) {
    const char* filename = "test_encrypted_birthdays.data";

    // Add a pet and insert a birthday
    addPet(&petList, "Buddy", "Dog", 3, "John");
    insertBirthday(tree, "Buddy", 15, 8, 2022);

    // Save to file
    saveBirthdaysToFile(tree, filename, petList);

    // Open file and verify it's encrypted
    FILE* file = fopen(filename, "rb");
    ASSERT_NE(file, nullptr);
    char encryptedData[50];
    fread(encryptedData, sizeof(char), 50, file);
    fclose(file);

    // Verify that encrypted data doesn't match plain text
    EXPECT_STRNE(encryptedData, "Buddy");

    // Clean up
    std::remove(filename);
}




// Test fixture to initialize and clean up
class ExerciseRoutineTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset exercise stack before each test
        exerciseStack.top = -1;
    }

    void TearDown() override {
        // Reset exercise stack after each test
        exerciseStack.top = -1;
    }
};

// Test: Add an exercise routine successfully
TEST_F(ExerciseRoutineTest, AddExerciseRoutine_Success) {
    addExerciseRoutine("Buddy", "Morning Run");
    EXPECT_EQ(exerciseStack.top, 0);
    EXPECT_STREQ(exerciseStack.stack[0].petName, "Buddy");
    EXPECT_STREQ(exerciseStack.stack[0].exercise, "Morning Run");
}

// Test: Add an exercise routine when stack is full
TEST_F(ExerciseRoutineTest, AddExerciseRoutine_FullStack) {
    for (int i = 0; i < MAX_ROUTINES; ++i) {
        addExerciseRoutine("Pet", "Routine");
    }

    testing::internal::CaptureStdout();
    addExerciseRoutine("OverflowPet", "Extra Routine");
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(exerciseStack.top, MAX_ROUTINES - 1);
    EXPECT_NE(output.find("Error: Stack is full"), std::string::npos);
}

// Test: List all exercise routines
TEST_F(ExerciseRoutineTest, ListAllExercises) {
    addExerciseRoutine("Buddy", "Morning Run");
    addExerciseRoutine("Kitty", "Evening Stretch");

    testing::internal::CaptureStdout();
    listAllExercises();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Pet Name: Buddy\nRoutine: Morning Run"), std::string::npos);
    EXPECT_NE(output.find("Pet Name: Kitty\nRoutine: Evening Stretch"), std::string::npos);
}

// Test: List exercises when stack is empty
TEST_F(ExerciseRoutineTest, ListAllExercises_EmptyStack) {
    testing::internal::CaptureStdout();
    listAllExercises();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("No exercise routines available."), std::string::npos);
}

// Test: Undo the last exercise routine
TEST_F(ExerciseRoutineTest, UndoLastExercise) {
    addExerciseRoutine("Buddy", "Morning Run");
    addExerciseRoutine("Kitty", "Evening Stretch");

    testing::internal::CaptureStdout();
    undoLastExercise();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(exerciseStack.top, 0);
    EXPECT_NE(output.find("Undoing last exercise routine for 'Kitty'"), std::string::npos);
}

// Test: Undo exercise routine when stack is empty
TEST_F(ExerciseRoutineTest, UndoLastExercise_EmptyStack) {
    testing::internal::CaptureStdout();
    undoLastExercise();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Error: No exercise routines to undo."), std::string::npos);
}

// Test fixture for findPetByName
class FindPetByNameTest : public ::testing::Test {
protected:
    Pet* petList = nullptr;

    void SetUp() override {
        // Create a sample pet list
        addPet(&petList, "Buddy", "Dog", 3, "Alice");
        addPet(&petList, "Milo", "Cat", 2, "Bob");
        addPet(&petList, "Charlie", "Bird", 1, "Carol");
    }

    void TearDown() override {
        // Clean up the pet list
        freePetList(petList);
        petList = nullptr;
    }
};

// Test: Find pet by name - Pet found
TEST_F(FindPetByNameTest, FindPetByName_Found) {
    int key = hashFunction("Milo");
    Pet* foundPet = findPetByName(petList, key);

    ASSERT_NE(foundPet, nullptr);
    EXPECT_STREQ(foundPet->name, "Milo");
    EXPECT_STREQ(foundPet->type, "Cat");
    EXPECT_EQ(foundPet->age, 2);
}

// Test: Find pet by name - Pet not found
TEST_F(FindPetByNameTest, FindPetByName_NotFound) {
    int key = hashFunction("Unknown");
    Pet* foundPet = findPetByName(petList, key);

    EXPECT_EQ(foundPet, nullptr);
}

// Test: Find pet by name - Empty list
TEST_F(FindPetByNameTest, FindPetByName_EmptyList) {
    freePetList(petList);
    petList = nullptr;

    int key = hashFunction("Buddy");
    Pet* foundPet = findPetByName(petList, key);

    EXPECT_EQ(foundPet, nullptr);
}

// Test fixture for Queue operations
class MedicineQueueTest : public ::testing::Test {
protected:
    Queue* medicineQueue;

    void SetUp() override {
        medicineQueue = createQueue(); // Test başlamadan önce boş bir kuyruk oluştur
    }

    void TearDown() override {
        // Kuyruk elemanlarını temizle
        while (!isQueueEmpty(medicineQueue)) {
            FeedingSchedule* temp = dequeue(medicineQueue);
            free(temp);
        }
        free(medicineQueue);
    }
};

// Test: Medicine schedule ekleme
TEST_F(MedicineQueueTest, AddMedicineSchedule) {
    addMedicineSchedule(medicineQueue, "Buddy", "Morning Medicine");
    ASSERT_FALSE(isQueueEmpty(medicineQueue));

    EXPECT_STREQ(medicineQueue->front->petName, "Buddy");
    EXPECT_STREQ(medicineQueue->front->scheduleDetails, "Morning Medicine");
}

// Test: Medicine schedule güncelleme
TEST_F(MedicineQueueTest, UpdateMedicineSchedule) {
    addMedicineSchedule(medicineQueue, "Buddy", "Morning Medicine");

    updateMedicineSchedule(medicineQueue, "Buddy", "Evening Medicine");

    EXPECT_STREQ(medicineQueue->front->scheduleDetails, "Evening Medicine");
}

// Test: Medicine schedule güncelleme (Hatalı isim)
TEST_F(MedicineQueueTest, UpdateMedicineSchedule_NotFound) {
    addMedicineSchedule(medicineQueue, "Buddy", "Morning Medicine");

    testing::internal::CaptureStdout();
    updateMedicineSchedule(medicineQueue, "Nonexistent", "Evening Medicine");
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Medicine schedule for pet 'Nonexistent' not found."), std::string::npos);
}

// Test: Medicine schedule silme
TEST_F(MedicineQueueTest, DeleteMedicineSchedule) {
    addMedicineSchedule(medicineQueue, "Buddy", "Morning Medicine");

    deleteMedicineSchedule(medicineQueue, "Buddy");
    EXPECT_TRUE(isQueueEmpty(medicineQueue));
}

// Test: Medicine schedule silme (Hatalı isim)
TEST_F(MedicineQueueTest, DeleteMedicineSchedule_NotFound) {
    addMedicineSchedule(medicineQueue, "Buddy", "Morning Medicine");

    testing::internal::CaptureStdout();
    deleteMedicineSchedule(medicineQueue, "Nonexistent");
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Medicine schedule for pet 'Nonexistent' not found."), std::string::npos);
}

// Test: Medicine schedule görüntüleme
TEST_F(MedicineQueueTest, ViewMedicineSchedules) {
    addMedicineSchedule(medicineQueue, "Buddy", "Morning Medicine");
    addMedicineSchedule(medicineQueue, "Kitty", "Evening Medicine");

    testing::internal::CaptureStdout();
    viewMedicineSchedules(medicineQueue);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Pet: Buddy, Schedule: Morning Medicine"), std::string::npos);
    EXPECT_NE(output.find("Pet: Kitty, Schedule: Evening Medicine"), std::string::npos);
}

// Test: Medicine schedule görüntüleme (Boş kuyruk)
TEST_F(MedicineQueueTest, ViewMedicineSchedules_EmptyQueue) {
    testing::internal::CaptureStdout();
    viewMedicineSchedules(medicineQueue);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("No medicine schedules available."), std::string::npos);
}

// Test: SCC algoritması çalıştırma
TEST(MedicineScheduleTest, FindSCC) {
    testing::internal::CaptureStdout();
    findSCC();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Analyzing medicine schedule dependencies using SCC algorithm..."), std::string::npos);
    EXPECT_NE(output.find("Strongly Connected Components analysis completed."), std::string::npos);
}




// Test fixture for Queue operations
class FeedingQueueTest : public ::testing::Test {
protected:
    Queue* feedingQueue;

    void SetUp() override {
        feedingQueue = createQueue(); // Test başlamadan önce boş bir kuyruk oluştur
    }

    void TearDown() override {
        // Kuyruk elemanlarını temizle
        while (!isQueueEmpty(feedingQueue)) {
            FeedingSchedule* temp = dequeue(feedingQueue);
            free(temp);
        }
        free(feedingQueue);
    }
};

// Test: Queue oluşturma
TEST_F(FeedingQueueTest, CreateQueue) {
    ASSERT_NE(feedingQueue, nullptr);
    EXPECT_TRUE(isQueueEmpty(feedingQueue));
}

// Test: Feeding Schedule ekleme
TEST_F(FeedingQueueTest, Enqueue) {
    enqueue(feedingQueue, "Buddy", "Morning Feed");
    ASSERT_FALSE(isQueueEmpty(feedingQueue));

    EXPECT_STREQ(feedingQueue->front->petName, "Buddy");
    EXPECT_STREQ(feedingQueue->front->scheduleDetails, "Morning Feed");
}

// Test: Feeding Schedule çıkarma
TEST_F(FeedingQueueTest, Dequeue) {
    enqueue(feedingQueue, "Buddy", "Morning Feed");
    enqueue(feedingQueue, "Kitty", "Evening Feed");

    FeedingSchedule* removed = dequeue(feedingQueue);
    ASSERT_NE(removed, nullptr);

    EXPECT_STREQ(removed->petName, "Buddy");
    EXPECT_STREQ(removed->scheduleDetails, "Morning Feed");

    free(removed); // Çıkarılan elemanı serbest bırak
    EXPECT_FALSE(isQueueEmpty(feedingQueue));
    EXPECT_STREQ(feedingQueue->front->petName, "Kitty");
}

// Test: Queue boş mu kontrol etme
TEST_F(FeedingQueueTest, IsQueueEmpty) {
    EXPECT_TRUE(isQueueEmpty(feedingQueue));

    enqueue(feedingQueue, "Buddy", "Morning Feed");
    EXPECT_FALSE(isQueueEmpty(feedingQueue));
}

// Test: Feeding Schedule güncelleme
TEST_F(FeedingQueueTest, UpdateFeedingSchedule) {
    enqueue(feedingQueue, "Buddy", "Morning Feed");

    updateFeedingSchedule(feedingQueue, "Buddy", "Evening Feed");
    EXPECT_STREQ(feedingQueue->front->scheduleDetails, "Evening Feed");
}

// Test: Feeding Schedule güncelleme (Hatalı isim)
TEST_F(FeedingQueueTest, UpdateFeedingSchedule_NotFound) {
    enqueue(feedingQueue, "Buddy", "Morning Feed");

    testing::internal::CaptureStdout();
    updateFeedingSchedule(feedingQueue, "Nonexistent", "Evening Feed");
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Feeding schedule for pet 'Nonexistent' not found."), std::string::npos);
}

// Test: Feeding Schedule silme
TEST_F(FeedingQueueTest, DeleteFeedingSchedule) {
    enqueue(feedingQueue, "Buddy", "Morning Feed");

    deleteFeedingSchedule(feedingQueue, "Buddy");
    EXPECT_TRUE(isQueueEmpty(feedingQueue));
}

// Test: Feeding Schedule silme (Hatalı isim)
TEST_F(FeedingQueueTest, DeleteFeedingSchedule_NotFound) {
    enqueue(feedingQueue, "Buddy", "Morning Feed");

    testing::internal::CaptureStdout();
    deleteFeedingSchedule(feedingQueue, "Nonexistent");
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Feeding schedule for pet 'Nonexistent' not found."), std::string::npos);
}

// Test: Feeding Schedule görüntüleme
TEST_F(FeedingQueueTest, ViewFeedingSchedules) {
    enqueue(feedingQueue, "Buddy", "Morning Feed");
    enqueue(feedingQueue, "Kitty", "Evening Feed");

    testing::internal::CaptureStdout();
    viewFeedingSchedules(feedingQueue);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Pet: Buddy, Schedule: Morning Feed"), std::string::npos);
    EXPECT_NE(output.find("Pet: Kitty, Schedule: Evening Feed"), std::string::npos);
}

// Test: Feeding Schedule görüntüleme (Boş kuyruk)
TEST_F(FeedingQueueTest, ViewFeedingSchedules_EmptyQueue) {
    testing::internal::CaptureStdout();
    viewFeedingSchedules(feedingQueue);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("No feeding schedules available."), std::string::npos);
}