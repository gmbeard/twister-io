#include "twister/event_loop.hpp"
#include "twister/io/tcp_listener.hpp"
#include "twister/io/tcp_stream.hpp"
#include "twister/io/acceptor.hpp"

#include "echo_handler.hpp"
#include "thread_pool.hpp"
#include "chat_server.hpp"

#define UNUSED(arg) (void)(arg)

int main(int argc, char const** argv) {
    UNUSED(argc);
    UNUSED(argv);

    using namespace twister::examples;

    ChatServer chat_srv;

    twister::io::TcpListener listener { "127.0.0.1", 8080 };
    twister::EventLoop loop;
    loop.run(
        twister::thread_pool(
            2,
            twister::io::acceptor(
                std::move(listener),
                [&](auto&& stream) {
                    return ChatClientHandler { std::move(stream), char_srv };
                }
            )
        )
    );

    return 0;
}
