#ifndef QUAD_H
#define QUAD_H

#include <memory>

#include "aabb.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"
#include "hittable_list.h"

// Abstract class for surfaces
class surface : public hittable {
    protected:
        point3 Q_;
        direction3 u_, v_;
        direction3 w_;
        std::shared_ptr<material> mat_;
        aabb bbox_;
        direction3 normal_;
        double D_;

        virtual void set_bounding_box() = 0;
        virtual bool is_interior(double a, double b, hit_record& rec) const = 0;

    public:
        aabb bounding_box() const override {
            return bbox_;
        }

        bool hit(
            const ray& r,
            interval_d ray_t,
            hit_record& rec
        ) const override {
            const auto denom = dot(normal_, r.direction());

            if (std::fabs(denom) < 1e-8) return false;

            const auto t = (D_ - dot(normal_, r.origin())) / denom;
            if (!ray_t.contains(t)) return false;

            const auto intersection = r.at(t);
            const auto pos_on_plane = intersection - Q_;
            const auto alpha = dot(w_, cross(pos_on_plane, v_));
            const auto beta = dot(w_, cross(u_, pos_on_plane));

            if (!is_interior(alpha, beta, rec)) return false;
            rec.t = t;
            rec.p = intersection;
            rec.mat = mat_;
            rec.set_face_normal(r, normal_);

            return true;
        }
};

// Abstract class for simple surfaces
class simple_surface : public surface {
    protected:
        void init_surface(
            const point3& Q,
            const direction3& u,
            const direction3& v,
            std::shared_ptr<material> mat
        ) {
            Q_ = Q;
            u_ = u;
            v_ = v;
            mat_ = mat;

            const auto n = cross(u_, v_);
            normal_ = unit_vector(n);
            D_ = dot(normal_, Q_);
            w_ = n / dot(n, n);

            set_bounding_box();
        }
};

class quad : public simple_surface {
    private:
        void set_bounding_box() override {
            const auto box_0 = aabb(Q_, Q_ + u_ + v_);
            const auto box_1 = aabb(Q_ + u_, Q_ + v_);
            bbox_ = aabb(box_0, box_1);
        }

        bool is_interior(double a, double b, hit_record& rec) const override {
            interval_d unit {0, 1};
            if (!unit.contains(a) || !unit.contains(b)) return false;
            rec.u = a;
            rec.v = b;
            return true;
        }

    public:
        quad(
            const point3& Q,
            const direction3& u,
            const direction3& v,
            std::shared_ptr<material> mat
        ) {
            init_surface(Q, u, v, mat);
        }
};

class triangle : public simple_surface {
    private:
        void set_bounding_box() override {
            const auto box_0 = aabb(Q_, Q_ + u_);
            const auto box_1 = aabb(Q_, Q_ + v_);
            bbox_ = aabb(box_0, box_1);
        }

        bool is_interior(double a, double b, hit_record& rec) const override {
            interval_d unit {0, 1};
            if (!unit.contains(a) || !unit.contains(b)) return false;
            if (a + b > 1) return false;

            rec.u = a;
            rec.v = b;
            return true;
        }

    public:
        triangle(
            const point3& Q,
            const direction3& u,
            const direction3& v,
            std::shared_ptr<material> mat
        ) {
            init_surface(Q, u, v, mat);
        }
};

class ellipse : public simple_surface {
    private:
        void set_bounding_box() override {
            const auto box_0 = aabb(Q_ + u_ + v_, Q_ - u_ - v_);
            const auto box_1 = aabb(Q_ + u_ - v_, Q_ - u_ + v_);
            bbox_ = aabb(box_0, box_1);
        }

        bool is_interior(double a, double b, hit_record& rec) const override {
            if (a * a + b * b > 1) return false;
            rec.u = a;
            rec.v = b;
            return true;
        }

    public:
        ellipse(
            const point3& Q,
            const direction3& u,
            const direction3& v,
            std::shared_ptr<material> mat
        ) {
            init_surface(Q, u, v, mat);
        }
};

class disc : public ellipse {
    public:
        disc(
            const point3& Q,
            const direction3& u,
            const direction3& v,
            double radius,
            std::shared_ptr<material> mat
        ) : ellipse(Q, u, v, mat) {
            // Need to get the component of v_ perpendicular to u_
            const auto v_perp = v_ - dot(u_, v_) / dot(u_, u_) * u_;
            // Need to divide by radius due to dual basis method of getting
            // the position parameters
            u_ = unit_vector(u_) / radius;
            v_ = unit_vector(v_perp) / radius;
        }
};

inline std::shared_ptr<hittable_list> box(
    const point3& a, const point3& b, std::shared_ptr<material> mat
) {
    const auto sides = std::make_shared<hittable_list>();

    const auto min = point3(
        std::min(a.x(), b.x()),
        std::min(a.y(), b.y()),
        std::min(a.z(), b.z())
    );
    const auto max = point3(
        std::max(a.x(), b.x()),
        std::max(a.y(), b.y()),
        std::max(a.z(), b.z())
    );

    const auto dx = direction3(max.x() - min.x(), 0, 0);
    const auto dy = direction3(0, max.y() - min.y(), 0);
    const auto dz = direction3(0, 0, max.z() - min.z());

    // Front face (z = max.z)
    sides->add(std::make_shared<quad>(
        point3(min.x(), min.y(), max.z()), dx, dy, mat
    ));
    // Right face (x = max.x)
    sides->add(std::make_shared<quad>(
        point3(max.x(), min.y(), max.z()), -dz, dy, mat
    ));
    // Back face (z = min.z)
    sides->add(std::make_shared<quad>(
        point3(max.x(), min.y(), min.z()), -dx, dy, mat
    ));
    // Left face (x = min.x)
    sides->add(std::make_shared<quad>(
        point3(min.x(), min.y(), min.z()), dz, dy, mat
    ));
    // Top face (y = max.y)
    sides->add(std::make_shared<quad>(
        point3(min.x(), max.y(), min.z()), dx, dz, mat
    ));
    // Bottom face (y = min.y)
    sides->add(std::make_shared<quad>(
        point3(min.x(), min.y(), min.z()), dx, dz, mat
    ));

    return sides;
}

#endif
