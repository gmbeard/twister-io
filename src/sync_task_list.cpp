#include "twister/sync_task_list.hpp"

using namespace twister;

std::optional<tasks::TaskId> SyncTaskList::try_pop() {
    std::unique_lock<std::mutex> lock { mutex_ };
    if (!tasks_.size()) {
        return std::nullopt;
    }

    tasks::TaskId t { std::move(tasks_.back()) };
    tasks_.pop_back();
    return t;
}

void SyncTaskList::push(tasks::TaskId task_id) {
    std::unique_lock<std::mutex> lock { mutex_ };
    tasks_.emplace_back(task_id);
}

size_t SyncTaskList::size() const noexcept {
    std::unique_lock<std::mutex> lock { mutex_ };
    return tasks_.size();
}
