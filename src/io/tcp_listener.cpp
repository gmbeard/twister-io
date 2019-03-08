#include "twister/io/tcp_listener.hpp"
#include "twister/event_loop.hpp"
#include <utility>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <system_error>
#include <stdexcept>
#include <algorithm>
#include <cstring>

#define UNUSED(var) (void)(var)

using namespace twister::io;

namespace {

template<typename F>
bool for_each_split(char const* first, 
                    char const* last, 
                    char comp, 
                    F&& f) noexcept(noexcept(f(first, last)))
{
    char const* prev_start = first;
    for (char const* curr = std::find(prev_start, last, comp); 
         curr != last; 
         curr = std::find(prev_start, last, comp)) 
    {
        if (!f(prev_start, curr)) {
            return false;
        }

        while (curr != last && *curr == comp) {
            prev_start = ++curr;
        }
    }

    if (prev_start != last) {
        if(!f(prev_start, last)) {
            return false;
        }
    }

    return true;
}

bool slice_to_octet(char const* first, 
                    char const* last, 
                    uint8_t& octet) noexcept
{
    if ((last - first) > 3 || (last - first) == 0) {
        return false;
    }

    size_t n = 0;
    for (char const* curr = first; curr != last; ++curr) {
        int c = *curr - '0';
        if (c < 0 || c > 9) {
            return false;
        }

        n = n * 10 + static_cast<size_t>(c);
        if (n > 255) {
            return false;
        }
    }

    octet = static_cast<uint8_t>(n);
    return true;
}

int create_os_socket_from_ipv4_and_port(char const* address,
                                        uint16_t port)
{
    int s = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (0 > s) {
        throw std::system_error { (int)errno, std::system_category() };
    }

    int reuse = 1;
    int err = ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (0 > err) {
        throw std::system_error { (int)errno, std::system_category() };
    }

    int flags = ::fcntl(s, F_GETFL);
    if (0 > flags) {
        throw std::system_error { (int)errno, std::system_category() };
    }

    flags |= O_NONBLOCK;

    err = ::fcntl(s, F_SETFL, flags);
    if (0 > err) {
        throw std::system_error { (int)errno, std::system_category() };
    }

    uint8_t parts[4] = { };
    size_t n = 0;
    bool result = for_each_split(
        address, 
        address + std::strlen(address), 
        '.',
        [&](auto f, auto l) {
            if (n > 3) {
                return false;
            }
            return slice_to_octet(f, l, parts[n++]);
        });

    if (!result) {
        throw std::runtime_error { "Couldn't parse IPv4 address" };
    }

    uint32_t ip_addr = 
        static_cast<uint32_t>(parts[0]) << 24 |
        static_cast<uint32_t>(parts[1]) << 16 |
        static_cast<uint32_t>(parts[2]) << 8 |
        static_cast<uint32_t>(parts[3]);

    sockaddr_in sock_addr = { };
    sock_addr.sin_port = ::htons(port);
    sock_addr.sin_addr.s_addr = ::htonl(ip_addr);

    err = ::bind(s, 
                 reinterpret_cast<sockaddr*>(&sock_addr), 
                 sizeof(sock_addr));

    if (0 > err) {
        throw std::system_error { (int)errno, std::system_category() };
    }

    err = ::listen(s, SOMAXCONN);
    if (0 > err) {
        throw std::system_error { (int)errno, std::system_category() };
    }

    return s;
}

} // End anonymous

TcpListener::TcpListener(char const* address, uint16_t port) :
    socket_ { create_os_socket_from_ipv4_and_port(address, port) }
{ }

TcpListener::TcpListener(TcpListener&& other) noexcept :
    socket_(other.socket_)
{
    other.socket_ = -1;
}

TcpListener::~TcpListener() {
    ::close(socket_);
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

bool TcpListener::accept(TcpStream& output_socket) {
    auto s = ::accept(socket_, nullptr, nullptr);
    if (0 > s) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            twister::notify(twister::NotifyEvent::Read, socket_);
            return false;
        }
        throw std::system_error { (int)errno, std::system_category() };
    }

    int flags = ::fcntl(s, F_GETFL);
    if (0 > flags) {
        throw std::system_error { (int)errno, std::system_category() };
    }

    flags |= O_NONBLOCK;
    int err = ::fcntl(s, F_SETFL, flags);
    if (0 > err) {
        throw std::system_error { (int)errno, std::system_category() };
    }

    TcpStream new_stream { s };
    swap(new_stream, output_socket);
    return true;
}
