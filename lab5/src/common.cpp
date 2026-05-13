#include "../include/common.h"
#include <sstream>
#include <iostream>
#include <cstring>

std::string employee_to_string(const employee& emp) {
    std::ostringstream oss;
    oss << "ID: " << emp.num
        << ", Name: " << emp.name
        << ", Hours: " << emp.hours;
    return oss.str();
}

void trim_string(char* str) {
    if (!str) return;

    int len = strlen(str);
    while (len > 0 && str[len - 1] == ' ') {
        str[--len] = '\0';
    }

    int start = 0;
    while (str[start] == ' ') {
        start++;
    }

    if (start > 0) {
        memmove(str, str + start, len - start + 1);
    }
}

bool is_valid_employee_id(int id) {
    return id > 0 && id <= MAX_EMPLOYEES;
}