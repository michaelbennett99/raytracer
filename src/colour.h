#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "colour.h"

constexpr int colour_multiplier = 256;

using colour = vec3<double>;

void write_colour(std::ostream& out, const colour& c) {
    auto r = c.x();
    auto g = c.y();
    auto b = c.z();

    static const interval intensity(0.000, 0.999);

    int rb = static_cast<int>(256 * intensity.clamp(r));
    int gb = static_cast<int>(256 * intensity.clamp(g));
    int bb = static_cast<int>(256 * intensity.clamp(b));

    out << rb << ' ' << gb << ' ' << bb << '\n';
}

#endif
