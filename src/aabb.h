#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "ray.h"
#include "vec3.h"

class AABB {
    private:
        interval_d x_ {};
        interval_d y_ {};
        interval_d z_ {};
        constexpr static const double delta { 0.0001 };

    void pad_to_minimums() {
        if (x_.size() < delta) x_ = x_.expand(delta);
        if (y_.size() < delta) y_ = y_.expand(delta);
        if (z_.size() < delta) z_ = z_.expand(delta);
    }

    public:
        AABB() {}
        AABB(const interval_d& _x, const interval_d& _y, const interval_d& _z)
            : x_{_x}, y_{_y}, z_{_z} {
                pad_to_minimums();
            }

        AABB(const Point3& a, const Point3& b) :
            x_{
                a.x() <= b.x()
                ? interval_d(a.x(), b.x()) : interval_d(b.x(), a.x())
            },
            y_{
                a.y() <= b.y()
                ? interval_d(a.y(), b.y()) : interval_d(b.y(), a.y())
            },
            z_{
                a.z() <= b.z()
                ? interval_d(a.z(), b.z()) : interval_d(b.z(), a.z())
            }
            {}

        AABB(const AABB& a, const AABB& b) :
            x_{a.x(), b.x()},
            y_{a.y(), b.y()},
            z_{a.z(), b.z()}
            {}

        const interval_d& x() const { return x_; }
        const interval_d& y() const { return y_; }
        const interval_d& z() const { return z_; }

        const interval_d& operator[](int i) const {
            return (i == 0) ? x_ : (i == 1) ? y_ : z_;
        }

        bool hit(const Ray& r, interval_d t) const {
            const auto& ray_origin { r.origin() };
            const auto& ray_direction { r.direction() };

            for (int axis = 0; axis < 3; axis++) {
                const auto& ax { operator[](axis) };
                const auto dir_inv { 1.0 / ray_direction[axis] };

                const auto t0 { (ax.min() - ray_origin[axis]) * dir_inv };
                const auto t1 { (ax.max() - ray_origin[axis]) * dir_inv };

                if (t0 < t1) {
                    if (t0 > t.min()) t = interval_d{t0, t.max()};
                    if (t1 < t.max()) t = interval_d{t.min(), t1};
                } else {
                    if (t1 > t.min()) t = interval_d{t1, t.max()};
                    if (t0 < t.max()) t = interval_d{t.min(), t0};
                }

                if (t.max() <= t.min()) {
                    return false;
                }
            }
            return true;
        }

        int longest_axis() const {
            return x().size() > y().size()
                ? x().size() > z().size()
                    ? 0 : 2
                : y().size() > z().size()
                    ? 1 : 2;
        }

        static const AABB empty, universe;
};

inline AABB operator+(const AABB& a, const Direction3& b) {
    return AABB(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
}

inline AABB operator+(const Direction3& b, const AABB& a) {
    return a + b;
}

const AABB AABB::empty {
    interval_d::empty, interval_d::empty, interval_d::empty
};

const AABB AABB::universe {
    interval_d::universe, interval_d::universe, interval_d::universe
};

#endif
