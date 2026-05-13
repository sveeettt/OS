#include "../include/named_pipe.h"
#include "../include/common.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>

#include <windows.h>
#define sleep(x) Sleep((x)*1000)
#define getpid() GetCurrentProcessId()

class Client {
private:
    NamedPipe* pipe;
    int client_pid;
    int pipe_index;

    bool find_available_pipe() {
        client_pid = getpid();

        for (int i = 0; i < 10; ++i) {
            std::ostringstream pipe_name;
            pipe_name << PIPE_NAME << "_" << i;

            std::cout << "Trying to connect to pipe: " << pipe_name.str() << std::endl;

            NamedPipe* test_pipe = new NamedPipe(pipe_name.str(), false);

            for (int attempt = 0; attempt < 10; ++attempt) {
                if (test_pipe->open()) {
                    pipe = test_pipe;
                    pipe_index = i;
                    std::cout << "Connected to pipe " << i << " successfully!" << std::endl;
                    return true;
                }

                DWORD error = GetLastError();
                if (error == ERROR_FILE_NOT_FOUND) {
                    std::cout << "Pipe " << i << " not found (attempt " << (attempt + 1) << "/10)" << std::endl;
                }
                else if (error == ERROR_PIPE_BUSY) {
                    std::cout << "Pipe " << i << " is busy (attempt " << (attempt + 1) << "/10)" << std::endl;
                }
                else {
                    std::cout << "Connection failed with error " << error << " (attempt " << (attempt + 1) << "/10)" << std::endl;
                }

                sleep(i < 2 ? 2 : 1);
            }

            delete test_pipe;
            std::cout << "Failed to connect to pipe " << i << " after 10 attempts" << std::endl;
        }

        return false;
    }

    void handle_read_operation() {
        int employee_id;
        std::cout << "Enter employee ID to read: ";
        std::cin >> employee_id;

        if (!is_valid_employee_id(employee_id)) {
            std::cout << "Invalid employee ID (must be 1-" << MAX_EMPLOYEES << ")" << std::endl;
            return;
        }

        Message request;
        initialize_message(request);
        request.type = MSG_READ_REQUEST;
        request.employee_id = employee_id;
        request.client_pid = client_pid;

        if (!pipe->send_message(request)) {
            std::cerr << "Failed to send read request" << std::endl;
            return;
        }

        Message response;
        if (!pipe->receive_message(response)) {
            std::cerr << "Failed to receive response" << std::endl;
            return;
        }

        if (response.type == MSG_ERROR) {
            std::cout << "Error: " << response.error_msg << std::endl;
            return;
        }

        if (response.type == MSG_READ_RESPONSE) {
            std::cout << "\n=== Employee Data ===" << std::endl;
            std::cout << employee_to_string(response.emp_data) << std::endl;
            std::cout << "=====================" << std::endl;

            std::cout << "\nPress Enter to release lock...";
            std::cin.ignore();
            std::cin.get();
            release_lock(employee_id);
        }
    }

