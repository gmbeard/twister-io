#ifndef TWISTER_SYNC_TASK_QUEUE_HPP_INCLUDED
#define TWISTER_SYNC_TASK_QUEUE_HPP_INCLUDED

#include "twister/tasks/task_id.hpp"
#include "twister/tasks/task_proxy.hpp"
#include <map>
#include <mutex>
#include <optional>

namespace twister {

struct SyncTaskQueue {
    void enqueue(tasks::TaskId task_id, tasks::TaskProxy&& task);
    std::optional<tasks::TaskProxy> try_dequeue(tasks::TaskId task_id);
    size_t size() const noexcept;
private:
    mutable std::mutex mutex_;
    std::map<twister::tasks::TaskId, tasks::TaskProxy> task_queue_;
};

}

#endif // TWISTER_SYNC_TASK_QUEUE_HPP_INCLUDED
