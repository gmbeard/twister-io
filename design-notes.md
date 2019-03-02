Usage
-----

### A User-Defined Handler

```c++
struct EchoHandler {
    explicit EchoHandler(twister::io::TcpStream&& stream) :
        stream_ { std::move(stream) }
    { }

    auto operator()() -> bool {
        while (true) {
            size_t bytes_read, bytes_written;
            if (!stream_.read(buffer_, read))
                return false; // Not ready. Poll me again later.

            if (bytes_read == 0)
                return true; // Finished. The stream was closed by
                             // the remote host.

            if (!stream_.write(buffer_, written))
                return false; // Not ready. Poll me again later.

            if (bytes_written == 0)
                return true; // Finished. The stream was closed by
                             // the remote host.
        }
    }

private:
    std::vector<uint8_t> buffer_;
    twister::io::TcpStream stream_;
};
```

### Main

```c++
auto main() -> int {
    twister::io::TcpListener listener { "127.0.0.1", 8080 };

    twister::EventLoop loop;
    loop.run(
        twister::io::acceptor(
            std::move(listener),
            [](auto&& tcp_stream) {
                return EchoHandler { std::move(tcp_stream) }
            }
        )
    );
}
```

Internals
---------

### Concepts

```c++
namespace twister::io {

template<typename T>
concept bool AsyncTask = requires(T a) {
    { a() } -> bool;
};

template<typename T>
concept bool StreamFactory = requires(T a) {
    { a(std::declval<twister::io::TcpStream>()) } -> AsyncTask;
};

}
```

### Acceptor

```c++
namespace twister::io {

template<StreamFactory F>
auto twister::io::acceptor(TcpListener&& listener,
                           F&& factory) 
{
    return [
        listener=std::move(listener), 
        factory=std::forward<F>(factory)
    ]() {
        twister::io::TcpStream stream;
        while (true) {
            if (!listener_.accept(stream)) {
                return false; // Not ready. Poll me again later.
            }

            twister::spawn(factory_(std::move(stream)));
        }
    }
}

}
```

### Spawning New Tasks

```c++
namespace twister {

template<AsyncTask T>
auto twister::spawn(T&& task) {

    // Poll the task immediately
    if (!task()) {

        // If it requires polling again later then
        // push in onto the pending event queue...
        twister::get_current_event_loop().enqueue_task(
            twister::tasks::TaskProxy { std::forward<T>(task) });
    }
}

}
```
