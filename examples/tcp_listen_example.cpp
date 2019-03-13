#include "twister/event_loop.hpp"
#include "twister/io/tcp_listener.hpp"
#include "twister/io/tcp_stream.hpp"
#include "twister/io/acceptor.hpp"

#include "echo_handler.hpp"
#include "keep_alive.hpp"
#include "thread_pool.hpp"
#include "limited_acceptor.hpp"

#include <vector>
#include <iostream>
#include <thread>

#define UNUSED(arg) (void)(arg)

int main(int argc, char const** argv) {
    UNUSED(argc);
    UNUSED(argv);

    using namespace twister::examples;

    twister::io::TcpListener listener { "127.0.0.1", 8080 };
    twister::EventLoop loop;
    loop.run(
        twister::thread_pool(
            2,
            //limited_acceptor(
            twister::io::acceptor(
                std::move(listener),
            //    2,
                [](auto&& stream) {
                    return EchoHandler { std::move(stream) };
                }
            )
        )
    );

    return 0;
}
