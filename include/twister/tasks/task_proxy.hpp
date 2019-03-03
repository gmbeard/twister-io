#ifndef TWISTER_TASKS_TASK_PROXY_HPP_INCLUDED
#define TWISTER_TASKS_TASK_PROXY_HPP_INCLUDED

#include "twister/tasks/concepts.hpp"
#include <memory>
#include <type_traits>

namespace twister::tasks {

struct TaskProxy;

template<typename T>
concept bool ProxyableTask = 
    concepts::AsyncTask<T> && 
    !std::is_same_v<std::decay_t<T>, TaskProxy>;

struct TaskProxy {
    template<ProxyableTask F>
    explicit TaskProxy(F&& inner) :
        inner_ { 
            std::make_unique<Model<std::decay_t<F>>>(
                std::forward<F>(inner))
        }
    { }

    bool poll();

    // NOTE: When TaskProxy is used as the type for 
    // a std::map, and operator() is defined, GCC 8.1 
    // generates an ICE!!
    //
//    auto operator()() -> bool;

private:
    struct Interface {
        virtual ~Interface() { }
        virtual auto call() -> bool = 0;
    };

    template<ProxyableTask F>
    struct Model : Interface {
        explicit Model(F&& inner) :
            inner_ { std::forward<F>(inner) }
        { }

        auto call() -> bool override {
            return inner_();
        }
    private:
        F inner_;
    };

    std::unique_ptr<Interface> inner_;
};

}

#endif // TWISTER_TASKS_TASK_PROXY_HPP_INCLUDED
