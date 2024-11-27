//#define ENABLE_petcare_TEST  // Uncomment this line to enable the petcare tests

#include "gtest/gtest.h"
#include "../../petcare/header/petcare.h"  // Adjust this include path based on your project structure

using namespace Coruh::petcare;

class petcareTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Setup test data
	}

	void TearDown() override {
		// Clean up test data
	}
};

TEST_F(petcareTest, TestAdd) {
	double result = petcare::add(5.0, 3.0);
	EXPECT_DOUBLE_EQ(result, 8.0);
}

TEST_F(petcareTest, TestSubtract) {
	double result = petcare::subtract(5.0, 3.0);
	EXPECT_DOUBLE_EQ(result, 2.0);
}

TEST_F(petcareTest, TestMultiply) {
	double result = petcare::multiply(5.0, 3.0);
	EXPECT_DOUBLE_EQ(result, 15.0);
}

TEST_F(petcareTest, TestDivide) {
	double result = petcare::divide(6.0, 3.0);
	EXPECT_DOUBLE_EQ(result, 2.0);
}

TEST_F(petcareTest, TestDivideByZero) {
	EXPECT_THROW(petcare::divide(5.0, 0.0), std::invalid_argument);
}

/**
 * @brief The main function of the test program.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 * @return int The exit status of the program.
 */
int main(int argc, char** argv) {
#ifdef ENABLE_petcare_TEST
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
#else
	return 0;
#endif
}