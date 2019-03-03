#ifndef TWISTER_EVENT_LOOP_HPP_INCLUDED
#define TWISTER_EVENT_LOOP_HPP_INCLUDED

#include "twister/tasks/concepts.hpp"
#include "twister/tasks/task_proxy.hpp"
#include "twister/tasks/task_id.hpp"
#include <map>

namespace twister {

enum class NotifyEvent {
    Read,
    Write
};

struct EventLoop {
    EventLoop();
    ~EventLoop();
    EventLoop(EventLoop const&) = delete;
    EventLoop& operator=(EventLoop const&) = delete;

    template<tasks::concepts::AsyncTask T>
    void run(T&&);

    template<tasks::concepts::AsyncTask T>
    friend void spawn(T&&);

    friend void notify(NotifyEvent, int);

private:
    void enqueue_task(tasks::TaskProxy&&,
                      tasks::TaskId id = tasks::TaskId { });

    void notify_(int fd, NotifyEvent event, tasks::TaskId id);
    void run_();

private:
    int os_event_loop_;
    std::map<twister::tasks::TaskId, tasks::TaskProxy> task_queue_;
};

EventLoop* current_event_loop_ptr = nullptr;
EventLoop& current_event_loop() noexcept;

template<tasks::concepts::AsyncTask T>
void spawn(T&&);

void notify(NotifyEvent event, int fd);

}

#include "twister/detail/event_loop.hpp"

#endif // TWISTER_EVENT_LOOP_HPP_INCLUDED
