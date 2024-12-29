#ifndef INTERVAL_H
#define INTERVAL_H

#include <limits>

#include "raytracing.h"

template <typename T>
class interval {
    private:
        T min_;
        T max_;

    public:

        // Default interval: empty
        constexpr interval() : min_{infinity<T>}, max_{-infinity<T>} {}

        constexpr interval(T _min, T _max) : min_{_min}, max_{_max} {}

        constexpr interval(const interval<T>& a, const interval<T>& b) {
            min_ = std::fmin(a.min(), b.min());
            max_ = std::fmax(a.max(), b.max());
        }

        constexpr T size() const {
            return max_ - min_;
        }

        constexpr T min() const {
            return min_;
        }
        constexpr T max() const {
            return max_;
        }

        constexpr bool contains(T x) const {
            return min_ <= x && x <= max_;
        }

        constexpr bool surrounds(T x) const {
            return min_ < x && x < max_;
        }

        constexpr T clamp(T x) const {
            if (x < min_) return min_;
            if (x > max_) return max_;
            return x;
        }

        constexpr interval<T> expand(T delta) const {
            const auto padding = delta / 2;
            return interval<T>(min_ - padding, max_ + padding);
        }

        static const interval<T> empty;
        static const interval<T> universe;

};

template <typename T>
constexpr interval<T> operator+(const interval<T>& a, T b) {
    return interval<T>(a.min() + b, a.max() + b);
}

template <typename T>
constexpr interval<T> operator+(T b, const interval<T>& a) {
    return interval<T>(a.min() + b, a.max() + b);
}

template <typename T>
const interval<T> interval<T>::empty = interval<T>(
    infinity<T>, -infinity<T>
);

template <typename T>
const interval<T> interval<T>::universe = interval<T>(
    -infinity<T>, infinity<T>
);

using interval_d = interval<double>;

#endif
