#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
    private:
        point3 orig;
        direction3 dir;
    public:
        ray() {}

        ray(const point3& origin, const direction3& direction) :
            orig(origin), dir(direction) {}

        const auto& origin() const { return orig; }
        const auto& direction() const { return dir; }

        point3 at(double t) const { return orig + t * dir; }
};

#endif
