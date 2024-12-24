#ifndef SHAPE_H
#define SHAPE_H

#include "ray.h"

class shape {
    public:
        virtual std::optional<double> hit(const ray& r) const = 0;
};

#endif
