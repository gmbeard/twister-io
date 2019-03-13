#ifndef TWISTER_EXAMPLES_KEEP_ALIVE_HPP_INCLUDED
#define TWISTER_EXAMPLES_KEEP_ALIVE_HPP_INCLUDED

namespace twister::examples {

struct KeepAlive {
    KeepAlive() noexcept;
    bool operator()() noexcept;
private:
    bool done_;
};

}

#endif // TWISTER_EXAMPLES_KEEP_ALIVE_HPP_INCLUDED
