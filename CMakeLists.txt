cmake_minimum_required(VERSION 3.0)

project(
    Twister
    LANGUAGES CXX
    VERSION 0.1
)

list(
    APPEND 
    CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/submodules/testy-mctest-face/cmake
)

add_library(
    twister
    src/event_loop.cpp
    src/io/tcp_listener.cpp
    src/io/tcp_stream.cpp
    src/tasks/task_proxy.cpp
    src/tasks/task_id.cpp
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

add_library(Twister::twister ALIAS twister)

if(TWISTER_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(examples)
endif()
