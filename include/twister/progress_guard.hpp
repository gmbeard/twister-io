#ifndef TWISTER_PROGRESS_GUARD_HPP_INCLUDED
#define TWISTER_PROGRESS_GUARD_HPP_INCLUDED

#include <atomic>

namespace twister {

struct ProgressGuard {
    ProgressGuard(std::atomic_size_t* counter) noexcept;
    ProgressGuard(ProgressGuard const&) = delete;
    ~ProgressGuard();
    ProgressGuard& operator=(ProgressGuard const&) = delete;
    void release() noexcept;
private:
    std::atomic_size_t* counter_;
    bool released_;
};

}

#endif // TWISTER_PROGRESS_GUARD_HPP_INCLUDED
