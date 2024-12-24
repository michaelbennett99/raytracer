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

        T size() const {
            return max - min;
        }

        bool contains(T x) const {
            return min <= x && x <= max;
        }

        bool surrounds(T x) const {
            return min < x && x < max;
        }

        static const interval empty, universe;

};

template <typename T>
const interval<T> interval<T>::empty = interval<T>(infinity<T>, -infinity<T>);

template <typename T>
const interval<T> interval<T>::universe = interval<T>(
    -infinity<T>, infinity<T>
);

using interval_d = interval<double>;

#endif
