#ifndef SPHERE_H
#define SPHERE_H

#include <memory>

#include "ray.h"
#include "hittable.h"
#include "material.h"

class sphere : public hittable {
    private:
        point3 cent;
        double rad;
        std::shared_ptr<material> mat;

    public:
        sphere(point3 center, double radius, std::shared_ptr<material> m)
            : cent(center), rad(std::fmax(radius, 0)), mat(m) {}

        double x() const { return cent[0]; }
        double y() const { return cent[1]; }
        double z() const { return cent[2]; }
        const point3& center() const { return cent; }
        double radius() const { return rad; }

        direction3 normal(const point3& p) const {
            return unit_vector(p - cent);
        }

        bool hit(
            const ray& r,
            interval_d t,
            hit_record& rec
        ) const override {
            // Ray-sphere interaction quantities
            direction3 oc = r.origin() - center();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius() * radius();

            // Quadratic equation to determine if ray intersects sphere
            const auto discriminant = h * h - a * c;
            if (discriminant < 0) {
                return false;
            }
            const auto sqrtd = std::sqrt(discriminant);

            // Determine which (if any) root is valid by the t-interval
            auto root = (-h - sqrtd) / a;
            if (!t.surrounds(root)) {
                root = (-h + sqrtd) / a;
                if (!t.surrounds(root)) {
                    return false;
                }
            }

            // Set hit record
            rec.set(root, r, normal(r.at(root)), mat);
            return true;
        }
};

#endif
