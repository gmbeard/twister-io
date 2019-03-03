#ifndef TWISTER_SCOPE_EXIT_HPP_INCLUDED
#define TWISTER_SCOPE_EXIT_HPP_INCLUDED

#include <utility>
#include <type_traits>

namespace twister {

template<typename F>
struct ScopeExit {
    ScopeExit(F&& f) :
        f_ { std::forward<F>(f) }
    ,   active_ { true }
    { }

    ScopeExit(ScopeExit const&) = delete;
    ScopeExit(ScopeExit&& other) noexcept :
        f_ { std::move(other.f_) }
    ,   active_ { other.active_ }
    {
        other.active_ = false;
    }

    ScopeExit& operator=(ScopeExit const&) = delete;
    ScopeExit& operator=(ScopeExit&&) = delete;

    ~ScopeExit() {
        if (active_) {
            f_();
        }
    }
private:
    F f_;
    bool active_;
};

template<typename F>
auto scope_exit(F&& f) {
    return ScopeExit<std::decay_t<F>> { std::forward<F>(f) };
}

}

#endif // TWISTER_SCOPE_EXIT_HPP_INCLUDED
