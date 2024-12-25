#ifndef RANDOM_H
#define RANDOM_H

#include <random>

namespace gen_rand {
    template <typename T>
    inline T gen_rand() {
        static std::uniform_real_distribution<T> distribution(T(0), T(1));
        static std::mt19937 generator;
        return distribution(generator);
    }

    template <typename T>
    inline T gen_rand(T min, T max) {
        return min + (max - min) * gen_rand<T>();
    }

    inline double random_double() {
        return gen_rand<double>();
    }

    inline double random_double(double min, double max) {
        return gen_rand<double>(min, max);
    }
};

#endif
