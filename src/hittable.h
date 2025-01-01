#ifndef HITTABLE_H
#define HITTABLE_H

#include <memory>

#include "ray.h"
#include "vec3.h"
#include "interval.h"
#include "aabb.h"

struct HitRecord;

class Hittable {
    public:
        virtual ~Hittable() = default;

        virtual bool hit(
            const ray& r,
            interval_d t,
            HitRecord& rec
        ) const = 0;

        virtual AABB bounding_box() const = 0;

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

class Material;

struct HitRecord {
    point3 p;
    double t;
    bool front_face;
    direction3 normal;
    std::shared_ptr<Material> mat;
    // Texture coordinates
    double u;
    double v;

    HitRecord(
        const ray& r,
        const point3& p,
        const direction3& outward_normal,
        double t
    )
        : p{ p }
        , t{ t }
        , front_face{ Hittable::is_front_face(r, outward_normal) }
        , normal{ Hittable::direct_normal(front_face, outward_normal) }
    {}

    // Add default constructor since we'll use it as an outparam
    HitRecord() = default;

    void set_face_normal(const ray& r, const direction3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

#endif
