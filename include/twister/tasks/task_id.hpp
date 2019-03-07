#ifndef TWISTER_TASKS_TASK_ID_HPP_INCLUDED
#define TWISTER_TASKS_TASK_ID_HPP_INCLUDED

#include "twister/tasks/concepts.hpp"
#include "twister/scope_exit.hpp"

#include <cstddef>
#include <cinttypes>
#include <cassert>
#include <limits>

namespace twister::tasks {

using TaskIdType = uint32_t;

struct TaskId {
    TaskId() noexcept;
    explicit TaskId(TaskIdType) noexcept;
    TaskIdType value() const noexcept;
//    friend bool operator==(TaskId const&, TaskId const&) noexcept;
    friend bool operator<(TaskId const&, TaskId const&) noexcept;
private:
    TaskIdType value_;
};

extern TaskId current_task_id;

template<tasks::concepts::AsyncTask T>
auto with_task(TaskId id, T&& t) {
//    assert(
//        current_task_id.value() == std::numeric_limits<TaskIdType>::max() &&
//        "current_task_id has already been set!"); 
    auto previous_task = current_task_id;
    current_task_id = id;
    auto reset_task_on_exit = scope_exit([=] {
        current_task_id = previous_task;
    });
    return std::forward<T>(t)();
}

//bool operator==(TaskId const&, TaskId const&) noexcept;
//bool operator!=(TaskId const&, TaskId const&) noexcept;
bool operator<(TaskId const&, TaskId const&) noexcept;
//bool operator<=(TaskId const&, TaskId const&) noexcept;
//bool operator>(TaskId const&, TaskId const&) noexcept;
//bool operator>=(TaskId const&, TaskId const&) noexcept;

//struct TaskIdHash {
//    TaskIdType operator()(TaskId const& task_id) const noexcept {
//        return task_id.value();
//    }
//};
}

//namespace std {
//template<>
//struct hash<twister::tasks::TaskId> {
//    using argument_type = twister::tasks::TaskId;
//    using result_type = twister::tasks::TaskIdType;
//
//    result_type operator()(argument_type const& arg) const noexcept {
//        return arg.value();
//    }
//};
//}

#endif // TWISTER_TASKS_TASK_ID_HPP_INCLUDED
