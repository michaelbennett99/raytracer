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
        const Ray& r,
        IntervalD t,
        HitRecord& rec
    ) const = 0;

    virtual AABB bounding_box() const = 0;

    static bool is_front_face(
        const Ray& r, const Direction3& outward_normal
    ) {
        return dot(r.direction(), outward_normal) < 0;
    }

    static Direction3 direct_normal(
        bool front_face, const Direction3& outward_normal
    ) {
        return front_face ? outward_normal : -outward_normal;
    }
};

class Material;

struct HitRecord {
    Point3 p {};
    double t {};
    bool front_face {};
    Direction3 normal {};
    std::shared_ptr<Material> mat {};
    // Texture coordinates
    double u {};
    double v {};

    HitRecord() = default;
    HitRecord(
        const Ray& r,
        const Point3& p,
        const Direction3& outward_normal,
        double t
    )
        : p{ p }
        , t{ t }
        , front_face{ Hittable::is_front_face(r, outward_normal) }
        , normal{ Hittable::direct_normal(front_face, outward_normal) }
    {}

    void set_face_normal(const Ray& r, const Direction3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

#endif
