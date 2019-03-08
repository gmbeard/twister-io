#include "twister/event_loop.hpp"
#include "twister/io/tcp_listener.hpp"
#include "twister/io/tcp_stream.hpp"
#include "twister/io/acceptor.hpp"
#include <vector>
#include <cinttypes>
#include <iostream>

#define UNUSED(arg) (void)(arg)

struct EchoHandler {
    explicit EchoHandler(twister::io::TcpStream&& stream) noexcept :
        buffer_(512)
    ,   bytes_to_write_ { 0 }
    ,   bytes_written_ { 0 }
    ,   stream_ { std::move(stream) }
    { 
        std::cerr << "# Client connected\n";
    }

    bool operator()() {
        while (true) {
            if (!bytes_to_write_) {
                std::cerr << "# Reading... ";
                if (!stream_.read(&buffer_[0], 
                                  buffer_.size(), 
                                  bytes_to_write_)) 
                {
                    std::cerr << "Not ready\n";
                    return false;
                }

                std::cerr << bytes_to_write_ << " bytes\n";

                if (bytes_to_write_ == 0) {
                    break;
                }

                bytes_written_ = 0;
            }
            else {
                std::cerr << "# Writing... ";
                size_t written = 0;
                if (!stream_.write(&buffer_[bytes_written_],
                                   bytes_to_write_ - bytes_written_,
                                   written))
                {
                    std::cerr << "Not ready\n";
                    return false;
                }

                if (written == 0) {
                    break;
                }

                std::cerr << written << "bytes\n";

                bytes_written_ += written;
                if (bytes_written_ == bytes_to_write_) {
                    bytes_to_write_ = 0;
                }
            }
        }

        std::cerr << "# Client done\n";

        return true;
    }

private:
    std::vector<uint8_t> buffer_;
    size_t bytes_to_write_;
    size_t bytes_written_;
    twister::io::TcpStream stream_;
};

int main(int argc, char const** argv) {
    UNUSED(argc);
    UNUSED(argv);

    twister::io::TcpListener listener { "127.0.0.1", 8080 };
    twister::EventLoop loop;
    loop.run(
        twister::io::acceptor(
            std::move(listener),
            [](auto&& stream) {
                return EchoHandler { std::move(stream) };
            }
        )
    );

    return 0;
}
