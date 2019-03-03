include(Testy OPTIONAL RESULT_VARIABLE Testy_INCLUDED)
if(Testy_INCLUDED STREQUAL "NOTFOUND")
    message(
        FATAL_ERROR
        "Couldn't locate Testy library. Maybe you forgot to "
        "call `git submodule update`"
    )
endif()

add_executable(
    twister_tests
    test_main.cpp
    task_proxy_tests.cpp
)

target_compile_options(
    twister_tests
    PRIVATE
        -std=c++2a -Wall -Werror -Wextra -pedantic
)

target_compile_options(
    twister_tests
    PRIVATE
        -fconcepts
)

target_link_libraries(
    twister_tests
    PRIVATE
    Twister::twister
    Testy::testy
)

add_test(
    NAME TwisterTests
    COMMAND twister_tests
)
