//Утилита Reporter создает текстовый файл, содержащий упорядоченный список сотрудников
//с начисленной заработной платой.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "Structure.h"

bool compareById(const employee& a, const employee& b) {
    return a.num < b.num;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "ru");

    try {
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0] << " <binary file> <report file> <hourly rate>\n";
            return 1;
        }

        std::string binFile = argv[1];
        std::string repFile = std::string(argv[2]) + ".txt";
        double rate = std::stod(argv[3]);

        if (rate <= 0) {
            throw std::runtime_error("Hourly rate must be positive");
        }

        
        std::ifstream inFile(binFile, std::ios::binary);
        if (!inFile) {
            throw std::runtime_error("Cannot open file: " + binFile);
        }

        // Чтение всех записей
        std::vector<employee> employees;
        employee emp;

        while (inFile.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
            employees.push_back(emp);
        }
        inFile.close();

        if (employees.empty()) {
            throw std::runtime_error("File is empty");
        }

        
        std::sort(employees.begin(), employees.end(), compareById);

        
        std::ofstream outFile(repFile);
        if (!outFile) {
            throw std::runtime_error("Cannot create file: " + repFile);
        }

        
        outFile << "Отчет по файлу \"" << binFile << "\n";
        outFile << "Номер сотрудника, Имя сотрудника, Часы, Зарплата" << "\n";

        std::cout << "\nОтчет по файлу \"" << binFile << "\n";;
        std::cout << "Номер сотрудника, Имя сотрудника, Часы, Зарплата" << "\n";

        
        for (const auto& e : employees) {
            double salary = e.hours * rate;

            outFile << e.num << ", " << e.name << ", " << e.hours << ", " << salary << "\n";
            std::cout << e.num << ", " << e.name << ", " << e.hours << ", " << salary << "\n";
        }

        outFile.close();
        std::cout << "\nReport saved to " << repFile << "\n";

        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}