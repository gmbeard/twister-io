#ifndef TWISTER_TASKS_CONCEPTS_HPP_INCLUDED
#define TWISTER_TASKS_CONCEPTS_HPP_INCLUDED

#include <type_traits>

namespace twister::tasks::concepts {

template<typename T>
concept bool AsyncTask = requires(T a) {
    { a() } -> bool;
    T { std::declval<T>() };
};

}

#endif // TWISTER_TASKS_CONCEPTS_HPP_INCLUDED
