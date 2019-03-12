#ifndef TWISTER_EVENT_LOOP_HPP_INCLUDED
#define TWISTER_EVENT_LOOP_HPP_INCLUDED

#include "twister/tasks/concepts.hpp"
#include "twister/event_trigger.hpp"
#include "twister/tasks/task_proxy.hpp"
#include "twister/tasks/task_id.hpp"
#include "twister/sync_task_queue.hpp"
#include "twister/sync_task_list.hpp"
#include <map>
#include <atomic>

namespace twister {

enum class NotifyEvent {
    Read,
    Write
};

template<tasks::concepts::AsyncTask T>
void spawn(T&&, tasks::TaskId = tasks::TaskId { });

struct EventLoop {
    EventLoop();
    ~EventLoop();
    EventLoop(EventLoop const&) = delete;
    EventLoop& operator=(EventLoop const&) = delete;

    template<tasks::concepts::AsyncTask T>
    void run(T&&);
    void run();

    template<tasks::concepts::AsyncTask T>
    friend void spawn(T&&, tasks::TaskId);

    friend void notify(NotifyEvent, int);
    friend void trigger(tasks::TaskId task_id);

private:
    void drain_trigger_list();
    void try_poll_task(tasks::TaskId task_id);

    void enqueue_task(tasks::TaskProxy&&,
                      tasks::TaskId id = tasks::TaskId { });

    void notify_(int fd, NotifyEvent event, tasks::TaskId id);

private:
    int os_event_loop_;
    SyncTaskQueue task_queue_;
    SyncTaskList trigger_list_;
    std::atomic_size_t in_progress_;
    EventTrigger event_trigger_;
    tasks::TaskId event_trigger_task_id_;
};

extern std::atomic<EventLoop*> current_event_loop_ptr;
EventLoop& current_event_loop() noexcept;

void notify(NotifyEvent event, int fd);

void trigger(tasks::TaskId task_id);
}

#include "twister/detail/event_loop.hpp"

#endif // TWISTER_EVENT_LOOP_HPP_INCLUDED
