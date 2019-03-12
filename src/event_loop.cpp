#include "twister/event_loop.hpp"
#include "twister/tasks/task_id.hpp"
#include "twister/scope_exit.hpp"
#include "twister/progress_guard.hpp"
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

std::atomic<EventLoop*> twister::current_event_loop_ptr = nullptr;

EventLoop::EventLoop() :
    os_event_loop_ { ::epoll_create(kMaxEvents) }
,   in_progress_ { 0 }
{
    if (0 > os_event_loop_) {
        throw std::system_error { (int)errno, std::system_category() };
    }
}

EventLoop::~EventLoop() {
    ::close(os_event_loop_);
}

EventLoop& twister::current_event_loop() noexcept {
    EventLoop* p = current_event_loop_ptr.load();
    assert(p && "current_event_loop_ptr == nullptr");
    return *p;
}

void EventLoop::enqueue_task(tasks::TaskProxy&& task, tasks::TaskId id) {
    task_queue_.enqueue(id, std::move(task));
}

void EventLoop::drain_trigger_list() {
    with_task(event_trigger_task_id_, [=] {
        event_trigger_.reset();
        return false;
    });

    for (auto next = trigger_list_.try_pop(); 
        next; 
        next = trigger_list_.try_pop()) 
    {
        try_poll_task(*next);
    }
}

void EventLoop::try_poll_task(tasks::TaskId task_id) {
    ProgressGuard progress_guard { &in_progress_ };
    if (auto task = task_queue_.try_dequeue(task_id); task) {
        if (!with_task(task_id, [&] { return task->poll(); })) {
            task_queue_.enqueue(task_id, std::move(*task));
        }
        progress_guard.release();
    }
}

void EventLoop::run() {
    std::vector<epoll_event> events(kMaxEvents);
    while (true) {
        size_t n = task_queue_.size() +
                   trigger_list_.size() +
                   in_progress_.load();

        if (!n) {
            break;
        }

        int num_of_events = epoll_wait(os_event_loop_,
                                       &events[0],
                                       kMaxEvents,
                                       100);

        if (0 > num_of_events) {
            if (errno == EINTR) {
                continue;
            }
            throw std::system_error { (int)errno, std::system_category() };
        }

        std::for_each(
            begin(events), 
            begin(events) + num_of_events, 
            [=](auto const& ev) {
                tasks::TaskId t { ev.data.u32 };
                if (t == event_trigger_task_id_) {
                    drain_trigger_list();
                }
                else {
                    try_poll_task(t);
                }
            });
    }
}

void EventLoop::notify_(int fd, NotifyEvent event, tasks::TaskId id) {
    epoll_event data = { };
    data.events = EPOLLET | EPOLLONESHOT;
    switch (event) {
        case NotifyEvent::Read:
            data.events |= EPOLLIN;
            break;
        case NotifyEvent::Write:
            data.events |= EPOLLOUT;
            break;
    }

    data.data.u64 = id.value();
    int err = ::epoll_ctl(os_event_loop_,
                          EPOLL_CTL_MOD,
                          fd,
                          &data);

    if (0 > err && errno == ENOENT) {
        err = ::epoll_ctl(os_event_loop_,
                          EPOLL_CTL_ADD,
                          fd,
                          &data);
    }

    if (0 > err) {
        throw std::system_error { (int)errno, std::system_category() };
    }
}

void twister::notify(NotifyEvent event, int fd) {
    tasks::TaskId id = tasks::current_task_id;
    current_event_loop().notify_(fd, event, id);
}

void twister::trigger(tasks::TaskId task_id) {
    EventLoop& loop = current_event_loop();
    loop.trigger_list_.push(task_id);
    loop.event_trigger_.set();
}

