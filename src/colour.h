#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"

class Colour : public Vec3<double> {
    private:
        static constexpr int colour_multiplier { 256 };
        static constexpr interval_d intensity { 0.0, 0.999 };

        static constexpr double linear_to_gamma(double linear) {
            if (linear > 0) {
                return std::sqrt(linear);
            }
            return 0;
        }

        static constexpr double gamma_correct(double linear) {
            return colour_multiplier * intensity.clamp(linear_to_gamma(linear));
        }

    public:
        Colour() = default;
        Colour(double r, double g, double b) : Vec3<double>(r, g, b) {}
        Colour(const Vec3<double>& v) : Vec3<double>(v) {}

        Colour gamma_correct() const {
            return Colour{
                gamma_correct(x()),
                gamma_correct(y()),
                gamma_correct(z())
            };
        }
};

#endif
