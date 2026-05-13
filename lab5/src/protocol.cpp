#include "../include/protocol.h"
#include <cstring>

void initialize_message(Message& msg) {
    memset(&msg, 0, sizeof(Message));
    msg.type = MSG_ERROR;
    msg.employee_id = -1;
    msg.client_pid = -1;
}

void set_error_message(Message& msg, const std::string& error) {
    msg.type = MSG_ERROR;
    strncpy(msg.error_msg, error.c_str(), sizeof(msg.error_msg) - 1);
    msg.error_msg[sizeof(msg.error_msg) - 1] = '\0';
}