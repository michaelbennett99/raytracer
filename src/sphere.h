#ifndef SPHERE_H
#define SPHERE_H

#include "ray.h"
#include "hittable.h"

class sphere : public hittable {
    private:
        point3 cent;
        double rad;

    public:
        sphere(point3 center, double radius)
            : cent(center), rad(std::fmax(radius, 0)) {}

        double x() const { return cent[0]; }
        double y() const { return cent[1]; }
        double z() const { return cent[2]; }
        const point3& center() const { return cent; }
        double radius() const { return rad; }

        direction3 normal(const point3& p) const {
            return unit_vector(p - cent);
        }

        std::optional<hit_record> hit(
            const ray& r, double t_min, double t_max
        ) const override {
            direction3 oc = r.origin() - center();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius() * radius();

            auto discriminant = h * h - a * c;
            if (discriminant < 0) {
                return std::nullopt;
            }

            auto sqrtd = std::sqrt(discriminant);

            auto root = (-h - sqrtd) / a;
            if (root < t_min || root > t_max) {
                root = (-h + sqrtd) / a;
                if (root < t_min || root > t_max) {
                    return std::nullopt;
                }
            }

            const auto p = r.at(root);

            return hit_record{
                .p = p,
                .normal = normal(p),
                .t = root
            };
        }
};

#endif
