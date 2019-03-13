#include "twister/progress_guard.hpp"
#include <cassert>

using namespace twister;

ProgressGuard::ProgressGuard(std::atomic_size_t* counter) noexcept :
    counter_(counter)
,   released_(false)
{
    counter_->fetch_add(1);
}

ProgressGuard::~ProgressGuard() {
    if (!released_) {
        release();
    }
}

void ProgressGuard::release() noexcept {
    released_ = true;
    auto prev = counter_->fetch_sub(1);
    assert(prev > 0 && "ProgressGuard::counter_ == 0!");
    (void)(prev);
}
