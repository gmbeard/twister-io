add_executable(
    tcp_listen_example
    tcp_listen_example.cpp
)

target_link_libraries(
    tcp_listen_example
    PRIVATE
        Twister::twister
)
