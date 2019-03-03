#ifndef TWISTER_IO_TCP_LISTENER_HPP_INCLUDED
#define TWISTER_IO_TCP_LISTENER_HPP_INCLUDED

#include "twister/io/tcp_stream.hpp"

namespace twister::io {

struct TcpListener {
    TcpListener(char const* address, uint16_t port);
    TcpListener(TcpListener&&) noexcept;
    ~TcpListener();
    friend void swap(TcpListener&, TcpListener&) noexcept;
    TcpListener& operator=(TcpListener&&) noexcept;
    bool accept(TcpStream& output_stream);    
private:
    int socket_;
};

void swap(TcpListener&, TcpListener&) noexcept;

}
#endif // TWISTER_IO_TCP_LISTENER_HPP_INCLUDED
