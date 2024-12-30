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

int main(int argc, char* argv[]) {
    const auto options = cli::parse_args(argc, argv);

    OutputHandler output_handler(options.output_file);

    auto sampler_ptr = create_sampler(options);

    std::map<std::string, std::shared_ptr<Renderer>> renderers_map;

    auto main_renderer = std::make_shared<MainRenderer>(sampler_ptr);
    renderers_map["main"] = main_renderer;
    if (options.output_density && options.output_file) {
        const auto density_renderer = std::make_shared<DensityRenderer>(
            sampler_ptr
        );
        renderers_map["density"] = density_renderer;
    }

    auto renderers = std::vector<std::shared_ptr<Renderer>>();
    for (const auto& renderer : renderers_map) {
        renderers.push_back(renderer.second);
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

    scene.render();

    output_handler.write_main_image(
        renderers_map["main"]->get_result(), image_format::PPM
    );

    if (renderers_map.find("density") != renderers_map.end()) {
        output_handler.write_density_image(
            renderers_map["density"]->get_result()
        );
    }

    return 0;
}
