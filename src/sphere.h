#ifndef SPHERE_H
#define SPHERE_H

#include "ray.h"
#include "shape.h"

class sphere : public shape {
    private:
        point3 cent;
        double rad;

    public:
        sphere(point3 center, double radius) : cent(center), rad(radius) {}

        double x() const { return cent[0]; }
        double y() const { return cent[1]; }
        double z() const { return cent[2]; }
        const point3& center() const { return cent; }
        double radius() const { return rad; }

        direction3 normal(const point3& p) const {
            return unit_vector(p - cent);
        }

        std::optional<double> hit(const ray& r) const override {
            direction3 oc = r.origin() - center();
            auto a = dot(r.direction(), r.direction());
            auto b = 2.0 * dot(oc, r.direction());
            auto c = dot(oc, oc) - radius() * radius();
            auto discriminant = b * b - 4 * a * c;
            if (discriminant < 0) {
                return std::nullopt;
            }
            return (-b - std::sqrt(discriminant)) / (2.0 * a);
        }
};

#endif
