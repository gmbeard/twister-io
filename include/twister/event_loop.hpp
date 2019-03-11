#ifndef TWISTER_EVENT_LOOP_HPP_INCLUDED
#define TWISTER_EVENT_LOOP_HPP_INCLUDED

#include "twister/tasks/concepts.hpp"
#include "twister/tasks/task_proxy.hpp"
#include "twister/tasks/task_id.hpp"
#include "twister/sync_task_queue.hpp"
#include <map>
#include <atomic>

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
    void run();

    template<tasks::concepts::AsyncTask T>
    friend void spawn(T&&);

    friend void notify(NotifyEvent, int);

private:
    void try_poll_task(tasks::TaskId task_id);

    void enqueue_task(tasks::TaskProxy&&,
                      tasks::TaskId id = tasks::TaskId { });

    void notify_(int fd, NotifyEvent event, tasks::TaskId id);

private:
    int os_event_loop_;
    SyncTaskQueue task_queue_;
    std::atomic_size_t in_progress_;
};

extern EventLoop* current_event_loop_ptr;
EventLoop& current_event_loop() noexcept;

template<tasks::concepts::AsyncTask T>
void spawn(T&&);

void notify(NotifyEvent event, int fd);

}

#include "twister/detail/event_loop.hpp"

#endif // TWISTER_EVENT_LOOP_HPP_INCLUDED
