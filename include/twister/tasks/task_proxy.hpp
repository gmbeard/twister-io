#ifndef TWISTER_TASKS_TASK_PROXY_HPP_INCLUDED
#define TWISTER_TASKS_TASK_PROXY_HPP_INCLUDED

#include "twister/tasks/concepts.hpp"
#include <memory>
#include <type_traits>

namespace twister::tasks {

struct TaskProxy {
    template<concepts::Task F>
    explicit TaskProxy(F&& inner) :
        inner_ { 
            std::make_unique<Model<std::decay_t<F>>>(
                std::forward<F>(inner))
        }
    { }

    auto operator()() -> bool {
        return inner_->call();
    }

private:
    struct Interface {
        virtual ~Interface() { }
        virtual auto call() -> bool = 0;
    };

    template<concepts::Task F>
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
