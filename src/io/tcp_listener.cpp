#include "twister/io/tcp_listener.hpp"
#include <utility>
#include <unistd.h>

#define UNUSED(var) (void)(var)

using namespace twister::io;

TcpListener::TcpListener(char const* address, uint16_t port) :
    socket_(-1)
{
    UNUSED(address);
    UNUSED(port);
}

TcpListener::TcpListener(TcpListener&& other) noexcept :
    socket_(other.socket_)
{
    other.socket_ = -1;
}

void twister::io::swap(TcpListener& lhs, TcpListener& rhs) noexcept {
    using std::swap;
    swap(lhs.socket_, rhs.socket_);
}

TcpListener& TcpListener::operator=(TcpListener&& rhs) noexcept {
    TcpListener tmp { std::move(rhs) };
    swap(tmp, *this);
    return *this;
}

TcpListener::~TcpListener() {
    ::close(socket_);
}

bool TcpListener::accept(TcpStream& output_socket) {
    UNUSED(output_socket);
    return false;
}
