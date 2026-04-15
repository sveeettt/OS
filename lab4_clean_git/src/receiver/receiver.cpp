#include "receiver.h"
#include "common/message_file.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <windows.h>
#include <process.h>

#define SIGNAL_PREFIX "Local\\"

static std::vector<HANDLE> readyEvents;

Receiver* Receiver::Create(const std::string& fileName, size_t numRecords) {
    if (!MessageFile::Create(fileName, numRecords)) {
        std::cerr << "Failed to create message file" << std::endl;
        return NULL;
    }

    MessageFile* messageFile = MessageFile::Open(fileName);
    if (!messageFile) {
        std::cerr << "Failed to open message file" << std::endl;
        return NULL;
    }

    return new Receiver(fileName, messageFile);
}

Receiver::Receiver(const std::string& fileName, MessageFile* messageFile)
    : m_fileName(fileName), m_messageFile(messageFile), m_numSenders(0) {
}

Receiver::~Receiver() {
    if (m_messageFile) {
        delete m_messageFile;
    }

    for (auto& pi : m_senderProcesses) {
        if (pi.hProcess) CloseHandle(pi.hProcess);
        if (pi.hThread) CloseHandle(pi.hThread);
    }
    m_senderProcesses.clear();
}

bool Receiver::LaunchSenders(int numSenders) {
    m_numSenders = numSenders;

    ReadySignal::Create(numSenders);

    char currentExePath[MAX_PATH];
    GetModuleFileNameA(NULL, currentExePath, MAX_PATH);
    std::string exePath(currentExePath);

    for (int i = 0; i < numSenders; ++i) {
        std::string commandLine = "\"" + exePath + "\" sender \"" + m_fileName + "\" " + std::to_string(i);

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        std::cout << "Launching sender " << i << " with: " << commandLine << std::endl;

        if (CreateProcessA(NULL, (LPSTR)commandLine.c_str(), NULL, NULL,
            FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            std::cout << "Sender " << i << " launched successfully (PID: " << pi.dwProcessId << ")" << std::endl;
            m_senderProcesses.push_back(pi);
        }
        else {
            DWORD error = GetLastError();
            std::cerr << "Failed to launch sender " << i << ". Error code: " << error << std::endl;
        }
    }

    return !m_senderProcesses.empty();
}

bool Receiver::WaitForSenders() {
    std::cout << "Waiting for all senders to be ready..." << std::endl;
    ReadySignal::WaitAll();
    std::cout << "All senders are ready!" << std::endl;
    return true;
}

std::string Receiver::ReadMessage() {
    if (!m_messageFile) {
        return "";
    }

    while (m_messageFile->IsEmpty()) {
        Sleep(100);
    }

    return m_messageFile->ReadMessage();
}

void Receiver::RunCommandLoop() {
    std::string command;
    bool running = true;

    while (running) {
        std::cout << "\nCommands:\n";
        std::cout << "  read  - Read message from file\n";
        std::cout << "  exit  - Exit program\n";
        std::cout << "Enter command: ";
        std::cin >> command;

        if (command == "read") {
            std::cout << "Waiting for message..." << std::endl;
            std::string message = ReadMessage();
            if (!message.empty()) {
                std::cout << "Received message: " << message << std::endl;
            }
            else {
                std::cout << "No message received" << std::endl;
            }
        }
        else if (command == "exit") {
            running = false;
        }
        else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

    if (m_messageFile) {
        m_messageFile->Close();
    }
    ReadySignal::Cleanup();
}

void ReadySignal::Create(int numSenders) {
    for (int i = 0; i < numSenders; ++i) {
        std::string signalName = SIGNAL_PREFIX "sender_ready_" + std::to_string(i);
        HANDLE hEvent = CreateEventA(NULL, TRUE, FALSE, signalName.c_str());
        if (hEvent) {
            readyEvents.push_back(hEvent);
        }
    }
}

void ReadySignal::WaitAll() {
    if (!readyEvents.empty()) {
        WaitForMultipleObjects(static_cast<DWORD>(readyEvents.size()), &readyEvents[0], TRUE, INFINITE);
    }
}

void ReadySignal::Cleanup() {
    for (size_t i = 0; i < readyEvents.size(); ++i) {
        CloseHandle(readyEvents[i]);
    }
    readyEvents.clear();
}

void ReadySignal::Signal(int senderIndex) {
    std::string signalName = SIGNAL_PREFIX "sender_ready_" + std::to_string(senderIndex);
    HANDLE hEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, signalName.c_str());
    if (hEvent) {
        SetEvent(hEvent);
        CloseHandle(hEvent);
    }
}