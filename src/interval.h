#ifndef INTERVAL_H
#define INTERVAL_H

#include <limits>

#include "concepts.h"
#include "raytracing.h"

template <Arithmetic T>
class Interval {
    private:
        T min_;
        T max_;

    public:

        // Default Interval: empty
        constexpr Interval() : min_{infinity<T>}, max_{-infinity<T>} {}

        constexpr Interval(T _min, T _max) : min_{_min}, max_{_max} {}

        constexpr Interval(const Interval<T>& a, const Interval<T>& b) {
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

        constexpr Interval<T> expand(T delta) const {
            const auto padding = delta / 2;
            return Interval<T>(min_ - padding, max_ + padding);
        }

        static const Interval<T> empty;
        static const Interval<T> universe;

};

template <Arithmetic T>
constexpr Interval<T> operator+(const Interval<T>& a, T b) {
    return Interval<T>(a.min() + b, a.max() + b);
}

template <Arithmetic T>
constexpr Interval<T> operator+(T b, const Interval<T>& a) {
    return Interval<T>(a.min() + b, a.max() + b);
}

template <Arithmetic T>
const Interval<T> Interval<T>::empty = Interval<T>(
    infinity<T>, -infinity<T>
);

template <Arithmetic T>
const Interval<T> Interval<T>::universe = Interval<T>(
    -infinity<T>, infinity<T>
);

using IntervalD = Interval<double>;

#endif
