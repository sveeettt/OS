#include "../include/named_pipe.h"
#include "../include/file_manager.h"
#include "../include/access_manager.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <sstream>
#include <cstring>

#include <windows.h>
#include <process.h>
#include <conio.h>
#define sleep(x) Sleep((x)*1000)

class Server {
private:
    std::vector<NamedPipe*> pipes;
    FileManager file_manager;
    AccessManager access_manager;
    bool running;
    int max_clients;

    std::vector<PROCESS_INFORMATION> client_processes;
    std::vector<HANDLE> pipe_threads;

    std::vector<employee> create_employee_data() {
        std::vector<employee> employees;
        int count;

        std::cout << "Enter number of employees: ";
        std::cin >> count;

        std::cin.ignore();

        for (int i = 0; i < count; ++i) {
            employee emp;
            std::cout << "\nEmployee " << (i + 1) << ":" << std::endl;

            std::cout << "ID: ";
            std::cin >> emp.num;
            std::cin.ignore();

            std::cout << "Name: ";
            std::cin.getline(emp.name, sizeof(emp.name));

            std::cout << "Hours: ";
            std::cin >> emp.hours;
            std::cin.ignore();

            employees.push_back(emp);
        }

        return employees;
    }

    struct ThreadData {
        Server* server;
        NamedPipe* pipe;
        int pipe_index;
    };

    static DWORD WINAPI client_handler_thread(LPVOID lpParam) {
        ThreadData* data = static_cast<ThreadData*>(lpParam);
        data->server->handle_client(data->pipe, data->pipe_index);
        delete data;
        return 0;
    }

