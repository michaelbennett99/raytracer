#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "colour.h"

constexpr int colour_multiplier = 256;

using colour = vec3<double>;

inline double linear_to_gamma(double linear) {
    if (linear > 0) {
        return std::sqrt(linear);
    }
    return 0;
}

void write_colour(std::ostream& out, const colour& c) {
    auto r = linear_to_gamma(c.x());
    auto g = linear_to_gamma(c.y());
    auto b = linear_to_gamma(c.z());

    static const interval intensity(0.000, 0.999);

    int rb = static_cast<int>(256 * intensity.clamp(r));
    int gb = static_cast<int>(256 * intensity.clamp(g));
    int bb = static_cast<int>(256 * intensity.clamp(b));

    out << rb << ' ' << gb << ' ' << bb << '\n';
}

#endif
