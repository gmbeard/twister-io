#include "chat_server.hpp"
#include "twister/event_loop.hpp"
#include "twister/tasks/concepts.hpp"
#include <cassert>
#include <utility>
#include <algorithm>

namespace {

struct ByteSpan {

    using value_type = uint8_t;
    using reference = value_type&;
    using const_reference = value_type const&;
    using pointer = value_type*;
    using const_pointer = value_type const*;
    using iterator = pointer;
    using const_iterator = const_pointer;

    ByteSpan() noexcept :
        data_ { nullptr }
    ,   size_ { 0 }
    { }

    explicit ByteSpan(char const* str) noexcept :
        data_ { nullptr }
    ,   size_ { 0 }
    {
        assert(str && "ByteSpan::ByteSpan(): str == nullptr");
        data_ = reinterpret_cast<const_pointer>(&*str);
        size_ = std::strlen(str);
    }

    ByteSpan(uint8_t* data, size_t count) noexcept :
        data_ { data }
    ,   size_ { count }
    {
        assert(data_ && "ByteSpan::ByteSpan(): data_ == nullptr!");
    }

    ByteSpan(ByteSpan const& other) noexcept :
        data_ { other.data_ }
    ,   size_ { other.size_ }
    { }

    ByteSpan& operator=(ByteSpan const& rhs) noexcept {
        ByteSpan tmp { rhs };
        swap(*this, tmp);
        return *this;
    }

    reference operator[](size_t n) noexcept {
        assert(data_ && n < size_,
            "ByteSpan::operator[](): data_ == nullptr || size_ <= n!");
        return *(data_ + n);
    }

    const_reference operator[](size_t n) const noexcept {
        return const_cast<ByteSpan&>(*this).operator[](n);
    }

    friend void swap(ByteSpan& lhs, ByteSpan& rhs) noexcept {
        using std::swap;
        swap(lhs.data_, rhs.data_);
        swap(lhs.size_, rhs.size_);
    }

    iterator begin() noexcept { return data_; }
    const_iterator begin() const noexcept { return data_; }
    iterator end() noexcept { return data_ + size_; }
    const_iterator end() const noexcept { return data_ + size_; }

    size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return !data_ || !size_; }

    iterator find(ByteSpan const& rhs) noexcept {
        if (size() < rhs.size() || rhs.empty()) {
            return end();
        }

        return std::search(begin(), end(), rhs.begin(), rhs.end());
    }

    const_iterator find(ByteSpan const& rhs) const noexcept {
        return const_cast<ByteSpan&>(*this).find(rhs);
    }

private:
    uint8_t* data_;
    size_t size_;
};

} // End anonymous

void ChatServer::broadcast_message(Message const& msg) {
    SharedMessage shared_msg = std::make_shared<Message>(msg);
    auto lock = std::lock_guard<std::mutex> { mutex_ };

    for (auto& client : connected_clients_) {
        if (auto p = client.lock()) {
            p->push(shared_msg);
        }
    }

    connected_clients_.erase(
        std::erase_if(connected_clients_.begin(),
                      connected_clients_.end(),
                      [](auto const& c) { return c.lock(); }),
        connected_clients_.end()
    );
}

void ChatServer::push_client(ClientMessageQueueWeakPtr client) {
    auto lock = std::lock_guard<std::mutex> { mutex_ };
    connected_clients_.push_back(client);
}

bool ClientMessageQueue::try_pop(SharedMessage& msg) {
    auto lock = std::lock_guard<std::mutex> { mutex_ };
    if (!messages_.size()) {
        tasks_to_wake_.push_back(twister::tasks::current_task_id);
        return false;
    }

    msg = messages_.back();
    messages_.pop_back();
    return true;
}

void ClientMessageQueue::push(SharedMessage const& msg) {

    auto lock = std::lock_guard<std::mutex> { mutex_ };
    messages_.push_back(msg);

    std::for_each(
        tasks_to_wake_.begin(), 
        tasks_to_wake_.end(), 
        [](auto const& t) { twister::trigger(t); });

    tasks_to_wake_.clear();
}

ChatClientHandler::ChatClientHandler(twister::io::TcpStream&& stream,
                                     ChatServer& srv) noexcept :
    stream_ { std::move(stream) }
,   srv_ { srv }
{ }

template<twister::tasks::concepts::AsyncTask First,
         twister::tasks::concepts::AsyncTask Second>
EitherResult either(First&& first, Second&& second) {
    if (std::forward<First>(first)()) {
        return EitherResult::First;
    }
    else if (std::forward<Second>(second)()) {
        return EitherResult::Second;
    }

    return EitherResult::None;
}

bool ChatClientHandler::operator()() {

    // 1. On first invocation, call srv_.push_client(message_queue_)
    // 2. On every other invocation:
    // LOOP
    //  auto either_result = either(
    //      [&msg, this] { return message_queue_->try_pop(msg); },
    //      [&, this] { 
    //          return stream_.read(buffer, to_read, read);
    //      }
    //  )
    //
    //  switch (either_result) {
    //      case EitherResult::First:
    //          break;
    //      case EitherResult::Second:
    //          break;
    //      default:
    //          break;
    //  }

    return false;
}
