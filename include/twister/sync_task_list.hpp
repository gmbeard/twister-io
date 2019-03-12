#ifndef TWISTER_SYNC_TASK_LIST_HPP_INCLUDED
#define TWISTER_SYNC_TASK_LIST_HPP_INCLUDED

#include "twister/tasks/task_id.hpp"
#include <mutex>
#include <optional>
#include <vector>

namespace twister {

struct SyncTaskList {
    std::optional<tasks::TaskId> try_pop();
    void push(tasks::TaskId task_id);
    size_t size() const noexcept;
private:
    mutable std::mutex mutex_;
    std::vector<tasks::TaskId> tasks_;
};

}

#endif // TWISTER_SYNC_TASK_LIST_HPP_INCLUDED
