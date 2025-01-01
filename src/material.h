#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

#include "colour.h"
#include "hittable.h"
#include "texture.h"

class Material {
    public:
        virtual ~Material() = default;

        virtual bool scatter(
            const Ray& r_in,
            const HitRecord& rec,
            Colour& attenuation,
            Ray& scattered
        ) const {
            return false;
        }

        virtual Colour emitted(double u, double v, const Point3& p) const {
            return Colour(0, 0, 0);
        }
};

class Lambertian : public Material {
    private:
        std::shared_ptr<Texture> tex;

    public:
        Lambertian(const Colour& a) : tex(std::make_shared<SolidColour>(a)) {}
        Lambertian(std::shared_ptr<Texture> tex) : tex(tex) {}

        bool scatter(
            const Ray& r_in,
            const HitRecord& rec,
            Colour& attenuation,
            Ray& scattered
        ) const override {
            auto scatter_direction = rec.normal + random_unit_vector();
            if (scatter_direction.near_zero()) {
                scatter_direction = rec.normal;
            }
            scattered = Ray(rec.p, scatter_direction, r_in.time());
            attenuation = tex->value(rec.u, rec.v, rec.p);
            return true;
        }
};

class Metal : public Material {
    private:
        Colour albedo;
        double fuzz;

    public:
        Metal(const Colour& a, double f) : albedo(a), fuzz(f) {}

        bool scatter(
            const Ray& r_in,
            const HitRecord& rec,
            Colour& attenuation,
            Ray& scattered
        ) const override {
            const auto reflected = reflect(
                r_in.direction(),
                rec.normal
            );
            const auto fuzzed = unit_vector(reflected)
                + (fuzz * random_unit_vector());
            scattered = Ray(rec.p, fuzzed, r_in.time());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
};

class Dielectric : public Material {
    private:
        double ir;

        static double reflectance(double cosine, double ref_idx) {
            // Schlick's approximation
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            return r0 + (1 - r0) * std::pow((1 - cosine), 5);
        }

    public:
        Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        bool scatter(
            const Ray& r_in,
            const HitRecord& rec,
            Colour& attenuation,
            Ray& scattered
        ) const override {
            attenuation = Colour(1.0, 1.0, 1.0);
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

            scattered = Ray(rec.p, direction, r_in.time());
            return true;
        }
};

class DiffuseLight : public Material {
    private:
        std::shared_ptr<Texture> tex;

    public:
        DiffuseLight(std::shared_ptr<Texture> tex) : tex(tex) {}
        DiffuseLight(const Colour& emit)
            : tex(std::make_shared<SolidColour>(emit)) {}

        Colour emitted(double u, double v, const Point3& p) const override {
            return tex->value(u, v, p);
        }
};

class Isotropic : public Material {
    private:
        std::shared_ptr<Texture> tex;

    public:
        Isotropic(const Colour& albedo)
            : tex(std::make_shared<SolidColour>(albedo)) {}
        Isotropic(std::shared_ptr<Texture> tex) : tex(tex) {}

        bool scatter(
            const Ray& r_in,
            const HitRecord& rec,
            Colour& attenuation,
            Ray& scattered
        ) const override {
            scattered = Ray(rec.p, random_unit_vector(), r_in.time());
            attenuation = tex->value(rec.u, rec.v, rec.p);
            return true;
        }
};

#endif
