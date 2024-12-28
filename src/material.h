#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

#include "colour.h"
#include "hittable.h"
#include "texture.h"

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

        virtual colour emitted(double u, double v, const point3& p) const {
            return colour(0, 0, 0);
        }
};

class lambertian : public material {
    private:
        std::shared_ptr<texture> tex;

    public:
        lambertian(const colour& a) : tex(std::make_shared<solid_colour>(a)) {}
        lambertian(std::shared_ptr<texture> tex) : tex(tex) {}

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
            scattered = ray(rec.p, scatter_direction, r_in.time());
            attenuation = tex->value(rec.u, rec.v, rec.p);
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
            scattered = ray(rec.p, fuzzed, r_in.time());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
};

class dielectric : public material {
    private:
        double ir;

        static double reflectance(double cosine, double ref_idx) {
            // Schlick's approximation
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            return r0 + (1 - r0) * std::pow((1 - cosine), 5);
        }

    public:
        dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        bool scatter(
            const ray& r_in,
            const hit_record& rec,
            colour& attenuation,
            ray& scattered
        ) const override {
            attenuation = colour(1.0, 1.0, 1.0);
            const auto refraction_ratio = rec.front_face ? (1.0 / ir) : ir;
            const auto unit_direction = unit_vector(r_in.direction());

            const auto cos_theta = std::fmin(
                dot(-unit_direction, rec.normal),
                1.0
            );
            const auto sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

            const auto cannot_refract = refraction_ratio * sin_theta > 1.0;
            const auto wont_refract = reflectance(cos_theta, refraction_ratio)
                    > gen_rand::random_double();
            const auto direction = cannot_refract || wont_refract
                ? reflect(unit_direction, rec.normal)
                : refract(unit_direction, rec.normal, refraction_ratio);

            scattered = ray(rec.p, direction, r_in.time());
            return true;
        }
};

class diffuse_light : public material {
    private:
        std::shared_ptr<texture> tex;

    public:
        diffuse_light(std::shared_ptr<texture> tex) : tex(tex) {}
        diffuse_light(const colour& emit)
            : tex(std::make_shared<solid_colour>(emit)) {}

        colour emitted(double u, double v, const point3& p) const override {
            return tex->value(u, v, p);
        }
};

class isotropic : public material {
    private:
        std::shared_ptr<texture> tex;

    public:
        isotropic(const colour& albedo)
            : tex(std::make_shared<solid_colour>(albedo)) {}
        isotropic(std::shared_ptr<texture> tex) : tex(tex) {}

        bool scatter(
            const ray& r_in,
            const hit_record& rec,
            colour& attenuation,
            ray& scattered
        ) const override {
            scattered = ray(rec.p, random_unit_vector(), r_in.time());
            attenuation = tex->value(rec.u, rec.v, rec.p);
            return true;
        }
};

#endif
