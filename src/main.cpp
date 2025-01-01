#include <memory>
#include <fstream>
#include <iostream>
#include <map>

#include "scene.h"
#include "cli.h"
#include "sampler.h"
#include "output.h"
#include "renderer.h"

SamplerConfig create_sampler_config(const RenderOptions& options) {
    SamplerConfig config;
    config.random.enabled = true;
    config.adaptive.enabled = options.adaptive_sampling;
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

std::vector<RendererType> create_renderer_config(const RenderOptions& options) {
    std::vector<RendererType> renderer_types;
    renderer_types.push_back(RendererType::Colour);
    if (options.output_density && options.output_file) {
        renderer_types.push_back(RendererType::Density);
    }
    return renderer_types;
}

int main(int argc, char* argv[]) {
    const auto options = CLI::parse_args(argc, argv);

    OutputHandler output_handler(options.output_file);

    const auto sampler_config = create_sampler_config(options);
    const auto renderer_types = create_renderer_config(options);
    const auto scene_number = options.scene.value_or(1);

    std::clog << "Scene: " << scene_number << std::endl;

    std::clog << "Sampler config: " << sampler_config << std::endl;

    scene scene;

    switch (scene_number) {
    case 1:
        scene = bouncing_spheres(
            sampler_config, renderer_types, options.aspect_ratio, options.image_width
        ); break;
    case 2:
        scene = checkered_spheres(
            sampler_config, renderer_types, options.aspect_ratio, options.image_width
        ); break;
    case 3:
        scene = earth(
            sampler_config, renderer_types, options.aspect_ratio, options.image_width
        ); break;
    case 4:
        scene = perlin_spheres(
            sampler_config, renderer_types, options.aspect_ratio, options.image_width
        ); break;
    case 5:
        scene = quads(
            sampler_config, renderer_types, options.aspect_ratio, options.image_width
        ); break;
    case 6:
        scene = triangles(
            sampler_config, renderer_types, options.aspect_ratio, options.image_width
        ); break;
    case 7:
        scene = ellipses(
            sampler_config, renderer_types, options.aspect_ratio, options.image_width
        ); break;
    case 8:
        scene = simple_light(
            sampler_config, renderer_types, options.aspect_ratio, options.image_width
        ); break;
    case 9:
        scene = cornell_box(
            sampler_config, renderer_types, options.aspect_ratio, options.image_width
        ); break;
    case 10:
        scene = cornell_smoke(
            sampler_config, renderer_types, options.aspect_ratio, options.image_width
        ); break;
    case 11:
        scene = final_scene(
            sampler_config, renderer_types, options.aspect_ratio, options.image_width
        ); break;
    default:
        std::cerr << "Invalid scene number" << std::endl;
        exit(1);
    }

    const auto results = scene.render();

    output_handler.write_main_image(
        results.at(RendererType::Colour), ImageFormat::PPM
    );

    if (results.find(RendererType::Density) != results.end()) {
        output_handler.write_density_image(
            results.at(RendererType::Density)
        );
    }

    return 0;
}
