#include "../include/access_manager.h"
#include <iostream>

AccessManager::AccessManager() {
}

AccessManager::~AccessManager() {
}

bool AccessManager::can_acquire_read_lock(int employee_id, int client_pid) const {
    std::map<int, LockInfo>::const_iterator it = locks.find(employee_id);
    if (it == locks.end()) {
        return true;
    }

    const LockInfo& lock_info = it->second;

    return lock_info.type == Access::LOCK_NONE ||
        lock_info.type == Access::LOCK_READ ||
        lock_info.clients.find(client_pid) != lock_info.clients.end();
}

bool AccessManager::can_acquire_write_lock(int employee_id, int client_pid) const {
    std::map<int, LockInfo>::const_iterator it = locks.find(employee_id);
    if (it == locks.end()) {
        return true;
    }

    const LockInfo& lock_info = it->second;

    return lock_info.type == Access::LOCK_NONE ||
        (lock_info.type == Access::LOCK_WRITE &&
            lock_info.clients.find(client_pid) != lock_info.clients.end());
}

bool AccessManager::acquire_read_lock(int employee_id, int client_pid) {
    if (!can_acquire_read_lock(employee_id, client_pid)) {
        return false;
    }

    LockInfo& lock_info = locks[employee_id];
    lock_info.type = Access::LOCK_READ;
    lock_info.clients.insert(client_pid);

    return true;
}

bool AccessManager::acquire_write_lock(int employee_id, int client_pid) {
    if (!can_acquire_write_lock(employee_id, client_pid)) {
        return false;
    }

    LockInfo& lock_info = locks[employee_id];
    lock_info.type = Access::LOCK_WRITE;
    lock_info.clients.clear();
    lock_info.clients.insert(client_pid);

    return true;
}

void AccessManager::release_lock(int employee_id, int client_pid) {
    std::map<int, LockInfo>::iterator it = locks.find(employee_id);
    if (it == locks.end()) {
        return;
    }

    LockInfo& lock_info = it->second;
    lock_info.clients.erase(client_pid);

    if (lock_info.clients.empty()) {
        lock_info.type = Access::LOCK_NONE;
        locks.erase(it);
    }
}

void AccessManager::release_all_locks(int client_pid) {
    std::map<int, LockInfo>::iterator it = locks.begin();
    while (it != locks.end()) {
        LockInfo& lock_info = it->second;
        lock_info.clients.erase(client_pid);

        if (lock_info.clients.empty()) {
            locks.erase(it++);
        }
        else {
            ++it;
        }
    }
}

bool AccessManager::has_lock(int employee_id, int client_pid) const {
    std::map<int, LockInfo>::const_iterator it = locks.find(employee_id);
    if (it == locks.end()) {
        return false;
    }

    return it->second.clients.find(client_pid) != it->second.clients.end();
}

Access::LockType AccessManager::get_lock_type(int employee_id, int client_pid) const {
    if (!has_lock(employee_id, client_pid)) {
        return Access::LOCK_NONE;
    }

    return locks.find(employee_id)->second.type;
}

void AccessManager::display_locks() const {
    std::cout << "\n=== Current Locks ===" << std::endl;
    for (std::map<int, LockInfo>::const_iterator it = locks.begin();
        it != locks.end(); ++it) {
        std::cout << "Employee " << it->first << ": ";
        if (it->second.type == Access::LOCK_READ) {
            std::cout << "READ lock by clients: ";
        }
        else if (it->second.type == Access::LOCK_WRITE) {
            std::cout << "WRITE lock by client: ";
        }

        for (std::set<int>::const_iterator client_it = it->second.clients.begin();
            client_it != it->second.clients.end(); ++client_it) {
            std::cout << *client_it << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "====================" << std::endl;
}