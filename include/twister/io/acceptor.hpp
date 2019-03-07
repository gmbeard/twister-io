#ifndef TWISTER_IO_ACCEPTOR_HPP_INCLUDED
#define TWISTER_IO_ACCEPTOR_HPP_INCLUDED

#include "twister/io/tcp_listener.hpp"
#include "twister/io/tcp_stream.hpp"
#include "twister/tasks/concepts.hpp"
#include "twister/event_loop.hpp"
#include <utility>

namespace twister::io {

template<typename T>
concept bool StreamFactory = requires(T a, TcpStream&& s) {
    { a(std::move(s)) } -> 
        twister::tasks::concepts::AsyncTask;
};

template<StreamFactory S>
auto acceptor(TcpListener listener,
              S&& factory)
{
    return [
        listener=std::move(listener),
        factory=std::forward<S>(factory)
    ] () mutable {
        TcpStream stream;
        while (true) {
            if (!listener.accept(stream)) {
                return false;
            }

            twister::spawn(factory(std::move(stream)));
        }
    };
}

}

#endif // TWISTER_IO_ACCEPTOR_HPP_INCLUDED
