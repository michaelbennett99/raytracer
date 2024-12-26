#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
    private:
        point3 orig;
        direction3 dir;
        double tm;
    public:
        ray() {}

        ray(
            const point3& origin,
            const direction3& direction,
            double time = 0.0
        ) : orig{origin}, dir{direction}, tm{time} {}

        const auto& origin() const { return orig; }
        const auto& direction() const { return dir; }

        double time() const { return tm; }

        point3 at(double t) const { return orig + t * dir; }
};

#endif
