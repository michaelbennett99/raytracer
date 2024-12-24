#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "vec3.h"

struct hit_record;

class hittable {
    public:
        virtual ~hittable() = default;

        virtual bool hit(
            const ray& r,
            double t_min,
            double t_max,
            hit_record& rec
        ) const = 0;

        static bool is_front_face(
            const ray& r, const direction3& outward_normal
        ) {
            return dot(r.direction(), outward_normal) < 0;
        }

        static direction3 direct_normal(
            bool front_face, const direction3& outward_normal
        ) {
            return front_face ? outward_normal : -outward_normal;
        }
};

struct hit_record {
    point3 p;
    double t;
    bool front_face;
    direction3 normal;

    hit_record(
        const ray& r,
        const point3& p,
        const direction3& outward_normal,
        double t
    )
        : p{ p }
        , t{ t }
        , front_face{ hittable::is_front_face(r, outward_normal) }
        , normal{ hittable::direct_normal(front_face, outward_normal) }
    {}

    // Add default constructor since we'll use it as an outparam
    hit_record() = default;

    void set_face_normal(const ray& r, const direction3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

#endif
