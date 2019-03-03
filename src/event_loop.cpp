#include "twister/event_loop.hpp"
#include "twister/tasks/task_id.hpp"
#include <system_error>
#include <sys/epoll.h>
#include <unistd.h>
#include <cassert>
#include <vector>
#include <algorithm>

using namespace twister;

namespace {

size_t const kMaxEvents = 512;

} // End anonymous

EventLoop::EventLoop() :
    os_event_loop_ { ::epoll_create(kMaxEvents) }
{
    if (0 > os_event_loop_) {
        throw std::system_error { (int)errno, std::system_category() };
    }
}

EventLoop::~EventLoop() {
    ::close(os_event_loop_);
}

EventLoop& twister::get_current_event_loop() noexcept {
    assert(current_event_loop_ptr &&
           "current_event_loop_ptr == nullptr");
    return *current_event_loop_ptr;
}

void EventLoop::enqueue_task(tasks::TaskProxy&& task, tasks::TaskId id) {
    task_queue_.emplace(std::make_pair(id, std::move(task)));
}

void EventLoop::run_() {
    std::vector<epoll_event> events(kMaxEvents);
    while (task_queue_.size()) {
        int num_of_events = epoll_wait(os_event_loop_,
                                       &events[0],
                                       kMaxEvents,
                                       -1);

        if (0 > num_of_events) {
            throw std::system_error { (int)errno, std::system_category() };
        }

        std::for_each(
            begin(events), 
            begin(events) + num_of_events, 
            [=](auto const& ev) {
                tasks::TaskId id(ev.data.u32);
                auto pos = task_queue_.find(id);

                assert(pos != task_queue_.end() &&
                       "Received event for non-existent task!");

                bool result = with_task(id, [&] {
                    return pos->second.poll();
                });

                if (result) {
                    task_queue_.erase(pos);
                }
            });
    }
}