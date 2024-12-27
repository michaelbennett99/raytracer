#ifndef QUAD_H
#define QUAD_H

#include <memory>

#include "aabb.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"

class quad : public hittable {
    private:
        point3 Q_;
        direction3 u_, v_;
        direction3 w_;
        std::shared_ptr<material> mat_;
        aabb bbox_;
        direction3 normal_;
        double D_;

        void set_bounding_box() {
            const auto box_0 = aabb(Q_, Q_ + u_ + v_);
            const auto box_1 = aabb(Q_ + u_, Q_ + v_);
            bbox_ = aabb(box_0, box_1);
        }

        bool is_interior(double a, double b) const {
            interval_d unit {0, 1};
            return unit.contains(a) && unit.contains(b);
        }

    public:
        quad(
            const point3& Q,
            const direction3& u,
            const direction3& v,
            std::shared_ptr<material> mat
        ) : Q_{Q}, u_{u}, v_{v}, mat_{mat} {
            const auto n = cross(u_, v_);
            normal_ = unit_vector(n);
            D_ = dot(normal_, Q_);
            w_ = n / dot(n, n);

            set_bounding_box();
        }

        aabb bounding_box() const override {
            return bbox_;
        }

        bool hit(
            const ray& r, interval_d ray_t, hit_record& rec
        ) const override {
            const auto denom = dot(normal_, r.direction());

            if (std::fabs(denom) < 1e-8) return false;

            const auto t = (D_ - dot(normal_, r.origin())) / denom;
            if (!ray_t.contains(t)) return false;

            const auto intersection = r.at(t);
            const auto pos_on_plane = intersection - Q_;
            const auto alpha = dot(w_, cross(pos_on_plane, v_));
            const auto beta = dot(w_, cross(u_, pos_on_plane));

            if (!is_interior(alpha, beta)) return false;

            rec.t = t;
            rec.p = intersection;
            rec.mat = mat_;
            rec.set_face_normal(r, normal_);
            rec.u = alpha;
            rec.v = beta;

            return true;
        }
};

#endif
