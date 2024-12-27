#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "ray.h"
#include "vec3.h"

class aabb {
    private:
        interval_d x_, y_, z_;

    public:
        aabb() {}
        aabb(const interval_d& _x, const interval_d& _y, const interval_d& _z)
            : x_{_x}, y_{_y}, z_{_z} {}

        aabb(const point3& a, const point3& b) :
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

        aabb(const aabb& a, const aabb& b) :
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

        bool hit(const ray& r, interval_d t) const {
            const point3& ray_origin = r.origin();
            const direction3& ray_direction = r.direction();

            for (int axis = 0; axis < 3; axis++) {
                const interval_d& ax = operator[](axis);
                const auto dir_inv = 1.0 / ray_direction[axis];

                const auto t0 = (ax.min - ray_origin[axis]) * dir_inv;
                const auto t1 = (ax.max - ray_origin[axis]) * dir_inv;

                if (t0 < t1) {
                    if (t0 > t.min) t.min = t0;
                    if (t1 < t.max) t.max = t1;
                } else {
                    if (t1 > t.min) t.min = t1;
                    if (t0 < t.max) t.max = t0;
                }

                if (t.max <= t.min) {
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

        static const aabb empty, universe;
};

const aabb aabb::empty = aabb(
    interval_d::empty, interval_d::empty, interval_d::empty
);

const aabb aabb::universe = aabb(
    interval_d::universe, interval_d::universe, interval_d::universe
);

#endif
