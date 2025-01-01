#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "random.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
    private:
        std::shared_ptr<hittable> boundary;
        double neg_inv_density;
        std::shared_ptr<material> phase_function;

    public:
        constant_medium(
            std::shared_ptr<hittable> boundary,
            double density,
            std::shared_ptr<texture> tex
        )
            : boundary{ boundary }
            , neg_inv_density{ -1 / density }
            , phase_function{ std::make_shared<isotropic>(tex) }
        {}

        constant_medium(
            std::shared_ptr<hittable> boundary,
            double density,
            const colour& albedo
        )
            : boundary{ boundary }
            , neg_inv_density{ -1 / density }
            , phase_function{ std::make_shared<isotropic>(albedo) }
        {}

        bool hit(
            const ray& r, interval_d t, hit_record& rec
        ) const override {
            hit_record rec1, rec2;

            if (!boundary->hit(r, interval_d::universe, rec1))
                return false;
            if (!boundary->hit(r, interval_d{rec1.t+0.0001, infinity_d}, rec2))
                return false;

            if (rec1.t < t.min()) rec1.t = t.min();
            if (rec2.t > t.max()) rec2.t = t.max();

            if (rec1.t >= rec2.t) return false;

            if (rec1.t < 0) rec1.t = 0;

            const auto ray_length = r.direction().length();
            const auto dist_in_boundary = (rec2.t - rec1.t) * ray_length;
            const auto hit_dist = neg_inv_density
                * std::log(gen_rand::random_double());

            if (hit_dist > dist_in_boundary) return false;

            rec.t = rec1.t + hit_dist / ray_length;
            rec.p = r.at(rec.t);

            rec.normal = direction3{1, 0, 0}; // arbitrary
            rec.front_face = true; // also arbitrary
            rec.mat = phase_function;

            return true;
        }

        AABB bounding_box() const override {
            return boundary->bounding_box();
        }
};

#endif
