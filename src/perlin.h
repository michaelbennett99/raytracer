#ifndef PERLIN_H
#define PERLIN_H

#include "random.h"
#include "vec3.h"
#include <array>

class Perlin {
    private:
        static constexpr int point_count { 256 };
        const std::array<Direction3, point_count> rand_vec;
        const std::array<int, point_count> perm_x;
        const std::array<int, point_count> perm_y;
        const std::array<int, point_count> perm_z;

        static std::array<Direction3, point_count> gen_rand_vec() {
            std::array<Direction3, point_count> p {};
            for (int i { 0 }; i < point_count; i++) {
                p[i] = unit_vector(Direction3::random(-1, 1));
            }
            return p;
        }

        static std::array<int, point_count> perlin_generate_perm() {
            std::array<int, point_count> p {};
            for (int i { 0 }; i < point_count; i++) {
                p[i] = i;
            }

            permute(p);
            return p;
        }

        static void permute(std::array<int, point_count>& p) {
            for (int i { point_count-1 }; i > 0; i--) {
                const int target { gen_rand::random_int(0, i) };
                std::swap(p[i], p[target]);
            }
        }

        static double perlin_interp(
            const Direction3 c[2][2][2], double u, double v, double w
        ) {
            const auto uu { u*u*(3-2*u) };
            const auto vv { v*v*(3-2*v) };
            const auto ww { w*w*(3-2*w) };
            auto accum { 0.0 };

            for (int i { 0 }; i < 2; i++) {
                for (int j { 0 }; j < 2; j++) {
                    for (int k { 0 }; k < 2; k++) {
                        const auto weight_v { Vec3(u-i, v-j, w-k) };
                        accum += (i*uu + (1-i)*(1-uu))
                            * (j*vv + (1-j)*(1-vv))
                            * (k*ww + (1-k)*(1-ww))
                            * dot(c[i][j][k], weight_v);
                    }
                }
            }

            return accum;
        }

    public:
        Perlin()
            : rand_vec { gen_rand_vec() }
            , perm_x { perlin_generate_perm() }
            , perm_y { perlin_generate_perm() }
            , perm_z { perlin_generate_perm() }
        {}

        double noise(const Point3& p) const {
            const auto u { p.x() - std::floor(p.x()) };
            const auto v { p.y() - std::floor(p.y()) };
            const auto w { p.z() - std::floor(p.z()) };

            const auto i { static_cast<int>(std::floor(p.x())) };
            const auto j { static_cast<int>(std::floor(p.y())) };
            const auto k { static_cast<int>(std::floor(p.z())) };

            Direction3 c[2][2][2] {};
            for (int di { 0 }; di < 2; di++) {
                for (int dj { 0 }; dj < 2; dj++) {
                    for (int dk { 0 }; dk < 2; dk++) {
                        c[di][dj][dk] = rand_vec[
                            perm_x[(i+di) & (point_count-1)] ^
                            perm_y[(j+dj) & (point_count-1)] ^
                            perm_z[(k+dk) & (point_count-1)]
                        ];
                    }
                }
            }

            return perlin_interp(c, u, v, w);
        }

        double turb(const Point3& p, int depth) const {
            auto accum { 0.0 };
            auto temp_p { p };
            auto weight { 1.0 };

            for (int i { 0 }; i < depth; i++) {
                accum += weight * noise(temp_p);
                weight *= 0.5;
                temp_p *= 2;
            }

            return std::fabs(accum);
        }
};

#endif
