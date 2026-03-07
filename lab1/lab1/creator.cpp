//Утилита Creator создает бинарный файл, имя которого и количество записей в котором
//получает через командную строку.
#include <iostream>
#include <fstream>
#include <string>
#include "Structure.h"

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "ru");

    try{
        if (argc != 3) {
            throw std::invalid_argument("Usage: Creator.exe <filename> <number of records>");
        }

        std::string filename = argv[1];
        int recordCount = std::stoi(argv[2]);

        if (recordCount <= 0) {
            throw std::invalid_argument("Number of records must be positive");
        }


        
        std::ifstream testFile(filename);
        if (testFile.good()) {
            testFile.close();
            std::cout << "File " << filename << " already exists. Overwrite? (y/n): ";
            char answer;
            std::cin >> answer;
            if (answer != 'y' && answer != 'Y') {
                std::cout << "Operation cancelled.\n";
                return 0;
            }
        }

        
        std::ofstream outFile(filename, std::ios::binary | std::ios::trunc);
        if (!outFile) {
            throw std::runtime_error("Cannot create file: " + filename);
        }


        std::cout << "Enter " << recordCount << " employees:" << "\n";

        for (int i = 0; i < recordCount; i++) {
            employee emp = {};

            std::cout << "\nEmployee " << i + 1 << "\n";

            std::cout << "ID: ";
            std::cin >> emp.num;

            std::cout << "Name (max 10 chars): ";
            std::cin >> emp.name;

            std::cout << "Hours: ";
            std::cin >> emp.hours;

            
            if (emp.num <= 0) {
                throw std::invalid_argument("ID must be positive");
            }
            if (emp.hours < 0) {
                throw std::invalid_argument("Hours cannot be negative");
            }
            if (emp.hours > 24 * 30) {
                throw std::invalid_argument("Hours exceed reasonable limit");
            }

            //в байты 
            outFile.write(reinterpret_cast<char*>(&emp), sizeof(emp));

            if (!outFile) {
                throw std::runtime_error("Error writing to file.");
            }
        }

        outFile.close();
        std::cout << "File " << filename << " created successfully! " << "\n";

        return 0;
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Input error: " << e.what() << "\n";
        return 1;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << "\n";
        return 2;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return 3;
    }
}