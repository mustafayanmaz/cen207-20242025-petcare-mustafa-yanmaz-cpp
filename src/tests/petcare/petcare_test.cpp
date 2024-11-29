#include <gtest/gtest.h>
#include "petcare.h"
#include <sstream>

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


/*TEST_F(PetManagementTest, UpdatePet_Success) {
    // Gerekli ön hazırlık
    addPet(&petList, "Bella", "Dog", 3, "Mustafa");

    // Kullanıcı girişini simüle et
    testing::internal::CaptureStdout();
    const char* name = "Bella";
    const char* owner = "Mustafa";

    // Yeni veri simülasyonu
    std::stringstream input("Luna\nDog\n4\n");
    std::cin.rdbuf(input.rdbuf()); // std::cin yönlendirme

    updatePet(petList, name, owner);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Pet updated successfully.") != std::string::npos);
}
bu amk testi coverage çıkmasını engelliyo ama test başarıyla geçiyo test explorarda*/

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
    // Eğer `freePetList` düzgün çalışıyorsa, aşağıdaki kodda bellek ihlali (segmentation fault) olmamalıdır.
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

