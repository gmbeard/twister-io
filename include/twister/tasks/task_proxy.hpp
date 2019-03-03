#ifndef TWISTER_TASKS_TASK_PROXY_HPP_INCLUDED
#define TWISTER_TASKS_TASK_PROXY_HPP_INCLUDED

#include "twister/tasks/concepts.hpp"
#include <memory>
#include <type_traits>

namespace twister::tasks {

struct TaskProxy {
    template<concepts::AsyncTask F>
    explicit TaskProxy(F&& inner) :
        inner_ { 
            std::make_unique<Model<std::decay_t<F>>>(
                std::forward<F>(inner))
        }
    { }

    bool poll();

    // NOTE: When TaskProxy is used as the type for 
    // a std::map, and operator() is defined, TGCC 8.1 
    // generates an ICE!!
    //
//    auto operator()() -> bool {
//        return inner_->call();
//    }

private:
    struct Interface {
        virtual ~Interface() { }
        virtual auto call() -> bool = 0;
    };

    template<concepts::AsyncTask F>
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
