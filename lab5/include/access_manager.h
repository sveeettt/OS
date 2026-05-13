#ifndef ACCESS_MANAGER_H
#define ACCESS_MANAGER_H

#include "common.h"
#include <map>
#include <set>

namespace Access {
    enum LockType {
        LOCK_NONE = 0,
        LOCK_READ = 1,
        LOCK_WRITE = 2
    };
}

struct LockInfo {
    Access::LockType type;
    std::set<int> clients;

    LockInfo() : type(Access::LOCK_NONE) {}
};

class AccessManager {
private:
    std::map<int, LockInfo> locks;

public:
    AccessManager();
    ~AccessManager();

    bool can_acquire_read_lock(int employee_id, int client_pid) const;
    bool can_acquire_write_lock(int employee_id, int client_pid) const;

    bool acquire_read_lock(int employee_id, int client_pid);
    bool acquire_write_lock(int employee_id, int client_pid);

    void release_lock(int employee_id, int client_pid);
    void release_all_locks(int client_pid);

    bool has_lock(int employee_id, int client_pid) const;
    Access::LockType get_lock_type(int employee_id, int client_pid) const;

    void display_locks() const;
};

#endif // ACCESS_MANAGER_H