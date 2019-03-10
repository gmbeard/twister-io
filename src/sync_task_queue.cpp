#include "twister/sync_task_queue.hpp"

using namespace twister;
using namespace twister::tasks;

void SyncTaskQueue::enqueue(TaskId task_id, TaskProxy&& task) {
    std::lock_guard<std::mutex> lock { mutex_ };
    task_queue_.emplace(std::make_pair(task_id, std::move(task)));
}

std::optional<TaskProxy> SyncTaskQueue::try_dequeue(TaskId task_id) {
    std::lock_guard<std::mutex> lock { mutex_ };
    auto pos = task_queue_.find(task_id);
    if (pos == task_queue_.end()) {
        return std::nullopt;
    }
    TaskProxy task { std::move(pos->second) };
    task_queue_.erase(pos);
    return task;
}

size_t SyncTaskQueue::size() const noexcept {
    std::lock_guard<std::mutex> lock { mutex_ };
    return task_queue_.size();
}
