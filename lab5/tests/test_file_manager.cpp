#include <gtest/gtest.h>
#include "../include/file_manager.h"
#include <vector>
#include <cstring>

class FileManagerTest : public ::testing::Test {
protected:
    std::string test_filename;
    FileManager* file_manager;

    void SetUp() override {
        test_filename = "test_employees.dat";
        file_manager = new FileManager(test_filename);

        remove(test_filename.c_str());
    }

    void TearDown() override {
        delete file_manager;
        remove(test_filename.c_str());
    }

    employee create_test_employee(int id, const char* name, double hours) {
        employee emp;
        emp.num = id;
        strncpy(emp.name, name, sizeof(emp.name) - 1);
        emp.name[sizeof(emp.name) - 1] = '\0';
        emp.hours = hours;
        return emp;
    }
};

TEST_F(FileManagerTest, CreateAndReadFile) {
    std::vector<employee> employees;
    employees.push_back(create_test_employee(1, "Alice", 40.0));
    employees.push_back(create_test_employee(2, "Bob", 35.5));
    employees.push_back(create_test_employee(3, "Charlie", 42.0));

    EXPECT_TRUE(file_manager->create_file(employees));
    EXPECT_TRUE(file_manager->file_exists());

    employee emp;
    EXPECT_TRUE(file_manager->read_employee(2, emp));
    EXPECT_EQ(emp.num, 2);
    EXPECT_STREQ(emp.name, "Bob");
    EXPECT_DOUBLE_EQ(emp.hours, 35.5);
}

TEST_F(FileManagerTest, ReadNonExistentEmployee) {
    std::vector<employee> employees;
    employees.push_back(create_test_employee(1, "Alice", 40.0));

    EXPECT_TRUE(file_manager->create_file(employees));

    employee emp;
    EXPECT_FALSE(file_manager->read_employee(999, emp));
}

TEST_F(FileManagerTest, WriteEmployee) {
    std::vector<employee> employees;
    employees.push_back(create_test_employee(1, "Alice", 40.0));
    employees.push_back(create_test_employee(2, "Bob", 35.5));

    EXPECT_TRUE(file_manager->create_file(employees));

    employee modified_emp = create_test_employee(2, "Robert", 45.0);
    EXPECT_TRUE(file_manager->write_employee(modified_emp));

    employee read_emp;
    EXPECT_TRUE(file_manager->read_employee(2, read_emp));
    EXPECT_EQ(read_emp.num, 2);
    EXPECT_STREQ(read_emp.name, "Robert");
    EXPECT_DOUBLE_EQ(read_emp.hours, 45.0);
}

TEST_F(FileManagerTest, WriteNonExistentEmployee) {
    std::vector<employee> employees;
    employees.push_back(create_test_employee(1, "Alice", 40.0));

    EXPECT_TRUE(file_manager->create_file(employees));

    employee new_emp = create_test_employee(999, "Unknown", 0.0);
    EXPECT_FALSE(file_manager->write_employee(new_emp));
}