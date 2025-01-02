#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class Ray {
private:
    Point3 origin_ {};
    Direction3 direction_ {};
    double time_ {};
public:
    // Default constructor: a ray with zero origin, direction, and time
    Ray() = default;
    Ray(
        const Point3& origin,
        const Direction3& direction,
        double time = 0.0
    ) : origin_ { origin }, direction_ { direction }, time_ { time } {}

    const auto& origin() const { return origin_; }
    const auto& direction() const { return direction_; }

    double time() const { return time_; }

    Point3 at(double t) const { return origin_ + t * direction_; }
};

#endif
