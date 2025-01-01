#ifndef SPHERE_H
#define SPHERE_H

#include <cmath>
#include <memory>

#include "ray.h"
#include "hittable.h"
#include "material.h"
#include "raytracing.h"

class sphere : public Hittable {
    private:
        Ray cent;
        double rad;
        std::shared_ptr<Material> mat;
        AABB bbox;

        static void get_sphere_uv(const point3& p, double& u, double& v) {
            const auto theta = std::acos(-p.y());
            const auto phi = std::atan2(-p.z(), p.x()) + pi;

            u = phi / (2 * pi);
            v = theta / pi;
        }

    public:
        // Stationary sphere
        sphere(
            const point3& center,
            double radius,
            std::shared_ptr<Material> m
        ) : cent(center, direction3(0, 0, 0)),
            rad{std::fmax(radius, 0)},
            mat{m} {
                const auto radius_vec = direction3(radius, radius, radius);
                bbox = AABB(center - radius_vec, center + radius_vec);
            }

        // Moving sphere
        sphere(
            const point3& c0,
            const point3& c1,
            double radius,
            std::shared_ptr<Material> m
        ) : cent(c0, c1 - c0), rad{std::fmax(radius, 0)}, mat{m} {
            const auto radius_vec = direction3(radius, radius, radius);
            AABB box0 = AABB(c0 - radius_vec, c0 + radius_vec);
            AABB box1 = AABB(c1 - radius_vec, c1 + radius_vec);
            bbox = AABB(box0, box1);
        }

        const Ray& center() const { return cent; }
        double radius() const { return rad; }

        bool hit(
            const Ray& r,
            interval_d t,
            HitRecord& rec
        ) const override {
            const auto current_center = center().at(r.time());
            // Ray-sphere interaction quantities
            const direction3 oc = r.origin() - current_center;
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius() * radius();

            // Quadratic equation to determine if Ray intersects sphere
            const auto discriminant = h * h - a * c;
            if (discriminant < 0) {
                return false;
            }
            const auto sqrtd = std::sqrt(discriminant);

            // Determine which (if any) root is valid by the t-Interval
            auto root = (-h - sqrtd) / a;
            if (!t.surrounds(root)) {
                root = (-h + sqrtd) / a;
                if (!t.surrounds(root)) {
                    return false;
                }
            }

            // Set hit record
            rec.t = root;
            rec.p = r.at(root);
            const auto outward_normal = (rec.p - current_center) / radius();
            rec.set_face_normal(r, outward_normal);
            rec.mat = mat;
            get_sphere_uv(outward_normal, rec.u, rec.v);
            return true;
        }

        AABB bounding_box() const override { return bbox; }
};

#endif
