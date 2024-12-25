#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "hittable.h"
#include "colour.h"
#include "ray.h"
#include "aspect.h"
#include "random.h"

class camera {
    private:
        int image_height;
        double pixel_samples_scale;
        point3 origin = point3(0, 0, 0);
        point3 pixel00_loc;
        direction3 pixel_delta_u;
        direction3 pixel_delta_v;

        void initialize() {
            image_height = height(image_width, ar);
            image_height = (image_height < 1) ? 1 : image_height;

            pixel_samples_scale = 1.0 / samples_per_pixel;

            origin = point3(0, 0, 0);

            // Viewport dimensions
            const auto focal_length = 1.0;
            const auto viewport_height = 2.0;
            const auto viewport_width = viewport_height
                * aspect_ratio(image_width, image_height);

            // Edge vectors of viewport
            const auto viewport_u = direction3(viewport_width, 0, 0);
            const auto viewport_v = direction3(0, -viewport_height, 0);

            // Delta vectors from pixel to pixel
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // Location of upper left pixel
            const auto viewport_ul = origin
                - direction3(0, 0, focal_length)
                - viewport_u / 2
                - viewport_v / 2;
            pixel00_loc = viewport_ul
                + pixel_delta_u / 2
                + pixel_delta_v / 2;
        }

        ray get_ray(int i, int j) const {
            auto offset = sample_square();
            auto pixel_sample = pixel00_loc
                + ((i + offset.x()) * pixel_delta_u)
                + ((j + offset.y()) * pixel_delta_v);
            auto ray_origin = origin;
            auto ray_direction = pixel_sample - ray_origin;
            return ray(ray_origin, ray_direction);
        }

        direction3 sample_square() const {
            return direction3(
                gen_rand::random_double(-0.5, 0.5),
                gen_rand::random_double(-0.5, 0.5),
                0
            );
        }

        colour ray_colour(
            const ray& r, const hittable& world, int depth
        ) const {
            if (depth <= 0) return colour(0, 0, 0);

            hit_record rec;

            if (world.hit(r, interval_d{0.001, infinity_d}, rec)) {
                const auto direction = rec.normal + random_unit_vector();
                const auto scattered = ray(rec.p, direction);
                return 0.5 * ray_colour(
                    scattered,
                    world,
                    depth - 1
                );
            }

            direction3 unit_direction = unit_vector(r.direction());
            auto a = 0.5 * (unit_direction[1] + 1.0);
            return (1.0 - a) * colour{ 1.0, 1.0, 1.0 }
                + a * colour{ 0.5, 0.7, 1.0 };
        }

    public:
        double ar = 1.0;
        int image_width = 400;
        int samples_per_pixel = 100;
        int max_depth = 10;

        void render (const hittable& world) {
            initialize();

            std::cout
                << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            for (int j = 0; j < image_height; ++j) {
                std::clog
                    << "\rScanlines remaining: " << image_height - j << ' '
                    << std::flush;

                for (int i = 0; i < image_width; ++i) {
                    colour pixel_colour(0, 0, 0);
                    for (int s = 0; s < samples_per_pixel; ++s) {
                        auto ray = get_ray(i, j);
                        pixel_colour += ray_colour(ray, world, max_depth);
                    }
                    write_colour(std::cout, pixel_colour * pixel_samples_scale);
                }
            }

            std::clog << "\rDone.                      \n";
        }
};

#endif
