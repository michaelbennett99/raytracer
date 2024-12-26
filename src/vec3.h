#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

#include "random.h"
#include "raytracing.h"

class vec_index {
    private:
        unsigned char value;

    public:
        constexpr vec_index(int i) : value(i) {
            if (i < 0 || i >= 3) {
                throw std::out_of_range("Index out of bounds");
            }
        }

        constexpr operator size_t() const { return value; }

        static vec_index gen_rand() {
            return int(gen_rand::random_double(0, 3));
        }
};

template <typename T>
class vec3 {
    private:
        T e[3];

    public:
        vec3() : e{0, 0, 0} {}
        vec3(T e0, T e1, T e2) : e{e0, e1, e2} {}

        T x() const { return e[0]; }
        T y() const { return e[1]; }
        T z() const { return e[2]; }

        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        T operator[](vec_index i) const { return e[i]; }
        T& operator[](vec_index i) { return e[i]; }

        vec3& operator+=(const vec3 v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator*=(const T t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(const T t) {
            return *this *= 1/t;
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

        static vec3<T> random() {
            return vec3<T>(
                gen_rand::gen_rand<T>(),
                gen_rand::gen_rand<T>(),
                gen_rand::gen_rand<T>()
            );
        }

        static vec3<T> random(T min, T max) {
            return vec3<T>(
                gen_rand::gen_rand<T>(min, max),
                gen_rand::gen_rand<T>(min, max),
                gen_rand::gen_rand<T>(min, max)
            );
        }
};

// Vector utility functions

template <typename T>
inline std::ostream& operator<<(std::ostream& out, const vec3<T>& v) {
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

template <typename T>
inline vec3<T> operator+(const vec3<T>& u, const vec3<T>& v) {
    return vec3<T>(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

template <typename T>
inline vec3<T> operator-(const vec3<T>& u, const vec3<T>& v) {
    return vec3<T>(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

template <typename T, typename U>
inline vec3<T> operator*(const vec3<T>& v, U t) {
    return vec3<T>(v[0] * t, v[1] * t, v[2] * t);
}

template <typename T, typename U>
inline vec3<T> operator*(U t, const vec3<T>& v) {
    return v * t;
}

template <typename T>
inline vec3<T> operator*(const vec3<T>& u, const vec3<T>& v) {
    return vec3<T>(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

template <typename T, typename U>
inline vec3<T> operator/(vec3<T> v, U t) {
    return v * (1/static_cast<T>(t));
}

template <typename T>
inline T dot(const vec3<T>& u, const vec3<T>& v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

template <typename T>
inline vec3<T> cross(const vec3<T>& u, const vec3<T>& v) {
    return vec3<T>(
        u[1] * v[2] - u[2] * v[1],
        u[2] * v[0] - u[0] * v[2],
        u[0] * v[1] - u[1] * v[0]
    );
}

template <typename T>
inline vec3<T> unit_vector(const vec3<T>& v) {
    return v / v.length();
}

inline vec3<double> random_unit_vector() {
    const auto theta = gen_rand::random_double(0, pi);
    const auto phi = gen_rand::random_double(0, 2 * pi);
    const auto x = std::sin(theta) * std::cos(phi);
    const auto y = std::sin(theta) * std::sin(phi);
    const auto z = std::cos(theta);
    return vec3<double>(x, y, z);
}

inline vec3<double> random_on_hemisphere(const vec3<double>& normal) {
    const auto on_unit_sphere = random_unit_vector();
    return dot(on_unit_sphere, normal) > 0.0
        ? on_unit_sphere
        : -on_unit_sphere;
}

inline vec3<double> random_in_unit_disk() {
    const auto angle = gen_rand::random_double(0, 2 * pi);
    const auto r = gen_rand::random_double(0, 1);
    const auto x = r * std::cos(angle);
    const auto y = r * std::sin(angle);
    return vec3<double>(x, y, 0);
}

template <typename T>
inline vec3<T> reflect(const vec3<T>& v, const vec3<T>& n) {
    return v - 2 * dot(v, n) * n;
}

template <typename T>
inline vec3<T> refract(
    const vec3<T>& uv, const vec3<T>& n, double etai_over_etat
) {
    const auto cos_theta = std::fmin(dot(-uv, n), 1.0);
    const auto r_out_perp = etai_over_etat * (uv + cos_theta * n);
    const auto r_out_parallel = -std::sqrt(1.0 - r_out_perp.length_squared())
        * n;
    return r_out_perp + r_out_parallel;
}

// Type aliases
using point3 = vec3<double>;
using direction3 = vec3<double>;

#endif
