#ifndef SPHERE_H
#define SPHERE_H

#include <memory>

#include "ray.h"
#include "hittable.h"
#include "material.h"

class sphere : public hittable {
    private:
        ray cent;
        double rad;
        std::shared_ptr<material> mat;
        aabb bbox;

    public:
        // Stationary sphere
        sphere(
            const point3& center,
            double radius,
            std::shared_ptr<material> m
        ) : cent(center, direction3(0, 0, 0)),
            rad{std::fmax(radius, 0)},
            mat{m} {
                const auto radius_vec = direction3(radius, radius, radius);
                bbox = aabb(center - radius_vec, center + radius_vec);
            }

        // Moving sphere
        sphere(
            const point3& c0,
            const point3& c1,
            double radius,
            std::shared_ptr<material> m
        ) : cent(c0, c1 - c0), rad{std::fmax(radius, 0)}, mat{m} {
            const auto radius_vec = direction3(radius, radius, radius);
            aabb box0 = aabb(c0 - radius_vec, c0 + radius_vec);
            aabb box1 = aabb(c1 - radius_vec, c1 + radius_vec);
            bbox = aabb(box0, box1);
        }

        const ray& center() const { return cent; }
        double radius() const { return rad; }

        bool hit(
            const ray& r,
            interval_d t,
            hit_record& rec
        ) const override {
            const auto current_center = center().at(r.time());
            // Ray-sphere interaction quantities
            const direction3 oc = r.origin() - current_center;
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
            const auto outward_normal = (r.at(root) - current_center)
                / radius();
            rec.set(root, r, outward_normal, mat);
            return true;
        }

        aabb bounding_box() const override { return bbox; }
};

#endif
