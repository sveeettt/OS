#include <gtest/gtest.h>
#include "../include/common.h"
#include <cstring>

class CommonTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(CommonTest, EmployeeToString) {
    employee emp;
    emp.num = 123;
    strcpy(emp.name, "John");
    emp.hours = 40.5;

    std::string result = employee_to_string(emp);

    EXPECT_NE(result.find("123"), std::string::npos);
    EXPECT_NE(result.find("John"), std::string::npos);
    EXPECT_NE(result.find("40.5"), std::string::npos);
}

TEST_F(CommonTest, TrimString) {
    char test1[] = "  hello  ";
    trim_string(test1);
    EXPECT_STREQ(test1, "hello");

    char test2[] = "world";
    trim_string(test2);
    EXPECT_STREQ(test2, "world");

    char test3[] = "   ";
    trim_string(test3);
    EXPECT_STREQ(test3, "");

    char test4[] = "";
    trim_string(test4);
    EXPECT_STREQ(test4, "");
}

TEST_F(CommonTest, IsValidEmployeeId) {
    EXPECT_TRUE(is_valid_employee_id(1));
    EXPECT_TRUE(is_valid_employee_id(500));
    EXPECT_TRUE(is_valid_employee_id(MAX_EMPLOYEES));

    EXPECT_FALSE(is_valid_employee_id(0));
    EXPECT_FALSE(is_valid_employee_id(-1));
    EXPECT_FALSE(is_valid_employee_id(MAX_EMPLOYEES + 1));
}