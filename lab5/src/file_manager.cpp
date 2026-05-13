#include "../include/file_manager.h"
#include <fstream>
#include <iostream>
#include <cstring>

FileManager::FileManager(const std::string& fname) : filename(fname) {
}

FileManager::~FileManager() {
}

bool FileManager::create_file(const std::vector<employee>& employees) {
    std::ofstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create file " << filename << std::endl;
        return false;
    }

    for (size_t i = 0; i < employees.size(); ++i) {
        file.write(reinterpret_cast<const char*>(&employees[i]), sizeof(employee));
    }

    file.close();
    return true;
}

bool FileManager::read_employee(int id, employee& emp) const {
    long pos = find_employee_position(id);
    if (pos == -1) {
        return false;
    }

    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.seekg(pos);
    file.read(reinterpret_cast<char*>(&emp), sizeof(employee));

    bool success = file.good() && emp.num == id;
    file.close();
    return success;
}

bool FileManager::write_employee(const employee& emp) {
    long pos = find_employee_position(emp.num);
    if (pos == -1) {
        return false;
    }

    std::fstream file(filename.c_str(), std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        return false;
    }

    file.seekp(pos);
    file.write(reinterpret_cast<const char*>(&emp), sizeof(employee));

    bool success = file.good();
    file.close();
    return success;
}

void FileManager::display_file() const {
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    employee emp;
    std::cout << "\n=== Employee Database ===" << std::endl;
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        std::cout << employee_to_string(emp) << std::endl;
    }
    std::cout << "=========================" << std::endl;

    file.close();
}

bool FileManager::file_exists() const {
    std::ifstream file(filename.c_str());
    return file.good();
}

long FileManager::find_employee_position(int id) const {
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open()) {
        return -1;
    }

    employee emp;
    long pos = 0;

    while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        if (emp.num == id) {
            file.close();
            return pos;
        }
        pos += sizeof(employee);
    }

    file.close();
    return -1;
}