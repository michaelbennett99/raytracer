#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

#include "concepts.h"
#include "random.h"
#include "raytracing.h"

template <typename T>
class Vec3 {
    private:
        T e[3];

    public:
        Vec3() : e{0, 0, 0} {}
        Vec3(T e0, T e1, T e2) : e{e0, e1, e2} {}

        T x() const { return e[0]; }
        T y() const { return e[1]; }
        T z() const { return e[2]; }

        Vec3 operator-() const { return Vec3{-e[0], -e[1], -e[2]}; }
        T operator[](int i) const { return e[i]; }
        T& operator[](int i) { return e[i]; }

        Vec3& operator+=(const Vec3 v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        Vec3& operator*=(const T t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        template <Arithmetic U>
        Vec3& operator/=(const U t) {
            return *this *= 1/static_cast<T>(t);
        }

        bool operator==(const Vec3& v) const {
            return e[0] == v.e[0] && e[1] == v.e[1] && e[2] == v.e[2];
        }

        bool operator!=(const Vec3& v) const {
            return !(*this == v);
        }

        bool operator<(const Vec3& v) const {
            return e[0] < v.e[0] && e[1] < v.e[1] && e[2] < v.e[2];
        }

        bool operator>(const Vec3& v) const {
            return e[0] > v.e[0] && e[1] > v.e[1] && e[2] > v.e[2];
        }

        bool operator<=(const Vec3& v) const {
            return !(*this > v);
        }

        bool operator>=(const Vec3& v) const {
            return !(*this < v);
        }

        T sum() const {
            return e[0] + e[1] + e[2];
        }

        T length() const {
            return std::sqrt(length_squared());
        }

        T length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        bool near_zero() const {
            const auto s = 1e-8;
            return length_squared() < s;
        }

        static Vec3<T> random() {
            return Vec3<T>(
                gen_rand::gen_rand<T>(),
                gen_rand::gen_rand<T>(),
                gen_rand::gen_rand<T>()
            );
        }

        static Vec3<T> random(T min, T max) {
            return Vec3<T>(
                gen_rand::gen_rand<T>(min, max),
                gen_rand::gen_rand<T>(min, max),
                gen_rand::gen_rand<T>(min, max)
            );
        }
};

// Vector utility functions

template <typename T>
inline std::ostream& operator<<(std::ostream& out, const Vec3<T>& v) {
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

template <Arithmetic T>
inline Vec3<T> operator+(const Vec3<T>& u, const Vec3<T>& v) {
    return Vec3<T> {
        u[0] + v[0],
        u[1] + v[1],
        u[2] + v[2]
    };
}

template <Arithmetic T, Arithmetic U>
inline Vec3<T> operator+(const Vec3<T>& u, U v) {
    return Vec3<T> {
        u[0] + static_cast<T>(v),
        u[1] + static_cast<T>(v),
        u[2] + static_cast<T>(v)
    };
}

template <Arithmetic T, Arithmetic U>
inline Vec3<T> operator+(U u, const Vec3<T>& v) {
    return v + u;
}

template <Arithmetic T>
inline Vec3<T> operator-(const Vec3<T>& u, const Vec3<T>& v) {
    return u + (-v);
}

template <Arithmetic T, Arithmetic U>
inline Vec3<T> operator-(const Vec3<T>& u, U v) {
    return u + (-v);
}

template <Arithmetic T, Arithmetic U>
inline Vec3<T> operator-(U u, const Vec3<T>& v) {
    return v - u;
}

template <Arithmetic T>
inline Vec3<T> operator*(const Vec3<T>& v, const Vec3<T>& t) {
    return Vec3<T> {
        v[0] * t[0],
        v[1] * t[1],
        v[2] * t[2]
    };
}

template <Arithmetic T, Arithmetic U>
inline Vec3<T> operator*(const Vec3<T>& t, U v) {
    return Vec3<T> {
        t[0] * v,
        t[1] * v,
        t[2] * v
    };
}

template <Arithmetic T, Arithmetic U>
inline Vec3<T> operator*(U t, const Vec3<T>& v) {
    return v * t;
}

template <Arithmetic T, Arithmetic U>
inline Vec3<T> operator/(const Vec3<T>& v, U t) {
    return v * (1/static_cast<T>(t));
}

template <Arithmetic T>
inline T dot(const Vec3<T>& u, const Vec3<T>& v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

template <Arithmetic T>
inline Vec3<T> cross(const Vec3<T>& u, const Vec3<T>& v) {
    return Vec3<T> {
        u[1] * v[2] - u[2] * v[1],
        u[2] * v[0] - u[0] * v[2],
        u[0] * v[1] - u[1] * v[0]
    };
}

template <Arithmetic T>
inline Vec3<T> unit_vector(const Vec3<T>& v) {
    return v / v.length();
}

inline Vec3<double> random_unit_vector() {
    const auto theta { gen_rand::random_double(0, pi) };
    const auto phi { gen_rand::random_double(0, 2 * pi) };
    const auto sin_theta { std::sin(theta) };
    const auto x { sin_theta * std::cos(phi) };
    const auto y { sin_theta * std::sin(phi) };
    const auto z { std::cos(theta) };
    return Vec3<double> { x, y, z };
}

inline Vec3<double> random_on_hemisphere(const Vec3<double>& normal) {
    const auto on_unit_sphere { random_unit_vector() };
    return dot(on_unit_sphere, normal) > 0.0
        ? on_unit_sphere
        : -on_unit_sphere;
}

inline Vec3<double> random_in_unit_disk() {
    const auto angle { gen_rand::random_double(0, 2 * pi) };
    const auto r { gen_rand::random_double(0, 1) };
    const auto x { r * std::cos(angle) };
    const auto y { r * std::sin(angle) };
    return Vec3<double> { x, y, 0 };
}

template <Arithmetic T>
inline Vec3<T> reflect(const Vec3<T>& v, const Vec3<T>& n) {
    return v - 2 * dot(v, n) * n;
}

template <Arithmetic T>
inline Vec3<T> refract(
    const Vec3<T>& uv, const Vec3<T>& n, double etai_over_etat
) {
    const auto cos_theta { std::fmin(dot(-uv, n), 1.0) };
    const auto r_out_perp { etai_over_etat * (uv + cos_theta * n) };
    const auto r_out_parallel {
        -std::sqrt(1.0 - r_out_perp.length_squared()) * n
    };
    return r_out_perp + r_out_parallel;
}

// Type aliases
using Point3 = Vec3<double>;
using Direction3 = Vec3<double>;

#endif
