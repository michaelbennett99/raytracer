#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "hittable.h"
#include "colour.h"
#include "ray.h"
#include "aspect.h"
#include "random.h"
#include "material.h"
#include "sampler.h"
#include <thread>
#include <mutex>
#include <sstream>

class camera {
    private:
        // Basic image properties
        double ar;
        int image_width;
        int image_height;
        int samples_per_pixel;
        int max_depth;
        colour background;

        // Camera parameters
        double vfov;
        point3 lookfrom;
        point3 lookat;
        direction3 vup;

        // Lens parameters
        double defocus_angle;
        double focus_dist;
        double pixel_samples_scale;

        // Pixel sampler
        std::unique_ptr<sampler> sampler;

        int calc_image_height() const {
            const auto h = height(image_width, ar);
            return (h < 1) ? 1 : h;
        }

        void initialize() {
            image_height = calc_image_height();
            const auto origin = lookfrom;

            // Viewport dimensions
            const auto theta = degrees_to_radians(vfov);
            const auto h = std::tan(theta / 2);
            const auto viewport_height = 2 * h * focus_dist;
            const auto viewport_width = viewport_height
                * aspect_ratio(image_width, image_height);

            // Calculate orthonormal basis
            const auto w = unit_vector(lookfrom - lookat);
            const auto u = unit_vector(cross(vup, w));
            const auto v = cross(w, u);

            // Edge vectors of viewport
            const auto viewport_u = viewport_width * u; // u is horizontal
            const auto viewport_v = viewport_height * -v; // v is vertical

            // Delta vectors from pixel to pixel
            const auto pixel_delta_u = viewport_u / image_width;
            const auto pixel_delta_v = viewport_v / image_height;

            // Location of upper left pixel
            const auto viewport_ul = origin
                - (focus_dist * w)
                - 0.5 * (viewport_u + viewport_v);
            const auto pixel00_loc = viewport_ul
                + 0.5 * (pixel_delta_u + pixel_delta_v);

            const auto defocus_radius = focus_dist
                * std::tan(degrees_to_radians(defocus_angle / 2));
            const auto defocus_disk_u = u * defocus_radius;
            const auto defocus_disk_v = v * defocus_radius;

            sampler = std::make_unique<random_sampler>(
                origin,
                pixel00_loc,
                pixel_delta_u,
                pixel_delta_v,
                defocus_angle,
                defocus_disk_u,
                defocus_disk_v,
                samples_per_pixel
            );
        }

        colour ray_colour(
            const ray& r, const hittable& world, int depth
        ) const {
            if (depth <= 0) return colour(0, 0, 0);

            hit_record rec;

            if (!world.hit(r, interval_d{0.001, infinity_d}, rec)) {
                return background;
            }

            ray scattered;
            colour attenuation;
            colour emmitted = rec.mat->emitted(rec.u, rec.v, rec.p);

            if (!rec.mat->scatter(r, rec, attenuation, scattered)) {
                return emmitted;
            }

            return emmitted
                + attenuation * ray_colour(scattered, world, depth - 1);
        }

    public:
        camera(
            double ar = 1.0,
            int image_width = 400,
            int samples_per_pixel = 100,
            int max_depth = 10,
            colour background = colour(0, 0, 0),
            double vfov = 90,
            point3 lookfrom = point3(0, 0, 0),
            point3 lookat = point3(0, 0, -1),
            direction3 vup = direction3(0, 1, 0),
            double defocus_angle = 0,
            double focus_dist = 10
        ) : ar{ ar },
            image_width{ image_width },
            samples_per_pixel{ samples_per_pixel },
            max_depth{ max_depth },
            background{ background },
            vfov{ vfov },
            lookfrom{ lookfrom },
            lookat{ lookat },
            vup{ vup },
            defocus_angle{ defocus_angle },
            focus_dist{ focus_dist },
            pixel_samples_scale{ 1.0 / samples_per_pixel }
        {
            initialize();
        }

        void render(const hittable& world) {
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
                        const auto rays = sampler->samples(i, j);
                        for (const auto& ray : rays) {
                            pixel_colour += ray_colour(ray, world, max_depth);
                        }
                        write_colour(
                            line_stream, pixel_colour * pixel_samples_scale
                        );
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
            const int chunk_size = (image_height + num_threads - 1)
                / num_threads;
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
