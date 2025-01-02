#ifndef WORLD_H
#define WORLD_H

#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "ray.h"
#include "texture.h"

class World {
    private:
        std::shared_ptr<HittableList> world_;
        Colour background_;

    public:
        World(
            const HittableList& world,
            Colour background = Colour(1e-3, 1e-3, 1e-3)
        ) : world_{ std::make_shared<HittableList>(world) },
            background_{ background } {}

        Colour ray_colour(
            const Ray& r, int depth
        ) const {
            if (depth <= 0) return background_;

            HitRecord rec;

            if (!world_->hit(r, IntervalD{0.001, infinity_d}, rec)) {
                return background_;
            }

            Ray scattered;
            Colour attenuation;
            Colour emitted = rec.mat->emitted(rec.u, rec.v, rec.p);

            if (!rec.mat->scatter(r, rec, attenuation, scattered)) {
                return emitted;
            }

            return emitted
                + attenuation * ray_colour(scattered, depth - 1);
        }
};

#endif
