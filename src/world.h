#ifndef WORLD_H
#define WORLD_H

#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "ray.h"
#include "texture.h"

class World {
    private:
        std::shared_ptr<hittable_list> world_;
        colour background_;

    public:
        World(
            const hittable_list& world,
            colour background = colour(1e-3, 1e-3, 1e-3)
        ) : world_{ std::make_shared<hittable_list>(world) },
            background_{ background } {}

        colour ray_colour(
            const ray& r, int depth
        ) const {
            if (depth <= 0) return background_;

            hit_record rec;

            if (!world_->hit(r, interval_d{0.001, infinity_d}, rec)) {
                return background_;
            }

            ray scattered;
            colour attenuation;
            colour emitted = rec.mat->emitted(rec.u, rec.v, rec.p);

            if (!rec.mat->scatter(r, rec, attenuation, scattered)) {
                return emitted;
            }

            return emitted
                + attenuation * ray_colour(scattered, depth - 1);
        }
};

#endif
