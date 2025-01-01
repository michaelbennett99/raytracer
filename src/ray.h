#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class Ray {
private:
    Point3 orig;
    Direction3 dir;
    double tm;
public:
    Ray() {}

    Ray(
        const Point3& origin,
        const Direction3& direction,
        double time = 0.0
    ) : orig{origin}, dir{direction}, tm{time} {}

    const auto& origin() const { return orig; }
    const auto& direction() const { return dir; }

    double time() const { return tm; }

    Point3 at(double t) const { return orig + t * dir; }
};

#endif