    void start_clients() {
        std::cout << "\nEnter number of client processes to start: ";
        std::cin >> max_clients;
        std::cin.ignore();

        for (int i = 0; i < max_clients; ++i) {
            STARTUPINFOA si;
            PROCESS_INFORMATION pi;

            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ZeroMemory(&pi, sizeof(pi));

            std::ostringstream cmd_stream;
            cmd_stream << "cmd /c start \"Client " << (i + 1) << "\" \""
                << "client.exe" << "\"";
            std::string cmd = cmd_stream.str();

            char* cmd_cstr = new char[cmd.length() + 1];
            strcpy(cmd_cstr, cmd.c_str());

            BOOL result = CreateProcessA(
                NULL,
                cmd_cstr,
                NULL,
                NULL,
                FALSE,
                CREATE_NEW_CONSOLE,
                NULL,
                NULL,
                &si,
                &pi
            );

            if (result) {
                client_processes.push_back(pi);
                std::cout << "Started client process " << (i + 1) << " (PID: " << pi.dwProcessId << ")" << std::endl;
                Sleep(500);
            }
            else {
                DWORD error = GetLastError();
                std::cerr << "Failed to start client process " << (i + 1)
                    << ", error code: " << error << std::endl;

                std::string direct_cmd = "client.exe";
                char* direct_cmd_cstr = new char[direct_cmd.length() + 1];
                strcpy(direct_cmd_cstr, direct_cmd.c_str());

                if (CreateProcessA(NULL, direct_cmd_cstr, NULL, NULL, FALSE,
                    CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
                    client_processes.push_back(pi);
                    std::cout << "Started client process " << (i + 1)
                        << " directly (PID: " << pi.dwProcessId << ")" << std::endl;
                }
                else {
                    std::cerr << "Failed to start client directly, error: " << GetLastError() << std::endl;
                }
                delete[] direct_cmd_cstr;
            }
            delete[] cmd_cstr;
        }
    }

    void cleanup_clients() {
        for (size_t i = 0; i < pipe_threads.size(); ++i) {
            if (pipe_threads[i] != INVALID_HANDLE_VALUE) {
                WaitForSingleObject(pipe_threads[i], 5000);
                CloseHandle(pipe_threads[i]);
            }
        }
        pipe_threads.clear();

        for (size_t i = 0; i < client_processes.size(); ++i) {
            if (WaitForSingleObject(client_processes[i].hProcess, 1000) == WAIT_TIMEOUT) {
                TerminateProcess(client_processes[i].hProcess, 0);
            }
            CloseHandle(client_processes[i].hProcess);
            CloseHandle(client_processes[i].hThread);
        }
        client_processes.clear();
    }

    void check_console_commands() {
        if (_kbhit()) {
            std::string command;
            std::getline(std::cin, command);

            if (command == "exit") {
                running = false;
                std::cout << "Server shutting down..." << std::endl;
            }
            else if (command == "show") {
                file_manager.display_file();
            }
            else if (command == "locks") {
                access_manager.display_locks();
            }
            else if (command == "help") {
                std::cout << "\nAvailable commands:" << std::endl;
                std::cout << "  show  - display employee file" << std::endl;
                std::cout << "  locks - display current locks" << std::endl;
                std::cout << "  exit  - shutdown server" << std::endl;
                std::cout << "  help  - show this help" << std::endl;
            }
            else if (!command.empty()) {
                std::cout << "Unknown command: " << command << ". Type 'help' for available commands." << std::endl;
            }
        }
    }

    void handle_client(NamedPipe* pipe, int pipe_index) {
        std::cout << "Handler thread started for pipe " << pipe_index << std::endl;

        if (!pipe->is_open()) {
            std::cerr << "Pipe " << pipe_index << " is not open" << std::endl;
            return;
        }

        std::cout << "Client connected to pipe " << pipe_index << std::endl;

        bool client_connected = true;
        int client_pid = -1;

        while (client_connected && running) {
            Message msg;
            if (pipe->receive_message(msg)) {
                if (client_pid == -1) {
                    client_pid = msg.client_pid;
                }

                Message response;
                handle_message(msg, response);
                if (!pipe->send_message(response)) {
                    std::cerr << "Failed to send response to client on pipe " << pipe_index << std::endl;
                    client_connected = false;
                }
            }
            else {
                client_connected = false;
            }
        }

        std::cout << "Client disconnected from pipe " << pipe_index << std::endl;

        if (client_pid != -1) {
            access_manager.release_all_locks(client_pid);
            std::cout << "Released all locks for client " << client_pid << std::endl;
        }

        pipe->close();

        std::ostringstream pipe_name;
        pipe_name << PIPE_NAME << "_" << pipe_index;

        if (pipe->create() && pipe->open()) {
            std::cout << "Pipe " << pipe_index << " recreated and ready for next client" << std::endl;
            handle_client(pipe, pipe_index);
        }
        else {
            std::cerr << "Failed to recreate pipe " << pipe_index << std::endl;
        }
    }

    void handle_message(const Message& msg, Message& response) {
        initialize_message(response);
        response.employee_id = msg.employee_id;
        response.client_pid = msg.client_pid;

        switch (msg.type) {
        case MSG_READ_REQUEST:
            handle_read_request(msg, response);
            break;
        case MSG_WRITE_REQUEST:
            handle_write_request(msg, response);
            break;
        case MSG_WRITE_COMMIT:
            handle_write_commit(msg, response);
            break;
        case MSG_RELEASE_LOCK:
            handle_release_lock(msg, response);
            break;
        default:
            set_error_message(response, "Unknown message type");
            break;
        }
    }

    void handle_read_request(const Message& msg, Message& response) {
        if (!access_manager.can_acquire_read_lock(msg.employee_id, msg.client_pid)) {
            set_error_message(response, "Cannot acquire read lock (write lock exists)");
            return;
        }

        employee emp;
        if (!file_manager.read_employee(msg.employee_id, emp)) {
            set_error_message(response, "Employee not found");
            return;
        }

        access_manager.acquire_read_lock(msg.employee_id, msg.client_pid);
        response.type = MSG_READ_RESPONSE;
        response.emp_data = emp;

        std::cout << "Client " << msg.client_pid << " acquired READ lock for employee " << msg.employee_id << std::endl;
    }

    void handle_write_request(const Message& msg, Message& response) {
        if (!access_manager.can_acquire_write_lock(msg.employee_id, msg.client_pid)) {
            set_error_message(response, "Cannot acquire write lock (lock exists)");
            return;
        }

        employee emp;
        if (!file_manager.read_employee(msg.employee_id, emp)) {
            set_error_message(response, "Employee not found");
            return;
        }

        access_manager.acquire_write_lock(msg.employee_id, msg.client_pid);
        response.type = MSG_WRITE_RESPONSE;
        response.emp_data = emp;

        std::cout << "Client " << msg.client_pid << " acquired WRITE lock for employee " << msg.employee_id << std::endl;
    }

    void handle_write_commit(const Message& msg, Message& response) {
        if (access_manager.get_lock_type(msg.employee_id, msg.client_pid) != Access::LOCK_WRITE) {
            set_error_message(response, "No write lock held");
            return;
        }

        if (!file_manager.write_employee(msg.emp_data)) {
            set_error_message(response, "Failed to write employee data");
            return;
        }

        response.type = MSG_SUCCESS;
        std::cout << "Client " << msg.client_pid << " committed changes for employee " << msg.employee_id << std::endl;
    }

    void handle_release_lock(const Message& msg, Message& response) {
        access_manager.release_lock(msg.employee_id, msg.client_pid);
        response.type = MSG_SUCCESS;
        std::cout << "Client " << msg.client_pid << " released lock for employee " << msg.employee_id << std::endl;
    }

public:
    Server(const std::string& filename)
        : file_manager(filename), running(false), max_clients(0) {
    }

    ~Server() {
        cleanup_clients();
        for (size_t i = 0; i < pipes.size(); ++i) {
            delete pipes[i];
        }
        pipes.clear();
    }

    bool initialize() {
        std::cout << "=== Employee Database Server ===" << std::endl;

        if (!file_manager.file_exists()) {
            std::cout << "\nFile does not exist. Creating new employee database." << std::endl;
            std::vector<employee> employees = create_employee_data();
            if (!file_manager.create_file(employees)) {
                std::cerr << "Failed to create employee file" << std::endl;
                return false;
            }
            std::cout << "Employee database created successfully!" << std::endl;
        }
        else {
            std::cout << "\nUsing existing employee database." << std::endl;
        }

        std::cout << "\nInitial employee file:" << std::endl;
        file_manager.display_file();

        start_clients();

        for (int i = 0; i < max_clients; ++i) {
            std::ostringstream pipe_name;
            pipe_name << PIPE_NAME << "_" << i;
            NamedPipe* pipe = new NamedPipe(pipe_name.str(), true);

            if (!pipe->create()) {
                std::cerr << "Failed to create named pipe " << i << std::endl;
                delete pipe;
                return false;
            }
            pipes.push_back(pipe);
        }

        std::cout << "\n=== Server Ready ===" << std::endl;
        std::cout << "Created " << pipes.size() << " named pipes for clients" << std::endl;
        std::cout << "Server commands: show, locks, exit, help" << std::endl;
        std::cout << "Waiting for client connections..." << std::endl;

        return true;
    }

    void run() {
        running = true;

        for (size_t i = 0; i < pipes.size(); ++i) {
            if (!pipes[i]->open()) {
                std::cerr << "Failed to open pipe " << i << " for listening" << std::endl;
                return;
            }
            std::cout << "Pipe " << i << " is now listening for connections..." << std::endl;
        }

        for (size_t i = 0; i < pipes.size(); ++i) {
            ThreadData* data = new ThreadData;
            data->server = this;
            data->pipe = pipes[i];
            data->pipe_index = static_cast<int>(i);

            HANDLE hThread = CreateThread(
                NULL,
                0,
                client_handler_thread,
                data,
                0,
                NULL
            );

            if (hThread != NULL) {
                pipe_threads.push_back(hThread);
                std::cout << "Created handler thread for pipe " << i << std::endl;
            }
            else {
                std::cerr << "Failed to create thread for pipe " << i << std::endl;
                delete data;
                pipe_threads.push_back(INVALID_HANDLE_VALUE);
            }
        }

        while (running) {
            check_console_commands();
            Sleep(100);
        }

        std::cout << "\n=== Server Shutdown ===" << std::endl;
        std::cout << "Final employee file:" << std::endl;
        file_manager.display_file();

        cleanup_clients();
    }
};

int main() {
    std::cout << "=== File Access Control System ===" << std::endl;

    std::string filename;
    std::cout << "Enter filename for employee database: ";
    std::getline(std::cin, filename);

    if (filename.empty()) {
        filename = "employees.dat";
        std::cout << "Using default filename: " << filename << std::endl;
    }

    Server server(filename);
    if (!server.initialize()) {
        return 1;
    }

    server.run();

    return 0;
}