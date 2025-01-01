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
#include "pixel_sampler.h"
#include "sampler_types.h"
#include "image.h"
#include "world.h"
#include "renderer.h"

class camera {
    private:
        // Image storage
        ImageData image_data;

        // Pixel sampler
        Sampler sampler;
        Renderers renderers;

        // Render parameters
        int max_depth;

        void process_pixel(int i, int j, const World& world) {
            auto pixel_sampler_ptr = sampler.pixel(i, j);
            auto& pixel_sampler = *pixel_sampler_ptr;
            std::vector<std::unique_ptr<PixelRenderer>> pixel_renderer_ptrs;
            for (auto& renderer : renderers) {
                pixel_renderer_ptrs.emplace_back(
                    renderer.create_pixel_renderer(i, j, pixel_sampler)
                );
            }

            for (const auto& ray : pixel_sampler) {
                colour pixel_colour = world.ray_colour(ray, max_depth);
                for (auto& renderer : pixel_renderer_ptrs) {
                    renderer->process_sample(ray, pixel_colour);
                }
                pixel_sampler.add_sample(pixel_colour);
            }
        }

    public:
        camera() = delete;
        camera(
            const SamplerConfig& sampler_config,
            const std::vector<RendererType>& renderer_types,
            double ar = 1.0,
            int image_width = 400,
            int max_depth = 10,
            double vfov = 90,
            const point3& lookfrom = point3(0, 0, 0),
            const point3& lookat = point3(0, 0, -1),
            const direction3& vup = direction3(0, 1, 0),
            double defocus_angle = 0,
            double focus_dist = 10
        ) :
            image_data{
                image_width, Image::calc_image_height(image_width, ar)
            },
            sampler{
                sampler_config,
                image_data,
                lookfrom,
                lookat,
                vup,
                vfov,
                defocus_angle,
                focus_dist
            },
            renderers{image_data, renderer_types},
            max_depth{ max_depth } {}

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

        std::map<RendererType, Image> get_results() const {
            std::map<RendererType, Image> results;
            for (const auto& renderer : renderers) {
                results[renderer.type()] = renderer.image();
            }
            return results;
        }
};

#endif
