#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>

#include "vec3.h"
#include "colour.h"
#include "rtw_stb_image.h"
#include "perlin.h"

class Texture {
    public:
        virtual ~Texture() = default;

        virtual Colour value(double u, double v, const Point3& p) const = 0;
};

class SolidColour : public Texture {
    private:
        Colour albedo;

    public:
        SolidColour(const Colour& c) : albedo(c) {}

        SolidColour(double red, double green, double blue)
            : SolidColour(Colour(red, green, blue)) {}

        virtual Colour value(
            double u, double v, const Point3& p
        ) const override {
            return albedo;
        }
};

class checker_texture : public Texture {
    private:
        double inv_scale;
        std::shared_ptr<Texture> even;
        std::shared_ptr<Texture> odd;

    public:
        checker_texture(
            double scale,
            std::shared_ptr<Texture> even,
            std::shared_ptr<Texture> odd
        )
            : inv_scale(1.0 / scale)
            , even(even)
            , odd(odd)
        {}

        checker_texture(double scale, const Colour& c1, const Colour& c2)
            : checker_texture(
                scale,
                std::make_shared<SolidColour>(c1),
                std::make_shared<SolidColour>(c2)
            ) {}

        Colour value(double u, double v, const Point3& p) const override {
            const auto x = int(std::floor(inv_scale * p.x()));
            const auto y = int(std::floor(inv_scale * p.y()));
            const auto z = int(std::floor(inv_scale * p.z()));

            const bool is_even = (x + y + z) % 2 == 0;
            return is_even ? even->value(u, v, p) : odd->value(u, v, p);
        }
};

class image_texture : public Texture {
    private:
        rtw_image img;

    public:
        image_texture(const char* filename) : img(filename) {}

        Colour value(double u, double v, const Point3& p) const override {
            if (img.height() <= 0) return Colour(0, 1, 1);

            u = interval_d(0, 1).clamp(u);
            v = 1.0 - interval_d(0, 1).clamp(v); // Flip V to image coordinates

            const auto i = int(u * img.width());
            const auto j = int(v * img.height());
            const auto pixel = img.pixel_data(i, j);

            const auto colour_scale = 1.0 / 255.0;
            return Colour(
                pixel[0] * colour_scale,
                pixel[1] * colour_scale,
                pixel[2] * colour_scale
            );
        }
};

class noise_texture : public Texture {
    private:
        Perlin noise;
        double scale;

    public:
        noise_texture(double scale) : scale(scale) {}

        Colour value(double u, double v, const Point3& p) const override {
            return Colour(1, 1, 1)
                * 0.5 * (1 + std::sin(scale * p.z() + 10 * noise.turb(p, 7)));
        }
};

#endif
