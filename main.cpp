#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "Structure.h"


void showBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file " << filename << "\n";
        return;
    }

    std::cout << "\n Contents of " << filename << "\n";

    employee emp;
    int count = 1;

    while (file.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        std::cout << count++ << ": ID=" << emp.num
            << ", Name=" << emp.name
            << ", Hours=" << emp.hours << "\n";
    }

   
    file.close();
}


void showReport(const std::string& filename) {
    std::string reportFile = filename + ".txt";
    std::ifstream file(reportFile);
    if (!file) {
        std::cerr << "Cannot open file " << reportFile << "\n";
        return;
    }

    std::cout << "\n REPORT " << "\n";;
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << "\n";
    }
    
    file.close();
}

int main() {
    setlocale(LC_ALL, "ru");

    try {
        std::string binFile;
        int records;
        std::string repFile;
        double rate;

        // 1: Ввод имени бин файла
        std::cout << "Enter binary file name: ";
        std::cin >> binFile;

        std::cout << "Enter number of records: ";
        std::cin >> records;

        // 2-3: Запуск Creator
        std::cout << "\nStarting Creator..." << "\n"; ;

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        std::string cmd = "Creator.exe " + binFile + " " + std::to_string(records);

        if (!CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
            CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            throw std::runtime_error("Failed to start Creator");
        }

        WaitForSingleObject(pi.hProcess, INFINITE);//ожидание завершения cREATOR
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        std::cout << "Creator finished.\n";

        // 4: Вывод бинарного файла
        showBinaryFile(binFile);

        // 5: Ввод данных для отчета
        std::cout << "\nEnter report file name (without .txt): ";
        std::cin >> repFile;

        std::cout << "Enter hourly rate: ";
        std::cin >> rate;

        // 6-7: Запуск Reporter
        std::cout << "\nStarting Reporter" << "\n";;

        cmd = "Reporter.exe " + binFile + " " + repFile + " " + std::to_string(rate);

        if (!CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
            CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            throw std::runtime_error("Failed to start Reporter");
        }

        WaitForSingleObject(pi.hProcess, INFINITE);//завершение Reporter
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        std::cout << "Reporter finished.\n";

        // 8: Вывод отчета
        showReport(repFile);

        // 9: Завершение
        std::cout << "\nProgram finished.\n";

        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}