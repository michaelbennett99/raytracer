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

int main(int argc, char* argv[]) {
    const auto options = cli::parse_args(argc, argv);

    OutputHandler output_handler(options.output_file);

    const auto config = create_sampler_config(options);

    auto sampler_ptr = std::make_shared<Sampler>(config);

    std::vector<RendererType> renderers;

    renderers.push_back(RendererType::Colour);
    if (options.output_density && options.output_file) {
        renderers.push_back(RendererType::Density);
    }

    std::clog << "Scene: " << options.scene.value_or(12) << std::endl;

    std::clog << "Sampler config: " << sampler_ptr->get_config() << std::endl;

    scene scene;

    switch (options.scene.value_or(1)) {
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

    const auto results = scene.render();

    output_handler.write_main_image(
        results.at(RendererType::Colour), image_format::PPM
    );

    if (results.find(RendererType::Density) != results.end()) {
        output_handler.write_density_image(
            results.at(RendererType::Density)
        );
    }

    return 0;
}
