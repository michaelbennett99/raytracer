#ifndef UTILS_H
#define UTILS_H

#include <limits>
#include <numbers>

#include "concepts.h"

// Constants

template <Arithmetic T>
constexpr T infinity = std::numeric_limits<T>::infinity();

constexpr double infinity_d = infinity<double>;

constexpr double pi = std::numbers::pi;

// Utility functions

constexpr double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

#endif
