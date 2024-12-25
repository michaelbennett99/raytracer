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

class metal : public material {
    private:
        colour albedo;
        double fuzz;

    public:
        metal(const colour& a, double f) : albedo(a), fuzz(f) {}

        bool scatter(
            const ray& r_in,
            const hit_record& rec,
            colour& attenuation,
            ray& scattered
        ) const override {
            const auto reflected = reflect(
                r_in.direction(),
                rec.normal
            );
            const auto fuzzed = unit_vector(reflected)
                + (fuzz * random_unit_vector());
            scattered = ray(rec.p, fuzzed);
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
};

#endif
