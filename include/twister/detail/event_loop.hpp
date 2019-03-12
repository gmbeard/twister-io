#ifndef TWISTER_DETAIL_EVENT_LOOP_HPP_INCLUDED
#define TWISTER_DETAIL_EVENT_LOOP_HPP_INCLUDED

#include "twister/tasks/task_id.hpp"
#include <cassert>

template<twister::tasks::concepts::AsyncTask T>
void twister::EventLoop::run(T&& parent) {
    using namespace twister::tasks;

    assert(!current_event_loop_ptr.load() &&
           "current_event_loop_ptr != nullptr!");

    current_event_loop_ptr.store(this);
    auto reset_event_loop_ptr = 
        scope_exit([] { current_event_loop_ptr.store(nullptr); });

    with_task(event_trigger_task_id_, [=] {
        event_trigger_.reset();
        return false;
    });

    TaskId parent_task_id;
    bool result = with_task(parent_task_id, [&] {
        return parent();
    });

    if (!result) {
        enqueue_task(tasks::TaskProxy { std::forward<T>(parent) },
                     parent_task_id);
        run();
    }
}

template<twister::tasks::concepts::AsyncTask T>
void twister::spawn(T&& task, tasks::TaskId new_task_id) {
    using namespace twister;

    bool result = with_task(new_task_id, [&] {
        return task();
    });

    if (!result) {
        current_event_loop().enqueue_task(
            tasks::TaskProxy { std::forward<T>(task) },
            new_task_id
        );
    }
}

#endif // TWISTER_DETAIL_EVENT_LOOP_HPP_INCLUDED
