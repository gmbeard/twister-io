#include "twister/event_trigger.hpp"
#include "twister/event_loop.hpp"
#include <system_error>
#include <unistd.h>
#include <fcntl.h>

#define CHECK_IO_ERR(e)     \
do {    \
    if (0 > (e)) {  \
        throw std::system_error { (int)errno, std::system_category() }; \
    }   \
}   \
while (0)

#define CHECK_IO_ERR_R(e, var)  \
auto&& var = (e);  \
do {    \
    if (0 > var) { \
        throw std::system_error { (int)errno, std::system_category() }; \
    }   \
}   \
while (0)

namespace {
    constexpr size_t kReadEnd = 0;
    constexpr size_t kWriteEnd = 1;
}

using namespace twister;

EventTrigger::EventTrigger() {
    CHECK_IO_ERR(::pipe(fd_pair_));
    CHECK_IO_ERR(::fcntl(fd_pair_[kReadEnd], F_SETFL, O_NONBLOCK));
    CHECK_IO_ERR(::fcntl(fd_pair_[kWriteEnd], F_SETFL, O_NONBLOCK));
}

EventTrigger::EventTrigger(EventTrigger&& other) noexcept {
    fd_pair_[kReadEnd] = other.fd_pair_[kReadEnd];
    fd_pair_[kWriteEnd] = other.fd_pair_[kWriteEnd];
    other.fd_pair_[kReadEnd] = -1;
    other.fd_pair_[kWriteEnd] = -1;
}

EventTrigger::~EventTrigger() {
    ::close(fd_pair_[kReadEnd]);
    ::close(fd_pair_[kWriteEnd]);
}

void twister::swap(EventTrigger& lhs, EventTrigger& rhs) noexcept {
    using std::swap;
    swap(lhs.fd_pair_, rhs.fd_pair_);
}

EventTrigger& EventTrigger::operator=(EventTrigger&& rhs) noexcept {
    EventTrigger tmp { std::move(rhs) };
    swap(*this, tmp);
    return *this;
}

void EventTrigger::set() noexcept {
    uint8_t byte = 1;
    auto n = ::write(fd_pair_[kWriteEnd], &byte, 1);
    assert(n == 1 && "EventTrigger::set(): couldn't write to pipe!");
}

void EventTrigger::reset() noexcept {
    constexpr size_t kTmpBufferSize = 128;
    uint8_t buffer[kTmpBufferSize];
    while (true) {
        auto n = ::read(fd_pair_[kReadEnd], &buffer[0], kTmpBufferSize);
        if (0 > n) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            assert(false && "EventTrigger::reset(): couldn't read from pipe!");
        }
    }

    notify(NotifyEvent::Read, fd_pair_[kReadEnd]);
}
