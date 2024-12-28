#ifndef SAMPLER_H
#define SAMPLER_H

#include <iterator>

#include "colour.h"
#include "hittable.h"
#include "vec3.h"
#include "ray.h"

class sampler;
class ray_iterator;

struct sample_range {
    const sampler* s;
    int i, j;
    ray_iterator begin() const;
    ray_iterator end() const;
};

class sampler {
    protected:
        // Data we need to sample rays
        point3 origin;
        point3 pixel00_loc;
        direction3 pixel_delta_u;
        direction3 pixel_delta_v;
        double defocus_angle;
        direction3 defocus_disk_u;
        direction3 defocus_disk_v;

        static direction3 sample_square() {
            return direction3(
                gen_rand::random_double(-0.5, 0.5),
                gen_rand::random_double(-0.5, 0.5),
                0
            );
        }

        point3 defocus_disk_sample() const {
            const auto p = random_in_unit_disk();
            return origin + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        virtual point3 sample_pixel(int i, int j) const = 0;

    public:
        sampler(
            point3 origin,
            point3 pixel00_loc,
            direction3 pixel_delta_u,
            direction3 pixel_delta_v,
            double defocus_angle,
            direction3 defocus_disk_u,
            direction3 defocus_disk_v
        ) : origin(origin),
            pixel00_loc(pixel00_loc),
            pixel_delta_u(pixel_delta_u),
            pixel_delta_v(pixel_delta_v),
            defocus_angle(defocus_angle),
            defocus_disk_u(defocus_disk_u),
            defocus_disk_v(defocus_disk_v) {}

        virtual ~sampler() = default;

        ray get_ray(int i, int j) const {
            const auto pixel_sample = sample_pixel(i, j);
            const auto ray_origin = (defocus_angle <= 0)
                ? origin
                : defocus_disk_sample();
            const auto ray_direction = pixel_sample - ray_origin;
            const auto ray_time = gen_rand::random_double(0, 1);

            return ray(ray_origin, ray_direction, ray_time);
        }

        sample_range samples(int i, int j) const {
            return {this, i, j};
        }

        virtual bool has_next_sample(int i, int j, int sample) const = 0;
};

class ray_iterator {
    private:
        const sampler* sampler_ptr;
        int pixel_i, pixel_j, current_sample;
        ray current_ray;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = ray;
        using difference_type = std::ptrdiff_t;
        using pointer = const ray*;
        using reference = const ray&;

        ray_iterator(
            const sampler* s, int i, int j, int sample = 0
        ) : sampler_ptr(s),
            pixel_i(i),
            pixel_j(j),
            current_sample(sample) {
            if (sampler_ptr) {
                current_ray = sampler_ptr->get_ray(pixel_i, pixel_j);
            }
        }

        reference operator*() const { return current_ray; }
        pointer operator->() const { return &current_ray; }

        ray_iterator& operator++() {
            if (sampler_ptr && sampler_ptr->has_next_sample(
                pixel_i, pixel_j, current_sample
            )) {
                current_sample++;
                current_ray = sampler_ptr->get_ray(pixel_i, pixel_j);
            } else {
                sampler_ptr = nullptr;
            }
            return *this;
        }

        bool operator==(const ray_iterator& other) const {
            return sampler_ptr == other.sampler_ptr;
        }

        bool operator!=(const ray_iterator& other) const {
            return !(*this == other);
        }
};

class random_sampler : public sampler {
    private:
        int samples_per_pixel;

        point3 sample_pixel(int i, int j) const override {
            const auto offset = sample_square();
            return pixel00_loc
                + (i + offset.x()) * pixel_delta_u
                + (j + offset.y()) * pixel_delta_v;
        }

    public:
        random_sampler(
            point3 origin,
            point3 pixel00_loc,
            direction3 pixel_delta_u,
            direction3 pixel_delta_v,
            double defocus_angle,
            direction3 defocus_disk_u,
            direction3 defocus_disk_v,
            int samples_per_pixel
        ) : sampler(
            origin,
            pixel00_loc,
            pixel_delta_u,
            pixel_delta_v,
            defocus_angle,
            defocus_disk_u,
            defocus_disk_v
        ),
        samples_per_pixel(samples_per_pixel) {}

        bool has_next_sample(int i, int j, int sample) const override {
            return sample < samples_per_pixel;
        }
};

inline ray_iterator sample_range::begin() const {
    return ray_iterator(s, i, j);
}

inline ray_iterator sample_range::end() const {
    return ray_iterator(nullptr, i, j);
}

#endif // SAMPLER_H
