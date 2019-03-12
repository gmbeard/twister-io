#include "twister/event_loop.hpp"
#include "twister/io/tcp_listener.hpp"
#include "twister/io/tcp_stream.hpp"
#include "twister/io/acceptor.hpp"
#include <vector>
#include <cinttypes>
#include <iostream>
#include <thread>

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
        std::cerr << "Polling from " << std::this_thread::get_id() << "\n";
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

struct KeepAlive {
    KeepAlive() noexcept :
        done_ { false }
    { }

    bool operator()() noexcept {
        auto r = done_;
        done_ = !done_;

        if (r) {
            std::cerr << "Keep alive done!\n";
        }
        return r;
    }

private:
    bool done_;
};

template<typename T>
struct ThreadPool {
    explicit ThreadPool(size_t num_of_threads, T&& inner) noexcept :
        thread_count_ { num_of_threads }
    ,   inner_ { std::move(inner) }
    { }

    ThreadPool(ThreadPool&&) = default; 

    ~ThreadPool() {
        std::cerr << "ThreadPool done\n";

        for (auto&& t : threads_) {
            // This check is required because _any_ of these threads
            // may dispose of _this_ object, meaning one of the
            // threads in `threads_` may call join on itself!
            if (t.get_id() != std::this_thread::get_id()) {
                t.join();
            }
        }
    }

    bool operator()() {
        if (!threads_.size() && thread_count_) {
            twister::spawn(KeepAlive { }, keep_alive_id_);

            for (size_t i = 0; i < thread_count_; ++i) {
                threads_.emplace_back([] { 
                    std::cerr << "Starting thread " <<
                        std::this_thread::get_id() << "\n";
                    twister::current_event_loop().run(); 
                    std::cerr << "Ending thread " <<
                        std::this_thread::get_id() << "\n";
                });
            }
        }

        if (inner_()) {
            twister::trigger(keep_alive_id_);
            return true;
        }
        return false;    
    }
private:
    size_t thread_count_;
    T inner_;
    std::vector<std::thread> threads_;
    twister::tasks::TaskId keep_alive_id_;
};

template<typename T>
auto thread_pool(size_t thread_count, T&& inner) {
    return ThreadPool<std::decay_t<T>> { 
        thread_count,
        std::forward<T>(inner)
    };
}

template<twister::io::StreamFactory F>
auto limited_acceptor(twister::io::TcpListener&& listener,
                      size_t accept_count,
                      F&& fact)
{
    return [
        listener=std::move(listener),
        accept_count,
        factory=std::forward<F>(fact)
    ] () mutable {
        twister::io::TcpStream s;
        while (accept_count) {
            if (!listener.accept(s)) {
                return false;
            }
            twister::spawn(factory(std::move(s)));
            --accept_count;
        }

        return true;
    };
}

int main(int argc, char const** argv) {
    UNUSED(argc);
    UNUSED(argv);

    twister::io::TcpListener listener { "127.0.0.1", 8080 };
    twister::EventLoop loop;
    loop.run(
        thread_pool(
            10,
            limited_acceptor(
            //twister::io::acceptor(
                std::move(listener),
                2,
                [](auto&& stream) {
                    return EchoHandler { std::move(stream) };
                }
            )
        )
    );

    return 0;
}
