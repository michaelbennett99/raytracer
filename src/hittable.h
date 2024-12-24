#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "vec3.h"

struct hit_record {
    point3 p;
    direction3 normal;
    double t;
};

class hittable {
    public:
        virtual ~hittable() = default;

        virtual std::optional<hit_record> hit(
            const ray& r, double t_min, double t_max
        ) const = 0;
};

#endif
