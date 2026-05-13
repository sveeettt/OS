#include "../include/named_pipe.h"
#include <windows.h>
#include <iostream>
#include <string>

NamedPipe::NamedPipe(const std::string& name, bool server)
    : pipe_name("\\\\.\\pipe\\" + name), hPipe(INVALID_HANDLE_VALUE), is_server(server) {
}

NamedPipe::~NamedPipe() {
    close();
}

bool NamedPipe::create() {
    if (!is_server) {
        return false;
    }

    hPipe = CreateNamedPipeA(
        pipe_name.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        1024,
        1024,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateNamedPipe failed, GLE=" << GetLastError() << std::endl;
        return false;
    }

    return true;
}

bool NamedPipe::open() {
    if (is_server) {
        return ConnectNamedPipe(hPipe, NULL) ?
            true : (GetLastError() == ERROR_PIPE_CONNECTED);
    }
    else {
        hPipe = CreateFileA(
            pipe_name.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "CreateFile failed, GLE=" << GetLastError() << std::endl;
            return false;
        }

        DWORD mode = PIPE_READMODE_MESSAGE;
        if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
            std::cerr << "SetNamedPipeHandleState failed, GLE=" << GetLastError() << std::endl;
            return false;
        }
    }

    return true;
}

void NamedPipe::close() {
    if (hPipe != INVALID_HANDLE_VALUE) {
        if (is_server) {
            DisconnectNamedPipe(hPipe);
        }
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
}

bool NamedPipe::send_message(const Message& msg) {
    if (hPipe == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytesWritten;
    BOOL success = WriteFile(
        hPipe,
        &msg,
        sizeof(Message),
        &bytesWritten,
        NULL);

    return success && (bytesWritten == sizeof(Message));
}

bool NamedPipe::receive_message(Message& msg) {
    if (hPipe == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytesRead;
    BOOL success = ReadFile(
        hPipe,
        &msg,
        sizeof(Message),
        &bytesRead,
        NULL);

    return success && (bytesRead == sizeof(Message));
}

bool NamedPipe::is_open() const {
    return hPipe != INVALID_HANDLE_VALUE;
}