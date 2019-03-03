#ifndef TWISTER_EVENT_LOOP_HPP_INCLUDED
#define TWISTER_EVENT_LOOP_HPP_INCLUDED

#include "twister/tasks/concepts.hpp"
#include "twister/tasks/task_proxy.hpp"
#include "twister/tasks/task_id.hpp"
#include <map>

namespace twister {

struct EventLoop {
    EventLoop();
    ~EventLoop();
    EventLoop(EventLoop const&) = delete;
    EventLoop& operator=(EventLoop const&) = delete;

    template<tasks::concepts::AsyncTask T>
    void run(T&&);

    template<tasks::concepts::AsyncTask T>
    friend void spawn(T&&);

private:
    void enqueue_task(tasks::TaskProxy&&,
                      tasks::TaskId id = tasks::TaskId { });
    void run_();

private:
    int os_event_loop_;
    std::map<twister::tasks::TaskId, tasks::TaskProxy> task_queue_;
};

EventLoop* current_event_loop_ptr = nullptr;
EventLoop& get_current_event_loop() noexcept;

template<tasks::concepts::AsyncTask T>
void spawn(T&&);

}

#include "twister/detail/event_loop.hpp"

#endif // TWISTER_EVENT_LOOP_HPP_INCLUDED
