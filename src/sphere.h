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
            direction3 oc = r.origin() - center();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius() * radius();

            auto discriminant = h * h - a * c;
            if (discriminant < 0) {
                return false;
            }

            auto sqrtd = std::sqrt(discriminant);

            auto root = (-h - sqrtd) / a;
            if (!t.surrounds(root)) {
                root = (-h + sqrtd) / a;
                if (!t.surrounds(root)) {
                    return false;
                }
            }

            rec.t = root;
            rec.p = r.at(root);
            rec.set_face_normal(r, normal(rec.p));
            rec.mat = mat;
            return true;
        }
};

#endif
