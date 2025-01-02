#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "random.h"
#include "material.h"
#include "texture.h"

class ConstantMedium : public Hittable {
    private:
        std::shared_ptr<Hittable> boundary;
        double neg_inv_density;
        std::shared_ptr<Material> phase_function;

    public:
        ConstantMedium(
            std::shared_ptr<Hittable> boundary,
            double density,
            std::shared_ptr<Texture> tex
        )
            : boundary{ boundary }
            , neg_inv_density{ -1 / density }
            , phase_function{ std::make_shared<Isotropic>(tex) }
        {}

        ConstantMedium(
            std::shared_ptr<Hittable> boundary,
            double density,
            const Colour& albedo
        )
            : boundary{ boundary }
            , neg_inv_density{ -1 / density }
            , phase_function{ std::make_shared<Isotropic>(albedo) }
        {}

        bool hit(
            const Ray& r, IntervalD t, HitRecord& rec
        ) const override {
            HitRecord rec1, rec2;

            if (!boundary->hit(r, IntervalD::universe, rec1))
                return false;
            if (!boundary->hit(r, IntervalD{rec1.t+0.0001, infinity_d}, rec2))
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

            rec.normal = Direction3{1, 0, 0}; // arbitrary
            rec.front_face = true; // also arbitrary
            rec.mat = phase_function;

            return true;
        }

        AABB bounding_box() const override {
            return boundary->bounding_box();
        }
};

#endif
