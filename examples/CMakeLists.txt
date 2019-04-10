add_executable(
    tcp_listen_example
    tcp_listen_example.cpp
    echo_handler.cpp
    keep_alive.cpp
)

add_executable(
    chat_server_example
    chat_server_example.cpp
    chat_server.cpp
    keep_alive.cpp
)

target_compile_options(
    tcp_listen_example
    PRIVATE
        -Wall -Werror -Wextra -Wpedantic
)

if(TWISTER_ENABLE_ADDRESS_SANITIZER)
    target_link_libraries(
        tcp_listen_example
        PRIVATE asan
    )
endif()

target_link_libraries(
    tcp_listen_example
    PRIVATE
        Twister::twister
        pthread
)

target_compile_options(
    chat_server_example
    PRIVATE
        -Wall -Werror -Wextra -Wpedantic
)

if(TWISTER_ENABLE_ADDRESS_SANITIZER)
    target_link_libraries(
        chat_server_example
        PRIVATE asan
    )
endif()

target_link_libraries(
    chat_server_example
    PRIVATE
        Twister::twister
        pthread
)
