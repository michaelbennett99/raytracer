#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

constexpr double colour_multiplier = 255.999;

using colour = vec3<double>;

void write_colour(std::ostream& out, const colour& c) {
    auto r = c.x();
    auto g = c.y();
    auto b = c.z();

    int rb = static_cast<int>(colour_multiplier * r);
    int gb = static_cast<int>(colour_multiplier * g);
    int bb = static_cast<int>(colour_multiplier * b);

    out << rb << ' ' << gb << ' ' << bb << '\n';
}

#endif
