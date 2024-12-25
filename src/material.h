#ifndef MATERIAL_H
#define MATERIAL_H

#include "colour.h"
#include "hittable.h"

class material {
    public:
        virtual ~material() = default;

        virtual bool scatter(
            const ray& r_in,
            const hit_record& rec,
            colour& attenuation,
            ray& scattered
        ) const {
            return false;
        }
};

class lambertian : public material {
    private:
        colour albedo;

    public:
        lambertian(const colour& a) : albedo(a) {}

        bool scatter(
            const ray& r_in,
            const hit_record& rec,
            colour& attenuation,
            ray& scattered
        ) const override {
            auto scatter_direction = rec.normal + random_unit_vector();
            if (scatter_direction.near_zero()) {
                scatter_direction = rec.normal;
            }
            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }
};

#endif
