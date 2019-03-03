#ifndef TWISTER_IO_TCP_STREAM_HPP_INCLUDED
#define TWISTER_IO_TCP_STREAM_HPP_INCLUDED

#include <cstddef>
#include <cinttypes>

namespace twister::io {

struct TcpStream {
    TcpStream() noexcept;
    TcpStream(TcpStream&&) noexcept;
    ~TcpStream();
    TcpStream& operator=(TcpStream&&) noexcept;
    friend void swap(TcpStream&, TcpStream&) noexcept;
    bool read(uint8_t* buffer, size_t len, size_t& read);
    bool write(uint8_t const* buffer, size_t len, size_t& written);
    friend TcpStream os_socket_to_stream(int socket) noexcept;
private:
    int socket_;
};

TcpStream os_socket_to_stream(int socket) noexcept;
void swap(TcpStream&, TcpStream&) noexcept;

}

#endif // TWISTER_IO_TCP_STREAM_HPP_INCLUDED
