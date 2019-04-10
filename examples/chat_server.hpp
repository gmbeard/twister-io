#ifndef TWISTER_EXAMPLES_CHAT_SERVER_HPP_INCLUDED
#define TWISTER_EXAMPLES_CHAT_SERVER_HPP_INCLUDED

#include "twister/io/tcp_stream.hpp"
#include "twister/tasks/task_id.hpp"
#include <vector>
#include <memory>
#include <mutex>

struct ClientMessageQueue;

using Message = std::vector<uint8_t>;
using SharedMessage = std::shared_ptr<Message const>;
using ClientMessageQueuePtr = std::shared_ptr<ClientMessageQueue>;
using ClientMessageQueueWeakPtr = std::weak_ptr<ClientMessageQueue>;

struct ChatServer {
    void broadcast_message(Message const& msg);
    void push_client(ClientMessageQueueWeakPtr client);
private:
    std::mutex mutex_;
    std::vector<ClientMessageQueueWeakPtr> connected_clients_;
};

struct ClientMessageQueue {
    bool try_pop(SharedMessage& msg);
    void push(SharedMessage const& msg);
private:
    std::mutex mutex_;
    std::vector<SharedMessage> messages_;
    std::vector<twister::tasks::TaskId> tasks_to_wake_;
};

struct ChatClientHandler {
    explicit ChatClientHandler(twister::io::TcpStream&& stream,
                               ChatServer& srv) noexcept;
    bool operator()();
private:
    ClientMessageQueuePtr message_queue_;
    twister::io::TcpStream stream_;
    ChatServer& srv_;
};

#endif // TWISTER_EXAMPLES_CHAT_SERVER_HPP_INCLUDED
