#include "echo_handler.hpp"
#include <thread>

using namespace twister::examples;

EchoHandler::EchoHandler(twister::io::TcpStream&& stream) noexcept :
    buffer_(512)
,   bytes_to_write_ { 0 }
,   bytes_written_ { 0 }
,   stream_ { std::move(stream) }
{ }

bool EchoHandler::operator()() {
    while (true) {
        if (!bytes_to_write_) {
            if (!stream_.read(&buffer_[0], 
                              buffer_.size(), 
                              bytes_to_write_)) 
            {
                return false;
            }

            if (bytes_to_write_ == 0) {
                break;
            }

            bytes_written_ = 0;
        }
        else {
            size_t written = 0;
            if (!stream_.write(&buffer_[bytes_written_],
                               bytes_to_write_ - bytes_written_,
                               written))
            {
                return false;
            }

            if (written == 0) {
                break;
            }

            bytes_written_ += written;
            if (bytes_written_ == bytes_to_write_) {
                bytes_to_write_ = 0;
            }
        }
    }

    return true;
}
