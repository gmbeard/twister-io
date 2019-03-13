#ifndef TWISTER_THREAD_POOL_HPP_INCLUDED
#define TWISTER_THREAD_POOL_HPP_INCLUDED

#include "twister/tasks/task_id.hpp"
#include "twister/event_loop.hpp"
#include "keep_alive.hpp"

#include <thread>
#include <cstddef>
#include <vector>

namespace twister {

template<typename T>
struct ThreadPool {
    explicit ThreadPool(size_t num_of_threads, T&& inner) noexcept :
        thread_count_ { num_of_threads }
    ,   inner_ { std::move(inner) }
    { }

    ThreadPool(ThreadPool&&) = default; 

    ~ThreadPool() {
        for (auto&& t : threads_) {
            // This check is required because _any_ of these threads
            // may dispose of _this_ object, meaning one of the
            // threads in `threads_` may call join on itself!
            if (t.get_id() != std::this_thread::get_id()) {
                t.join();
            }
        }
    }

    bool operator()() {
        if (!threads_.size() && thread_count_) {
            twister::spawn(examples::KeepAlive { }, keep_alive_id_);

            for (size_t i = 0; i < thread_count_; ++i) {
                threads_.emplace_back([] { 
                    twister::current_event_loop().run(); 
                });
            }
        }

        if (inner_()) {
            twister::trigger(keep_alive_id_);
            return true;
        }
        return false;    
    }
private:
    size_t thread_count_;
    T inner_;
    std::vector<std::thread> threads_;
    tasks::TaskId keep_alive_id_;
};

template<typename T>
auto thread_pool(size_t thread_count, T&& inner) {
    return ThreadPool<std::decay_t<T>> { 
        thread_count,
        std::forward<T>(inner)
    };
}

}

#endif // TWISTER_THREAD_POOL_HPP_INCLUDED
