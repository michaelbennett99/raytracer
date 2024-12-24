#include <iostream>

#include "color.h"
#include "aspect.h"
#include "ray.h"
#include "vec3.h"
#include "sphere.h"

colour ray_colour(const ray& r) {
    const sphere s{ point3{ 0, 0, -1 }, 0.5 };
    const auto t = s.hit(r);
    if (t) {
        const auto p = r.at(*t);
        const auto n = s.normal(p);
        return 0.5 * colour{ n[0] + 1, n[1] + 1, n[2] + 1 };
    }

    direction3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction[1] + 1.0);
    return (1.0 - a) * colour{ 1.0, 1.0, 1.0 } + a * colour{ 0.5, 0.7, 1.0 };
}

int main() {
    // Image
    constexpr auto ar = 16.0 / 9.0;
    constexpr int image_width = 400;
    constexpr int max_colour_value = 255;

    int image_height = height(image_width, ar);
    image_height = (image_height < 1) ? 1 : image_height;

    // Camera

    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height
        * aspect_ratio(image_width, image_height);
    auto camera_center = point3(0, 0, 0);

    // Calculate vectors across horizontal and down vertical viewport edges
    auto viewport_u = direction3(viewport_width, 0, 0);
    auto viewport_v = direction3(0, -viewport_height, 0);

    // Delta vectors from pixel to pixel
    auto pixel_delta_u = viewport_u / static_cast<double>(image_width);
    auto pixel_delta_v = viewport_v / static_cast<double>(image_height);

    // Location of upper left pixel
    auto viewport_ul = camera_center
        - direction3(0, 0, focal_length)
        - viewport_u / 2
        - viewport_v / 2;
    auto pixel00_loc = viewport_ul
        + pixel_delta_u / 2
        + pixel_delta_v / 2;

    // Render

    std::cout
        << "P3\n"
        << image_width << " " << image_height << "\n"
        << max_colour_value << "\n";

    for (int j = 0; j < image_height; j++) {
        std::clog
            << "\rScanlines remaining: " << (image_height - j) << " "
            << std::flush;
        for (int i = 0; i < image_width; i++) {
            auto pixel_centre = pixel00_loc
                + (i * pixel_delta_u)
                + (j * pixel_delta_v);
            auto ray_direction = pixel_centre - camera_center;
            ray r(camera_center, ray_direction);
            auto pixel_colour = ray_colour(r);
            write_colour(std::cout, pixel_colour);
        }
    }
    std::clog << "\rDone.                    \n";
}
