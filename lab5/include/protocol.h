#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "common.h"

enum MessageType {
    MSG_READ_REQUEST = 1,
    MSG_WRITE_REQUEST = 2,
    MSG_READ_RESPONSE = 3,
    MSG_WRITE_RESPONSE = 4,
    MSG_WRITE_COMMIT = 5,
    MSG_RELEASE_LOCK = 6,
    MSG_ERROR = 7,
    MSG_SUCCESS = 8
};

struct Message {
    MessageType type;
    int employee_id;
    employee emp_data;
    int client_pid;
    char error_msg[256];
};

void initialize_message(Message& msg);
void set_error_message(Message& msg, const std::string& error);

#endif // PROTOCOL_H