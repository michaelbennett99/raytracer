#ifndef SPHERE_H
#define SPHERE_H

#include <cmath>
#include <memory>

#include "ray.h"
#include "hittable.h"
#include "material.h"
#include "raytracing.h"

class Sphere : public Hittable {
    private:
        Ray cent;
        double rad;
        std::shared_ptr<Material> mat;
        AABB bbox;

        static void get_sphere_uv(const Point3& p, double& u, double& v) {
            const auto theta = std::acos(-p.y());
            const auto phi = std::atan2(-p.z(), p.x()) + pi;

            u = phi / (2 * pi);
            v = theta / pi;
        }

    public:
        // Stationary Sphere
        Sphere(
            const Point3& center,
            double radius,
            std::shared_ptr<Material> m
        ) : cent { center, Direction3 { 0, 0, 0 } },
            rad{ std::fmax(radius, 0) },
            mat{ m },
            bbox { center - radius, center + radius }
        {}

        // Moving Sphere
        Sphere(
            const Point3& center0,
            const Point3& center1,
            double radius,
            std::shared_ptr<Material> m
        ) : cent { center0, center1 - center0 },
            rad{ std::fmax(radius, 0) },
            mat{ m },
            bbox {
                AABB{ center0 - radius, center0 + radius },
                AABB{ center1 - radius, center1 + radius }
            }
        {}

        const Ray& center() const { return cent; }
        double radius() const { return rad; }

        bool hit(
            const Ray& r,
            IntervalD t,
            HitRecord& rec
        ) const override {
            const auto current_center = center().at(r.time());
            // Ray-Sphere interaction quantities
            const Direction3 oc { r.origin() - current_center };
            const auto a { r.direction().length_squared() };
            const auto h { dot(r.direction(), oc) };
            const auto c { oc.length_squared() - radius() * radius() };

            // Quadratic equation to determine if Ray intersects Sphere
            const auto discriminant { h * h - a * c };
            if (discriminant < 0) {
                return false;
            }
            const auto sqrtd { std::sqrt(discriminant) };

            // Determine which (if any) root is valid by the t-Interval
            auto root { (-h - sqrtd) / a };
            if (!t.surrounds(root)) {
                root = (-h + sqrtd) / a;
                if (!t.surrounds(root)) {
                    return false;
                }
            }

            // Set hit record
            rec.t = root;
            rec.p = r.at(root);
            const auto outward_normal { (rec.p - current_center) / radius() };
            rec.set_face_normal(r, outward_normal);
            rec.mat = mat;
            get_sphere_uv(outward_normal, rec.u, rec.v);
            return true;
        }

        AABB bounding_box() const override { return bbox; }
};

#endif
