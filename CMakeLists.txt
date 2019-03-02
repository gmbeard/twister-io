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

add_library(twister INTERFACE)

target_include_directories(
    twister
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_library(Twister::twister ALIAS twister)

if(TWISTER_ENABLE_TESTS)
    include(Testy)
    enable_testing()
    add_subdirectory(tests)
endif()
