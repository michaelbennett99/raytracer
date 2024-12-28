#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "vec3.h"
#include "ray.h"
#include "hittable.h"

class translate : public hittable {
    private:
        std::shared_ptr<hittable> object;
        direction3 offset;
        aabb bbox;

    public:
        translate(std::shared_ptr<hittable> p, const direction3& displacement)
            : object{ p }, offset{ displacement } {
                bbox = object->bounding_box() + offset;
            }

        bool hit(const ray& r, interval_d t, hit_record& rec) const override {
            ray offset_r(r.origin() - offset, r.direction());
            if (!object->hit(offset_r, t, rec)) return false;

            rec.p += offset;
            return true;
        }

        aabb bounding_box() const override {
            return bbox;
        }
};

class rotate_y : public hittable {
    private:
        std::shared_ptr<hittable> object;
        double sin_theta;
        double cos_theta;
        aabb bbox;

        point3 rotate(const point3& p) const {
            return point3(
                cos_theta * p.x() - sin_theta * p.z(),
                p.y(),
                sin_theta * p.x() + cos_theta * p.z()
            );
        }

        point3 derotate(const point3& p) const {
            return point3(
                cos_theta * p.x() + sin_theta * p.z(),
                p.y(),
                -sin_theta * p.x() + cos_theta * p.z()
            );
        }

        void set_bounding_box() {
            bbox = object->bounding_box();

            point3 min(infinity_d, infinity_d, infinity_d);
            point3 max(-infinity_d, -infinity_d, -infinity_d);

            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    for (int k = 0; k < 2; k++) {
                        const auto x = i * bbox.x().max() + (1 - i)
                            * bbox.x().min();
                        const auto y = j * bbox.y().max() + (1 - j)
                            * bbox.y().min();
                        const auto z = k * bbox.z().max() + (1 - k)
                            * bbox.z().min();

                        const auto newx = cos_theta * x + sin_theta * z;
                        const auto newz = -sin_theta * x + cos_theta * z;

                        direction3 tester(newx, y, newz);

                        for (int c = 0; c < 3; c++) {
                            min[c] = std::min(min[c], tester[c]);
                            max[c] = std::max(max[c], tester[c]);
                        }
                    }
                }
            }

            bbox = aabb(min, max);
        }

    public:
        rotate_y(std::shared_ptr<hittable> object, double angle) : object{ object } {
            const auto radians = degrees_to_radians(angle);
            sin_theta = std::sin(radians);
            cos_theta = std::cos(radians);
            set_bounding_box();
        }

        bool hit(const ray& r, interval_d t, hit_record& rec) const override {

            // Transform ray from world space to object space
            const auto origin = rotate(r.origin());
            const auto direction = rotate(r.direction());

            ray rotated_r(origin, direction, r.time());

            // Determine if there's a hit in object space

            if (!object->hit(rotated_r, t, rec)) return false;

            // Transform intersection back to world space

            rec.p = derotate(rec.p);
            rec.normal = derotate(rec.normal);
            return true;
        }

        aabb bounding_box() const override {
            return bbox;
        }
};

#endif
