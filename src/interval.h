#ifndef INTERVAL_H
#define INTERVAL_H

#include <limits>

#include "raytracing.h"

template <typename T>
class interval {
    public:
        T min;
        T max;

        // Default interval: empty
        interval() : min{infinity<T>}, max{-infinity<T>} {}

        interval(T _min, T _max) : min{_min}, max{_max} {}

        interval(const interval<T>& a, const interval<T>& b) {
            min = std::fmin(a.min, b.min);
            max = std::fmax(a.max, b.max);
        }

        T size() const {
            return max - min;
        }

        bool contains(T x) const {
            return min <= x && x <= max;
        }

        bool surrounds(T x) const {
            return min < x && x < max;
        }

        T clamp(T x) const {
            if (x < min) return min;
            if (x > max) return max;
            return x;
        }

        interval<T> expand(T delta) const {
            const auto padding = delta / 2;
            return interval<T>(min - padding, max + padding);
        }

        static const interval empty, universe;

};

template <typename T>
inline interval<T> operator+(const interval<T>& a, T b) {
    return interval<T>(a.min + b, a.max + b);
}

template <typename T>
inline interval<T> operator+(T b, const interval<T>& a) {
    return interval<T>(a.min + b, a.max + b);
}

template <typename T>
const interval<T> interval<T>::empty = interval<T>(infinity<T>, -infinity<T>);

template <typename T>
const interval<T> interval<T>::universe = interval<T>(
    -infinity<T>, infinity<T>
);

using interval_d = interval<double>;

#endif
