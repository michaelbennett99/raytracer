#ifndef CONCEPTS_H
#define CONCEPTS_H

#include <concepts>

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

#endif
