#ifndef TWISTER_EXAMPLES_ECHO_HANDLER_HPP_INCLUDED
#define TWISTER_EXAMPLES_ECHO_HANDLER_HPP_INCLUDED

#include "twister/io/tcp_stream.hpp"
#include <vector>
#include <cinttypes>

namespace twister::examples {

struct EchoHandler {
    explicit EchoHandler(twister::io::TcpStream&& stream) noexcept;
    bool operator()();
private:
    std::vector<uint8_t> buffer_;
    size_t bytes_to_write_;
    size_t bytes_written_;
    twister::io::TcpStream stream_;
};

}

#endif // TWISTER_EXAMPLES_ECHO_HANDLER_HPP_INCLUDED
