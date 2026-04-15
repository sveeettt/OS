#include "message_file.h"
#include <iostream>
#include <cstring>
#include <windows.h>

bool MessageFile::Create(const std::string& filename, size_t numRecords) {
    std::ofstream file(filename.c_str(), std::ios::binary | std::ios::out);
    if (!file) {
        std::cerr << "Failed to create file: " << filename << std::endl;
        return false;
    }

    FileHeader header;
    header.numRecords = numRecords;
    header.readIndex = 0;
    header.writeIndex = 0;
    header.count = 0;

    file.write(reinterpret_cast<const char*>(&header), sizeof(FileHeader));

    Message emptyMessage;
    for (size_t i = 0; i < numRecords; ++i) {
        file.write(reinterpret_cast<const char*>(&emptyMessage), sizeof(Message));
    }

    file.close();
    return true;
}

MessageFile* MessageFile::Open(const std::string& filename) {
    MessageFile* messageFile = new MessageFile(filename);
    if (!messageFile->Initialize()) {
        delete messageFile;
        return NULL;
    }
    return messageFile;
}

MessageFile::MessageFile(const std::string& filename) : m_filename(filename) {
    m_hFile = INVALID_HANDLE_VALUE;
    m_hMutex = NULL;
}

MessageFile::~MessageFile() {
    Close();
}

bool MessageFile::Initialize() {
    m_file.open(m_filename.c_str(), std::ios::binary | std::ios::in | std::ios::out);
    if (!m_file) {
        std::cerr << "Failed to open file: " << m_filename << std::endl;
        return false;
    }

    if (!ReadHeader()) {
        std::cerr << "Failed to read header" << std::endl;
        return false;
    }

    std::string mutexName = "MessageFile_" + m_filename;
    for (size_t i = 0; i < mutexName.length(); ++i) {
        if (mutexName[i] == '\\' || mutexName[i] == '/' || mutexName[i] == ':') {
            mutexName[i] = '_';
        }
    }
    m_hMutex = CreateMutexA(NULL, FALSE, mutexName.c_str());
    if (!m_hMutex) {
        std::cerr << "Failed to create mutex" << std::endl;
        return false;
    }

    return true;
}

bool MessageFile::ReadHeader() {
    m_file.seekg(0, std::ios::beg);
    m_file.read(reinterpret_cast<char*>(&m_header), sizeof(FileHeader));
    return m_file.good();
}

bool MessageFile::WriteHeader() {
    m_file.seekp(0, std::ios::beg);
    m_file.write(reinterpret_cast<const char*>(&m_header), sizeof(FileHeader));
    m_file.flush();
    return m_file.good();
}

bool MessageFile::LockFile() {
    DWORD result = WaitForSingleObject(m_hMutex, INFINITE);
    return result == WAIT_OBJECT_0;
}

void MessageFile::UnlockFile() {
    ReleaseMutex(m_hMutex);
}

bool MessageFile::WriteMessage(const std::string& message) {
    if (!m_file) return false;

    LockFile();
    ReadHeader();

    while (m_header.count >= m_header.numRecords) {
        UnlockFile();
        Sleep(100);
        LockFile();
        ReadHeader();
    }

    Message newMessage;
    newMessage.occupied = true;
    strncpy(newMessage.text, message.c_str(), MAX_MESSAGE_LENGTH - 1);
    newMessage.text[MAX_MESSAGE_LENGTH - 1] = '\0';

    size_t filePos = sizeof(FileHeader) + m_header.writeIndex * sizeof(Message);
    m_file.seekp(filePos, std::ios::beg);
    m_file.write(reinterpret_cast<const char*>(&newMessage), sizeof(Message));

    m_header.writeIndex = (m_header.writeIndex + 1) % m_header.numRecords;
    m_header.count++;
    WriteHeader();

    UnlockFile();
    return true;
}

std::string MessageFile::ReadMessage() {
    if (!m_file) return "";

    LockFile();
    ReadHeader();

    while (m_header.count == 0) {
        UnlockFile();
        Sleep(100);
        LockFile();
        ReadHeader();
    }

    Message message;
    size_t filePos = sizeof(FileHeader) + m_header.readIndex * sizeof(Message);
    m_file.seekg(filePos, std::ios::beg);
    m_file.read(reinterpret_cast<char*>(&message), sizeof(Message));

    Message emptyMessage;
    m_file.seekp(filePos, std::ios::beg);
    m_file.write(reinterpret_cast<const char*>(&emptyMessage), sizeof(Message));

    m_header.readIndex = (m_header.readIndex + 1) % m_header.numRecords;
    m_header.count--;
    WriteHeader();

    std::string result = message.text;
    UnlockFile();
    return result;
}

bool MessageFile::IsFull() {
    if (!LockFile()) return false;
    ReadHeader();
    bool full = (m_header.count >= m_header.numRecords);
    UnlockFile();
    return full;
}

bool MessageFile::IsEmpty() {
    if (!LockFile()) return true;
    ReadHeader();
    bool empty = (m_header.count == 0);
    UnlockFile();
    return empty;
}

void MessageFile::Close() {
    if (m_file.is_open()) {
        m_file.close();
    }

    if (m_hMutex) {
        CloseHandle(m_hMutex);
        m_hMutex = NULL;
    }
}