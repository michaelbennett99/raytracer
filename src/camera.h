#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "hittable.h"
#include "colour.h"
#include "ray.h"
#include "aspect.h"
#include "random.h"
#include "material.h"
#include <thread>
#include <mutex>
#include <sstream>

class camera {
    private:
        int image_height;
        double pixel_samples_scale;
        point3 origin;
        point3 pixel00_loc;
        direction3 pixel_delta_u;
        direction3 pixel_delta_v;
        direction3 u, v, w;
        direction3 defocus_disk_u;
        direction3 defocus_disk_v;

        void initialize() {
            image_height = height(image_width, ar);
            image_height = (image_height < 1) ? 1 : image_height;

            pixel_samples_scale = 1.0 / samples_per_pixel;

            origin = lookfrom;

            // Viewport dimensions
            const auto theta = degrees_to_radians(vfov);
            const auto h = std::tan(theta / 2);
            const auto viewport_height = 2 * h * focus_dist;
            const auto viewport_width = viewport_height
                * aspect_ratio(image_width, image_height);

            // Calculate orthonormal basis
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            // Edge vectors of viewport
            const auto viewport_u = viewport_width * u; // u is horizontal
            const auto viewport_v = viewport_height * -v; // v is vertical

            // Delta vectors from pixel to pixel
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // Location of upper left pixel
            const auto viewport_ul = origin
                - (focus_dist * w)
                - 0.5 * (viewport_u + viewport_v);
            pixel00_loc = viewport_ul + 0.5 * (pixel_delta_u + pixel_delta_v);

            const auto defocus_radius = focus_dist
                * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }

        ray get_ray(int i, int j) const {
            // Construct ray from defocus disk to sampled point at (i, j)
            auto offset = sample_square();
            auto pixel_sample = pixel00_loc
                + ((i + offset.x()) * pixel_delta_u)
                + ((j + offset.y()) * pixel_delta_v);
            auto ray_origin = (defocus_angle <= 0)
                ? origin
                : defocus_disk_sample();
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

        point3 defocus_disk_sample() const {
            const auto p = random_in_unit_disk();
            return origin + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        colour ray_colour(
            const ray& r, const hittable& world, int depth
        ) const {
            if (depth <= 0) return colour(0, 0, 0);

            hit_record rec;

            if (world.hit(r, interval_d{0.001, infinity_d}, rec)) {
                ray scattered;
                colour attenuation;
                if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                    return attenuation * ray_colour(
                        scattered,
                        world,
                        depth - 1
                    );
                }
                return colour(0, 0, 0);
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

        // Camera parameters
        double vfov = 90;
        point3 lookfrom = point3(0, 0, 0);
        point3 lookat = point3(0, 0, -1);
        direction3 vup = direction3(0, 1, 0);

        // Lens parameters
        double defocus_angle = 0;
        double focus_dist = 10;

        void render(const hittable& world) {
            initialize();

            std::cout
                << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            const int num_threads = std::thread::hardware_concurrency();
            std::vector<std::thread> threads;
            std::mutex cout_mutex;
            int completed_scanlines = 0;

            // Vector to store pixel data for each scanline
            std::vector<std::string> scanlines(image_height);

            // Lambda to process a chunk of scanlines
            auto process_chunk = [&](int start_row, int end_row) {
                for (int j = start_row; j < end_row; ++j) {
                    std::ostringstream line_stream;
                    for (int i = 0; i < image_width; ++i) {
                        colour pixel_colour(0, 0, 0);
                        for (int s = 0; s < samples_per_pixel; ++s) {
                            auto ray = get_ray(i, j);
                            pixel_colour += ray_colour(ray, world, max_depth);
                        }
                        write_colour(line_stream, pixel_colour * pixel_samples_scale);
                    }
                    scanlines[j] = line_stream.str();

                    // Update progress
                    {
                        std::lock_guard<std::mutex> lock(cout_mutex);
                        completed_scanlines++;
                        std::clog << "\rScanlines remaining: "
                            << image_height - completed_scanlines << ' '
                            << std::flush;
                    }
                }
            };

            // Calculate chunk size and create threads
            const int chunk_size = (image_height + num_threads - 1) / num_threads;
            for (int t = 0; t < num_threads; ++t) {
                int start_row = t * chunk_size;
                int end_row = std::min(start_row + chunk_size, image_height);
                if (start_row < image_height) {
                    threads.emplace_back(process_chunk, start_row, end_row);
                }
            }

            // Wait for all threads to complete
            for (auto& thread : threads) {
                thread.join();
            }

            // Output all scanlines in order
            for (const auto& scanline : scanlines) {
                std::cout << scanline;
            }

            std::clog << "\rDone.                      \n";
        }
};

#endif
