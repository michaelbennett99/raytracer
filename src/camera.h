#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "hittable.h"
#include "colour.h"
#include "ray.h"
#include "aspect.h"

class camera {
    private:
        int image_height;
        point3 origin = point3(0, 0, 0);
        point3 pixel00_loc;
        direction3 pixel_delta_u;
        direction3 pixel_delta_v;

        void initialize() {
            image_height = height(image_width, ar);
            image_height = (image_height < 1) ? 1 : image_height;

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

        colour ray_colour(const ray& r, const hittable& world) const {
            hit_record rec;

            if (world.hit(r, interval_d{0, infinity_d}, rec)) {
                return 0.5 * (rec.normal + colour{1, 1, 1});
            }

            direction3 unit_direction = unit_vector(r.direction());
            auto a = 0.5 * (unit_direction[1] + 1.0);
            return (1.0 - a) * colour{ 1.0, 1.0, 1.0 }
                + a * colour{ 0.5, 0.7, 1.0 };
        }

    public:
        double ar = 1.0;
        int image_width = 100;

        void render (const hittable& world) {
            initialize();

            std::cout
                << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            for (int j = image_height - 1; j >= 0; --j) {
                std::clog
                    << "\rScanlines remaining: " << j << ' ' << std::flush;

                for (int i = 0; i < image_width; ++i) {
                    auto pixel_center = pixel00_loc
                        + (i * pixel_delta_u)
                        + (j * pixel_delta_v);
                    auto ray_direction = pixel_center - origin;
                    ray r(origin, ray_direction);

                    colour pixel_colour = ray_colour(r, world);
                    write_colour(std::cout, pixel_colour);
                }
            }

            std::clog << "\rDone.                      \n";
        }
};

#endif
