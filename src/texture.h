#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>

#include "vec3.h"
#include "colour.h"

class texture {
    public:
        virtual ~texture() = default;

        virtual colour value(double u, double v, const point3& p) const = 0;
};

class solid_colour : public texture {
    private:
        colour albedo;

    public:
        solid_colour(const colour& c) : albedo(c) {}

        solid_colour(double red, double green, double blue)
            : solid_colour(colour(red, green, blue)) {}

        virtual colour value(
            double u, double v, const point3& p
        ) const override {
            return albedo;
        }
};

class checker_texture : public texture {
    private:
        double inv_scale;
        std::shared_ptr<texture> even;
        std::shared_ptr<texture> odd;

    public:
        checker_texture(
            double scale,
            std::shared_ptr<texture> even,
            std::shared_ptr<texture> odd
        )
            : inv_scale(1.0 / scale)
            , even(even)
            , odd(odd)
        {}

        checker_texture(double scale, const colour& c1, const colour& c2)
            : checker_texture(
                scale,
                std::make_shared<solid_colour>(c1),
                std::make_shared<solid_colour>(c2)
            ) {}

        colour value(double u, double v, const point3& p) const override {
            const auto x = int(std::floor(inv_scale * p.x()));
            const auto y = int(std::floor(inv_scale * p.y()));
            const auto z = int(std::floor(inv_scale * p.z()));

            const bool is_even = (x + y + z) % 2 == 0;
            return is_even ? even->value(u, v, p) : odd->value(u, v, p);
        }
};

#endif
