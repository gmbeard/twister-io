#include "twister/tasks/task_id.hpp"
#include <atomic>

using namespace twister::tasks;

namespace {

uint32_t next_task_id() noexcept {
    static std::atomic<TaskIdType> current_task_id { 0 };
    return ++current_task_id;
}

} // End anonymous

thread_local TaskId twister::tasks::current_task_id = 
    TaskId(std::numeric_limits<TaskIdType>::max());

TaskId::TaskId() noexcept :
    value_ { next_task_id() }
{ }

TaskId::TaskId(TaskIdType value) noexcept :
    value_ { value }
{ }

TaskIdType TaskId::value() const noexcept { return value_; }

bool twister::tasks::operator==(TaskId const& lhs,
                                TaskId const& rhs) noexcept
{
    return lhs.value() == rhs.value();
}

bool twister::tasks::operator!=(TaskId const& lhs,
                                TaskId const& rhs) noexcept
{
    return !(lhs == rhs);
}

bool twister::tasks::operator<(TaskId const& lhs,
                               TaskId const& rhs) noexcept
{
    return lhs.value_ < rhs.value_;
}

//bool twister::tasks::operator<=(TaskId const& lhs,
//                                TaskId const& rhs) noexcept
//{
//    return lhs < rhs || lhs == rhs;
//}
//
//bool twister::tasks::operator>(TaskId const& lhs,
//                               TaskId const& rhs) noexcept
//{
//    return !(lhs <= rhs);
//}
//
//bool twister::tasks::operator>=(TaskId const& lhs,
//                                TaskId const& rhs) noexcept
//{
//    return !(lhs < rhs);
//}
