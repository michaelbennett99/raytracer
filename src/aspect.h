#ifndef ASPECT_H
#define ASPECT_H

#include "vec3.h"

constexpr int height(int width, double aspect_ratio) {
    return static_cast<int>(width / aspect_ratio);
};

constexpr double aspect_ratio(int width, int height) {
    return static_cast<double>(width) / height;
};

#endif
