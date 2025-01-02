#ifndef RANDOM_H
#define RANDOM_H

#include <random>

#include "concepts.h"

namespace gen_rand {
    template <Arithmetic T>
    inline T gen_rand() {
        static std::uniform_real_distribution<T> distribution(T(0), T(1));
        static std::mt19937 generator;
        return distribution(generator);
    }

    template <Arithmetic T>
    inline T gen_rand(T min, T max) {
        return min + (max - min) * gen_rand<T>();
    }

    inline double random_double() {
        return gen_rand<double>();
    }

    inline double random_double(double min, double max) {
        return gen_rand<double>(min, max);
    }

    inline int random_int(int min, int max) {
        return static_cast<int>(gen_rand<double>(min, max + 1));
    }
};

#endif
