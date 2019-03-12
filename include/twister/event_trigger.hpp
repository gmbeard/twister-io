#ifndef TWISTER_EVENT_TRIGGER_HPP_INCLUDED
#define TWISTER_EVENT_TRIGGER_HPP_INCLUDED

namespace twister {

struct EventTrigger {
    EventTrigger();
    ~EventTrigger();

    EventTrigger(EventTrigger&& other) noexcept;
    EventTrigger(EventTrigger const&) = delete;

    EventTrigger& operator=(EventTrigger&& rhs) noexcept;
    EventTrigger& operator=(EventTrigger const&) = delete;

    friend void swap(EventTrigger& lhs, EventTrigger& rhs) noexcept;
    void set() noexcept;

    void reset() noexcept;

    int read_fd() const noexcept;
private:
    int fd_pair_[2];
};

void swap(EventTrigger&, EventTrigger&) noexcept;

}

#endif // TWISTER_EVENT_TRIGGER_HPP_INCLUDED