    void handle_write_operation() {
        int employee_id;
        std::cout << "Enter employee ID to modify: ";
        std::cin >> employee_id;

        if (!is_valid_employee_id(employee_id)) {
            std::cout << "Invalid employee ID (must be 1-" << MAX_EMPLOYEES << ")" << std::endl;
            return;
        }

        Message request;
        initialize_message(request);
        request.type = MSG_WRITE_REQUEST;
        request.employee_id = employee_id;
        request.client_pid = client_pid;

        if (!pipe->send_message(request)) {
            std::cerr << "Failed to send write request" << std::endl;
            return;
        }

        Message response;
        if (!pipe->receive_message(response)) {
            std::cerr << "Failed to receive response" << std::endl;
            return;
        }

        if (response.type == MSG_ERROR) {
            std::cout << "Error: " << response.error_msg << std::endl;
            return;
        }

        if (response.type == MSG_WRITE_RESPONSE) {
            employee new_emp = response.emp_data;

            std::cout << "\n=== Current Employee Data ===" << std::endl;
            std::cout << employee_to_string(new_emp) << std::endl;
            std::cout << "=============================" << std::endl;

            std::cout << "\nEnter new values (press Enter to keep current value):" << std::endl;

            std::cout << "Name [" << new_emp.name << "]: ";
            std::cin.ignore();

            char temp_name[256];
            std::cin.getline(temp_name, sizeof(temp_name));

            if (strlen(temp_name) > 0) {
                strncpy(new_emp.name, temp_name, sizeof(new_emp.name) - 1);
                new_emp.name[sizeof(new_emp.name) - 1] = '\0';
                trim_string(new_emp.name);
            }

            std::cout << "Hours [" << new_emp.hours << "]: ";
            std::string hours_str;
            std::getline(std::cin, hours_str);

            if (!hours_str.empty()) {
                new_emp.hours = atof(hours_str.c_str());
                if (new_emp.hours < 0) {
                    new_emp.hours = 0;
                    std::cout << "Hours cannot be negative, set to 0" << std::endl;
                }
            }

            std::cout << "\n=== New Employee Data ===" << std::endl;
            std::cout << employee_to_string(new_emp) << std::endl;
            std::cout << "=========================" << std::endl;

            std::cout << "Commit changes? (y/n): ";
            char confirm;
            std::cin >> confirm;

            if (confirm == 'y' || confirm == 'Y') {
                Message commit_msg;
                initialize_message(commit_msg);
                commit_msg.type = MSG_WRITE_COMMIT;
                commit_msg.employee_id = employee_id;
                commit_msg.client_pid = client_pid;
                commit_msg.emp_data = new_emp;

                if (pipe->send_message(commit_msg)) {
                    Message commit_response;
                    if (pipe->receive_message(commit_response)) {
                        if (commit_response.type == MSG_SUCCESS) {
                            std::cout << "Changes committed successfully!" << std::endl;
                        }
                        else {
                            std::cout << "Error committing changes: " << commit_response.error_msg << std::endl;
                        }
                    }
                    else {
                        std::cout << "Failed to receive commit response" << std::endl;
                    }
                }
                else {
                    std::cout << "Failed to send commit message" << std::endl;
                }
            }
            else {
                std::cout << "Changes discarded." << std::endl;
            }

            std::cout << "\nPress Enter to release lock...";
            std::cin.ignore();
            std::cin.get();
            release_lock(employee_id);
        }
    }

    void release_lock(int employee_id) {
        Message release_msg;
        initialize_message(release_msg);
        release_msg.type = MSG_RELEASE_LOCK;
        release_msg.employee_id = employee_id;
        release_msg.client_pid = client_pid;

        if (pipe->send_message(release_msg)) {
            Message response;
            if (pipe->receive_message(response)) {
                if (response.type == MSG_SUCCESS) {
                    std::cout << "Lock released successfully." << std::endl;
                }
                else {
                    std::cout << "Error releasing lock: " << response.error_msg << std::endl;
                }
            }
            else {
                std::cout << "Failed to receive release response" << std::endl;
            }
        }
        else {
            std::cout << "Failed to send release message" << std::endl;
        }
    }

    void display_menu() {
        std::cout << "\n=== Client " << client_pid << " (Pipe " << pipe_index << ") Menu ===" << std::endl;
        std::cout << "Available commands:" << std::endl;
        std::cout << "  (r) - Read employee record" << std::endl;
        std::cout << "  (w) - Write/modify employee record" << std::endl;
        std::cout << "  (h) - Show this help" << std::endl;
        std::cout << "  (q) - Quit client" << std::endl;
        std::cout << "===============================================" << std::endl;
    }

public:
    Client() : pipe(NULL), client_pid(getpid()), pipe_index(-1) {
    }

    ~Client() {
        if (pipe) {
            delete pipe;
        }
    }

    bool initialize() {
        std::cout << "Client " << client_pid << " starting..." << std::endl;
        std::cout << "Waiting for server to be ready..." << std::endl;
        sleep(5);

        std::cout << "Searching for available server pipe..." << std::endl;

        if (!find_available_pipe()) {
            std::cerr << "Failed to connect to any server pipe after trying all available options" << std::endl;
            std::cerr << "Make sure the server is running and has created the named pipes" << std::endl;
            return false;
        }

        return true;
    }

    void run() {
        std::cout << "\n=== Client " << client_pid << " started (connected to pipe " << pipe_index << ") ===" << std::endl;
        std::cout << "Available commands:" << std::endl;
        std::cout << "  (r) - read employee record" << std::endl;
        std::cout << "  (w) - write/modify employee record" << std::endl;
        std::cout << "  (q) - quit" << std::endl;

        char command;
        while (true) {
            std::cout << "\nClient " << client_pid << " > Enter command (r/w/q): ";
            std::cin >> command;

            switch (command) {
            case 'r':
            case 'R':
                handle_read_operation();
                break;
            case 'w':
            case 'W':
                handle_write_operation();
                break;
            case 'q':
            case 'Q':
                std::cout << "Client " << client_pid << " exiting." << std::endl;
                return;
            default:
                std::cout << "Invalid command. Use r (read), w (write), or q (quit)." << std::endl;
                break;
            }
        }
    }
};

int main() {
    Client client;
    if (!client.initialize()) {
        return 1;
    }
    client.run();
    return 0;
}