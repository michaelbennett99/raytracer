#include <memory>
#include <fstream>
#include <iostream>

#include "scene.h"
#include "cli.h"
#include "sampler.h"
#include "output.h"
#include "renderer.h"

SamplerConfig create_sampler_config(const RenderOptions& options) {
    SamplerConfig config;
    if (options.samples_per_pixel) {
        config.samples_per_pixel = options.samples_per_pixel;
    }
    if (options.burn_in) {
        config.adaptive.burn_in = options.burn_in;
    }
    if (options.check_every) {
        config.adaptive.check_every = options.check_every;
    }
    if (options.tolerance) {
        config.adaptive.tolerance = options.tolerance;
    }
    return config;
}

std::shared_ptr<sampler> create_sampler(const RenderOptions& options) {
    const auto config = create_sampler_config(options);
    if (options.adaptive_sampling) {
        return std::make_shared<adaptive_random_sampler>(config);
    }
    return std::make_shared<random_sampler>(config);
}

std::vector<std::shared_ptr<Renderer>> create_renderers(
    const RenderOptions& options
) {
    std::vector<std::shared_ptr<Renderer>> renderers;
    renderers.push_back(std::make_shared<MainRenderer>());
    return renderers;
}

int main(int argc, char* argv[]) {
    const auto options = cli::parse_args(argc, argv);

    OutputHandler output_handler(options.output_file);

    const auto sampler_ptr = create_sampler(options);

    const auto renderers = create_renderers(options);

    std::clog << "Scene: " << options.scene.value_or(12) << std::endl;

    std::clog << "Sampler config: " << sampler_ptr->get_config() << std::endl;

    scene scene;

    switch (options.scene.value_or(12)) {
    case 1:
        scene = bouncing_spheres(
            sampler_ptr, renderers, options.aspect_ratio, options.image_width
        ); break;
    case 2:
        scene = checkered_spheres(
            sampler_ptr, renderers, options.aspect_ratio, options.image_width
        ); break;
    case 3:
        scene = earth(
            sampler_ptr, renderers, options.aspect_ratio, options.image_width
        ); break;
    case 4:
        scene = perlin_spheres(
            sampler_ptr, renderers, options.aspect_ratio, options.image_width
        ); break;
    case 5:
        scene = quads(
            sampler_ptr, renderers, options.aspect_ratio, options.image_width
        ); break;
    case 6:
        scene = triangles(
            sampler_ptr, renderers, options.aspect_ratio, options.image_width
        ); break;
    case 7:
        scene = ellipses(
            sampler_ptr, renderers, options.aspect_ratio, options.image_width
        ); break;
    case 8:
        scene = simple_light(
            sampler_ptr, renderers, options.aspect_ratio, options.image_width
        ); break;
    case 9:
        scene = cornell_box(
            sampler_ptr, renderers, options.aspect_ratio, options.image_width
        ); break;
    case 10:
        scene = cornell_smoke(
            sampler_ptr, renderers, options.aspect_ratio, options.image_width
        ); break;
    case 11:
        scene = final_scene(
            sampler_ptr, renderers, options.aspect_ratio, options.image_width
        ); break;
    default:
        std::cerr << "Invalid scene number" << std::endl;
        exit(1);
    }

    scene.render();

    return 0;
}
