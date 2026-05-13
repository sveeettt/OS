#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "common.h"
#include <string>
#include <vector>

class FileManager {
private:
    std::string filename;

public:
    FileManager(const std::string& fname);
    ~FileManager();

    bool create_file(const std::vector<employee>& employees);
    bool read_employee(int id, employee& emp) const;
    bool write_employee(const employee& emp);
    void display_file() const;
    bool file_exists() const;

private:
    long find_employee_position(int id) const;
};

#endif // FILE_MANAGER_H