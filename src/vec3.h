#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

class vec_index {
    private:
        unsigned char value;

    public:
        consteval vec_index(int i) : value(i) {
            if (i < 0 || i >= 3) {
                throw std::out_of_range("Index out of bounds");
            }
        }

        constexpr operator size_t() const { return value; }
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
    return v * (1/t);
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

// Type aliases
using point3 = vec3<double>;
using direction3 = vec3<double>;

#endif
