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
        const Colour albedo;

    public:
        SolidColour() = delete;
        explicit SolidColour(const Colour& c) : albedo { c } {}
        SolidColour(double red, double green, double blue)
            : SolidColour { Colour { red, green, blue } }
        {}

        virtual Colour value(
            double u, double v, const Point3& p
        ) const override {
            return albedo;
        }
};

class CheckerTexture : public Texture {
    private:
        const double inv_scale;
        const std::shared_ptr<Texture> even;
        const std::shared_ptr<Texture> odd;

    public:
        CheckerTexture(
            double scale,
            std::shared_ptr<Texture> even,
            std::shared_ptr<Texture> odd
        )
            : inv_scale { 1.0 / scale }
            , even { even }
            , odd { odd }
        {}

        CheckerTexture(double scale, const Colour& c1, const Colour& c2)
            : CheckerTexture(
                scale,
                std::make_shared<SolidColour>(c1),
                std::make_shared<SolidColour>(c2)
            ) {}

        Colour value(double u, double v, const Point3& p) const override {
            const auto x { static_cast<int>(std::floor(inv_scale * p.x())) };
            const auto y { static_cast<int>(std::floor(inv_scale * p.y())) };
            const auto z { static_cast<int>(std::floor(inv_scale * p.z())) };

            const bool is_even { (x + y + z) % 2 == 0 };
            return is_even ? even->value(u, v, p) : odd->value(u, v, p);
        }
};

class ImageTexture : public Texture {
    private:
        const rtw_image img;

    public:
        ImageTexture() = delete;
        explicit ImageTexture(const char* filename) : img { filename } {}

        Colour value(double u, double v, const Point3& p) const override {
            if (img.height() <= 0) return Colour(0, 1, 1);

            u = IntervalD(0, 1).clamp(u);
            v = 1.0 - IntervalD(0, 1).clamp(v); // Flip V to image coordinates

            const auto i { static_cast<int>(u * img.width()) };
            const auto j { static_cast<int>(v * img.height()) };
            const auto pixel { img.pixel_data(i, j) };

            const auto colour_scale { 1.0 / 255.0 };
            return Colour {
                pixel[0] * colour_scale,
                pixel[1] * colour_scale,
                pixel[2] * colour_scale
            };
        }
};

class NoiseTexture : public Texture {
    private:
        const Perlin noise {};
        const double scale;

    public:
        NoiseTexture() = delete;
        explicit NoiseTexture(double scale) : scale { scale } {}

        Colour value(double u, double v, const Point3& p) const override {
            return Colour { 1, 1, 1 }
                * 0.5 * (1 + std::sin(scale * p.z() + 10 * noise.turb(p, 7)));
        }
};

#endif
