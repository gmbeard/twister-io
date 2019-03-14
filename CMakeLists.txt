cmake_minimum_required(VERSION 3.0)

project(
    Twister
    LANGUAGES CXX
    VERSION 0.1
)

option(
    TWISTER_ENABLE_THREAD_SANITIZER
    "Enable thread sanitizer for ${PROJECT_NAME}"
    OFF
)

option(
    TWISTER_ENABLE_ADDRESS_SANITIZER
    "Enable address sanitizer for ${PROJECT_NAME}"
    OFF
)

list(
    APPEND 
    CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/submodules/testy-mctest-face/cmake
)

add_library(
    twister
    src/event_loop.cpp
    src/event_trigger.cpp
    src/sync_task_queue.cpp
    src/sync_task_list.cpp
    src/io/tcp_listener.cpp
    src/io/tcp_stream.cpp
    src/tasks/task_proxy.cpp
    src/tasks/task_id.cpp
    src/progress_guard.cpp
)

target_include_directories(
    twister
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_options(
    twister
    PUBLIC
        -std=c++2a -fconcepts
)

target_compile_options(
    twister
    PRIVATE
        -Wall -Werror -Wextra -Wpedantic
)

if(TWISTER_ENABLE_THREAD_SANITIZER)
    target_compile_options(
        twister
        PUBLIC
            -fsanitize=thread
    )

    target_link_libraries(
        twister
        PUBLIC tsan
    )
endif()

if(TWISTER_ENABLE_ADDRESS_SANITIZER)
    target_compile_options(
        twister
        PUBLIC
            -fsanitize=address
    )

    target_link_libraries(
        twister
        PUBLIC asan
    )
endif()

add_library(Twister::twister ALIAS twister)

if(TWISTER_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(examples)
endif()
