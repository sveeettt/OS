#ifndef NAMED_PIPE_H
#define NAMED_PIPE_H

#include "protocol.h"
#include <string>
#include <windows.h>  

class NamedPipe {
private:
    std::string pipe_name;
    HANDLE hPipe;
    bool is_server;

public:
    NamedPipe(const std::string& name, bool server = false);
    ~NamedPipe();

    bool create();
    bool open();
    void close();

    bool send_message(const Message& msg);
    bool receive_message(Message& msg);

    bool is_open() const;
};

#endif // NAMED_PIPE_H