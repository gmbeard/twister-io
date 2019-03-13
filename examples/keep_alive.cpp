#include "keep_alive.hpp"

using namespace twister::examples;

KeepAlive::KeepAlive() noexcept :
    done_ { false }
{ }

bool KeepAlive::operator()() noexcept {
    auto r = done_;
    done_ = !done_;
    return r;
}
