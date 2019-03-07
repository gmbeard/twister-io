#include "twister/io/tcp_stream.hpp"
#include "twister/event_loop.hpp"
#include <utility>
#include <unistd.h>
#include <system_error>

#define UNUSED(var) (void)(var)

using namespace twister::io;

TcpStream::TcpStream() noexcept :
    socket_ { -1 }
{ }

TcpStream::TcpStream(int os_socket) noexcept :
    socket_ { os_socket }
{ }

TcpStream::TcpStream(TcpStream&& other) noexcept :
    socket_ { other.socket_ }
{
    other.socket_ = -1;
}

TcpStream::~TcpStream() {
    ::close(socket_);
}

void twister::io::swap(TcpStream& lhs, TcpStream& rhs) noexcept {
    using std::swap;
    swap(lhs.socket_, rhs.socket_);
}

TcpStream& TcpStream::operator=(TcpStream&& rhs) noexcept {
    TcpStream tmp { std::move(rhs) };
    swap(tmp, *this);
    return *this;
}

bool TcpStream::read(uint8_t* buffer, size_t len, size_t& read) {
    auto s = ::read(socket_, buffer, len);
    if (0 > s) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            twister::notify(twister::NotifyEvent::Read, socket_);
            return false;
        }
        throw std::system_error { (int)errno, std::system_category() };
    }

    read = s;
    return true;
}

bool TcpStream::write(uint8_t const* buffer, size_t len, size_t& written) {
    auto s = ::write(socket_, buffer, len);
    if (0 > s) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            twister::notify(twister::NotifyEvent::Write, socket_);
            return false;
        }
        throw std::system_error { (int)errno, std::system_category() };
    }

    written = s;
    return true;
}

TcpStream twister::io::os_socket_to_stream(int socket) noexcept {
    TcpStream s;
    s.socket_ = socket;
    return s;
}
