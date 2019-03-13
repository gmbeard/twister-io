#ifndef TWISTER_EXAMPLES_LIMITED_ACCEPTOR_HPP_INCLUDED
#define TWISTER_EXAMPLES_LIMITED_ACCEPTOR_HPP_INCLUDED

#include "twister/io/tcp_listener.hpp"
#include "twister/io/tcp_stream.hpp"
#include "twister/event_loop.hpp"
#include <cstddef>

namespace twister::examples {

template<twister::io::StreamFactory F>
auto limited_acceptor(twister::io::TcpListener&& listener,
                      size_t accept_count,
                      F&& fact)
{
    return [
        listener=std::move(listener),
        accept_count,
        factory=std::forward<F>(fact)
    ] () mutable {
        twister::io::TcpStream s;
        while (accept_count) {
            if (!listener.accept(s)) {
                return false;
            }
            twister::spawn(factory(std::move(s)));
            --accept_count;
        }

        return true;
    };
}

}
#endif // TWISTER_EXAMPLES_LIMITED_ACCEPTOR_HPP_INCLUDED
