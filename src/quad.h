#ifndef QUAD_H
#define QUAD_H

#include <memory>

#include "aabb.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"
#include "hittable_list.h"

// Abstract class for surfaces
class Surface : public Hittable {
    protected:
        const Point3 Q_;
        const Direction3 u_;
        const Direction3 v_;
        const Direction3 w_;
        const std::shared_ptr<Material> mat_;
        const Direction3 normal_;
        const double D_;
        // Struct to cache bounding box
        mutable struct {
            bool set { false };
            AABB bbox {};
        } bbox_ {};

        static Direction3 get_normal(const Direction3& u, const Direction3& v) {
            return cross(u, v);
        }
        static Direction3 get_w(const Direction3& u, const Direction3& v) {
            const auto n { cross(u, v) };
            return n / dot(n, n);
        }
        static Direction3 get_unit_normal(const Direction3& w) {
            return unit_vector(w);
        }
        static double get_D(const Direction3& normal, const Point3& Q) {
            return dot(normal, Q);
        }

        virtual AABB get_bounding_box() const = 0;
        virtual bool is_interior(double a, double b, HitRecord& rec) const = 0;

    public:
        Surface() = delete;
        Surface(
            const Point3& Q,
            const Direction3& u,
            const Direction3& v,
            std::shared_ptr<Material> mat
        ) : Q_ { Q }
            , u_ { u }
            , v_ { v }
            , w_ { get_w(u, v) }
            , mat_ { mat }
            , normal_ { get_unit_normal(w_) }
            , D_ { dot(normal_, Q_) }
        {}

        AABB bounding_box() const override {
            if (!bbox_.set) {
                bbox_.bbox = get_bounding_box();
                bbox_.set = true;
            }
            return bbox_.bbox;
        }

        bool hit(
            const Ray& r,
            IntervalD ray_t,
            HitRecord& rec
        ) const override {
            const auto denom { dot(normal_, r.direction()) };

            if (std::fabs(denom) < 1e-8) return false;

            const auto t { (D_ - dot(normal_, r.origin())) / denom };
            if (!ray_t.contains(t)) return false;

            const auto intersection { r.at(t) };
            const auto pos_on_plane { intersection - Q_ };
            const auto alpha { dot(w_, cross(pos_on_plane, v_)) };
            const auto beta { dot(w_, cross(u_, pos_on_plane)) };

            if (!is_interior(alpha, beta, rec)) return false;
            rec.t = t;
            rec.p = intersection;
            rec.mat = mat_;
            rec.set_face_normal(r, normal_);

            return true;
        }
};

class Quad : public Surface {
    private:
        AABB get_bounding_box() const override {
            const auto box_0 { AABB(Q_, Q_ + u_ + v_) };
            const auto box_1 { AABB(Q_ + u_, Q_ + v_) };
            return AABB(box_0, box_1);
        }

        bool is_interior(double a, double b, HitRecord& rec) const override {
            const IntervalD unit { 0, 1 };
            if (!unit.contains(a) || !unit.contains(b)) return false;
            rec.u = a;
            rec.v = b;
            return true;
        }

    public:
        Quad(
            const Point3& Q,
            const Direction3& u,
            const Direction3& v,
            std::shared_ptr<Material> mat
        ) : Surface { Q, u, v, mat } {}
};

class Triangle : public Surface {
    private:
        AABB get_bounding_box() const override {
            const auto box_0 { AABB(Q_, Q_ + u_) };
            const auto box_1 { AABB(Q_, Q_ + v_) };
            return AABB(box_0, box_1);
        }

        bool is_interior(double a, double b, HitRecord& rec) const override {
            const IntervalD unit { 0, 1 };
            if (!unit.contains(a) || !unit.contains(b)) return false;
            if (a + b > 1) return false;

            rec.u = a;
            rec.v = b;
            return true;
        }

    public:
        Triangle(
            const Point3& Q,
            const Direction3& u,
            const Direction3& v,
            std::shared_ptr<Material> mat
        ) : Surface { Q, u, v, mat } {}
};

class Ellipse : public Surface {
    private:
        AABB get_bounding_box() const override {
            const auto box_0 { AABB(Q_ + u_ + v_, Q_ - u_ - v_) };
            const auto box_1 { AABB(Q_ + u_ - v_, Q_ - u_ + v_) };
            return AABB(box_0, box_1);
        }

        bool is_interior(double a, double b, HitRecord& rec) const override {
            if (a * a + b * b > 1) return false;
            rec.u = a;
            rec.v = b;
            return true;
        }

    public:
        Ellipse(
            const Point3& Q,
            const Direction3& u,
            const Direction3& v,
            std::shared_ptr<Material> mat
        ) : Surface { Q, u, v, mat } {}
};

class Disc : public Ellipse {
    public:
        Disc(
            const Point3& Q,
            const Direction3& u,
            const Direction3& v,
            double radius,
            std::shared_ptr<Material> mat
        ) : Ellipse {
            Q,
            unit_vector(u) / radius,
            unit_vector(
                v - dot(u, v) / dot(u, u) * u // v perpendicular to u
            ) / radius,
            mat
        } {}
};

inline std::shared_ptr<HittableList> box(
    const Point3& a,
    const Point3& b,
    std::shared_ptr<Material> mat
) {
    const auto sides { std::make_shared<HittableList>() };

    const auto min { Point3(
        std::min(a.x(), b.x()),
        std::min(a.y(), b.y()),
        std::min(a.z(), b.z())
    ) };
    const auto max { Point3(
        std::max(a.x(), b.x()),
        std::max(a.y(), b.y()),
        std::max(a.z(), b.z())
    ) };

    const auto dx { Direction3(max.x() - min.x(), 0, 0) };
    const auto dy { Direction3(0, max.y() - min.y(), 0) };
    const auto dz { Direction3(0, 0, max.z() - min.z()) };

    // Front face (z = max.z)
    sides->add(std::make_shared<Quad>(
        Point3 { min.x(), min.y(), max.z() }, dx, dy, mat
    ));
    // Right face (x = max.x)
    sides->add(std::make_shared<Quad>(
        Point3 { max.x(), min.y(), max.z() }, -dz, dy, mat
    ));
    // Back face (z = min.z)
    sides->add(std::make_shared<Quad>(
        Point3 { max.x(), min.y(), min.z() }, -dx, dy, mat
    ));
    // Left face (x = min.x)
    sides->add(std::make_shared<Quad>(
        Point3 { min.x(), min.y(), min.z() }, dz, dy, mat
    ));
    // Top face (y = max.y)
    sides->add(std::make_shared<Quad>(
        Point3 { min.x(), max.y(), min.z() }, dx, dz, mat
    ));
    // Bottom face (y = min.y)
    sides->add(std::make_shared<Quad>(
        Point3 { min.x(), min.y(), min.z() }, dx, dz, mat
    ));

    return sides;
}

#endif
