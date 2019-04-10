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

### Non-RPC based servers (E.g. group chat / IRC)

These types of servers require tasks to be awoken by events other than I/O. For example, we have 3 clients `A`, `B`, and `C`, connected via 3 handlers, `hA`, `hB`, `hC`. `hA` is notified of incoming data on its socket, it reads the data and determines that it has recieved a full message object. The message object must now be sent to clients `B` and `C`. `hB` and `hC` would normally only be awoken by activity on their own sockets, so `hA`, which has received the message, must now distribute the message to handlers `hB` and `hC` and somehow *wake* them so that they can perform I/O to their associated clients' connections. 

```

enum class EitherResult {
    First,
    Second,
    Niether
};

template<AsyncTask First, AsyncTask Second>
EitherResult either(First&& f, Second&& s) {
    if (std::forward<First>(f)()) {
        return EitherResult::First;
    }
    else if (std::forward<Second>(s)()) {
        return EitherResult::Second;
    }

    return EitherResult::Niether;
}

struct Message { ... };

struct MessagesFromOtherClients { 
    bool poll(); 
private:
    std::vector<std::shared_ptr<Message>> messages_;
};

// In handler...
size_t read = 0;
Message msg;
auto r = either(
    [&read]() {
        socket.read(); 
    },
    [&msg]() {
        msgs_from_others.poll();
    }
);

switch (r) {
    case EitherResult::First:
        ...
        break;
    case EitherResult::Second:
        ...
        break;
    default:
        // niether happened
        break;
}

if (either.holds_alternative<PolledFirst>()) {
    // ... Handle socket.read
}
else if (either.holds_alternative<PolledSecond>()) {
    // ... Handle received message from other client
}

```
