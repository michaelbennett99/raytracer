#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <thread>
#include <mutex>
#include <sstream>
#include <functional>
#include <optional>

#include "vec3.h"
#include "hittable.h"
#include "colour.h"
#include "ray.h"
#include "random.h"
#include "material.h"
#include "sampler.h"
#include "image.h"
#include "world.h"
#include "renderer.h"

class camera {
    private:
        // Image storage
        ImageData image_data;

        // Pixel sampler
        std::shared_ptr<sampler> sampler_;

        // Render parameters
        int max_depth;

        // Camera parameters
        double vfov;
        point3 lookfrom;
        point3 lookat;
        direction3 vup;

        // Lens parameters
        double defocus_angle;
        double focus_dist;

        std::vector<std::shared_ptr<Renderer>> renderers_;

        void initialize() {
            const auto origin = lookfrom;

            // Viewport dimensions
            const auto theta = degrees_to_radians(vfov);
            const auto h = std::tan(theta / 2);
            const auto viewport_height = 2 * h * focus_dist;
            const auto viewport_width = viewport_height
                * image_data.aspect_ratio();

            // Calculate orthonormal basis
            const auto w = unit_vector(lookfrom - lookat);
            const auto u = unit_vector(cross(vup, w));
            const auto v = cross(w, u);

            // Edge vectors of viewport
            const auto viewport_u = viewport_width * u; // u is horizontal
            const auto viewport_v = viewport_height * -v; // v is vertical

            // Delta vectors from pixel to pixel
            const auto pixel_delta_u = viewport_u / image_data.width;
            const auto pixel_delta_v = viewport_v / image_data.height;

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

            sampler_->initialise(
                origin,
                pixel00_loc,
                pixel_delta_u,
                pixel_delta_v,
                defocus_angle,
                defocus_disk_u,
                defocus_disk_v
            );

            for (auto& renderer : renderers_) {
                renderer->prepare(image_data);
            }
        }

        void process_pixel(int i, int j, const World& world) {
            for (auto& renderer : renderers_) {
                renderer->start_pixel(i, j);
            }

            const auto rays = sampler_->samples(i, j);
            for (const auto& ray : rays) {
                colour pixel_colour = world.ray_colour(ray, max_depth);
                for (auto& renderer : renderers_) {
                    renderer->process_sample(i, j, ray, pixel_colour);
                }
            }

            for (auto& renderer : renderers_) {
                renderer->finish_pixel(i, j);
            }

            sampler_->clear();
        }

    public:
        camera() = delete;
        camera(
            std::shared_ptr<sampler> sampler,
            std::vector<std::shared_ptr<Renderer>> renderers,
            double ar = 1.0,
            int image_width = 400,
            int max_depth = 10,
            double vfov = 90,
            point3 lookfrom = point3(0, 0, 0),
            point3 lookat = point3(0, 0, -1),
            direction3 vup = direction3(0, 1, 0),
            double defocus_angle = 0,
            double focus_dist = 10
        ) :
            image_data{
                image_width, Image::calc_image_height(image_width, ar)
            },
            sampler_{ sampler },
            max_depth{ max_depth },
            vfov{ vfov },
            lookfrom{ lookfrom },
            lookat{ lookat },
            vup{ vup },
            defocus_angle{ defocus_angle },
            focus_dist{ focus_dist },
            renderers_{ renderers }
        {
            initialize();
        }

        void add_renderer(std::shared_ptr<Renderer> renderer) {
            renderer->prepare(image_data);
            renderers_.push_back(renderer);
        }

        std::vector<Image> get_results() const {
            std::vector<Image> results;
            for (const auto& renderer : renderers_) {
                results.push_back(renderer->get_result());
            }
            return results;
        }

        // Modified render method that doesn't handle output
        void render(const World& world) {
            for (int j = 0; j < image_data.height; ++j) {
                for (int i = 0; i < image_data.width; ++i) {
                    process_pixel(i, j, world);
                }
                std::clog << "\rScanlines remaining: "
                    << image_data.height - j << ' '
                    << std::flush;
            }

            std::clog << "\rDone.                      \n";
        }
};

#endif
