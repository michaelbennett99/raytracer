#include <memory>

#include "raytracing.h"
#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "bvh.h"
#include "texture.h"

void bouncing_spheres() {
    hittable_list world;

    auto ground_material = std::make_shared<lambertian>(colour(0.5, 0.5, 0.5));
    world.add(std::make_shared<sphere>(
        point3(0,-1000,0), 1000, ground_material
    ));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = gen_rand::random_double();
            point3 center(
                a + 0.9*gen_rand::random_double(),
                0.2,
                b + 0.9*gen_rand::random_double()
            );

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = colour::random() * colour::random();
                    sphere_material = std::make_shared<lambertian>(albedo);
                    auto center2 = center
                        + direction3(0, gen_rand::random_double(0, 0.5), 0);

                    world.add(std::make_shared<sphere>(
                            center, center2, 0.2, sphere_material
                    ));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = colour::random(0.5, 1);
                    auto fuzz = gen_rand::random_double(0, 0.5);
                    sphere_material = std::make_shared<metal>(albedo, fuzz);

                    world.add(std::make_shared<sphere>(
                        center, 0.2, sphere_material
                    ));
                } else {
                    // glass
                    sphere_material = std::make_shared<dielectric>(1.5);
                    world.add(std::make_shared<sphere>(
                        center, 0.2, sphere_material
                    ));
                }
            }
        }
    }

    auto material1 = std::make_shared<dielectric>(1.5);
    world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<lambertian>(colour(0.4, 0.2, 0.1));
    world.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    world = hittable_list(std::make_shared<bvh_node>(world));

    camera cam(
        16.0 / 9.0,
        600,
        100,
        50,
        20,
        point3(13,2,3),
        point3(0,0,0),
        direction3(0,1,0),
        0.6,
        10.0
    );

    cam.render(world);
}

void checkered_spheres() {
    hittable_list world;

    const auto checker = std::make_shared<checker_texture>(
        0.32,
        colour(0.2, 0.3, 0.1),
        colour(0.9, 0.9, 0.9)
    );

    const auto sphere_material = std::make_shared<lambertian>(checker);

    world.add(std::make_shared<sphere>(point3(0,-10,0), 10, sphere_material));
    world.add(std::make_shared<sphere>(point3(0,10,0), 10, sphere_material));

    camera cam(
        16.0 / 9.0,
        400,
        100,
        50,
        20,
        point3(13, 2, 3),
        point3(0, 0, 0),
        direction3(0, 1, 0)
    );

    cam.render(world);
}

void earth() {
    hittable_list world;

    const auto earth_texture = std::make_shared<image_texture>("map.jpg");
    const auto earth_material = std::make_shared<lambertian>(earth_texture);
    const auto globe = std::make_shared<sphere>(
        point3(0, 0, 0), 2, earth_material
    );

    world.add(globe);

    camera cam(
        16.0 / 9.0,
        400,
        100,
        50,
        20,
        point3(0, 0, 12),
        point3(0, 0, 0)
    );

    cam.render(world);
}

static void usage(const char* argv0) {
    std::cerr << "Usage: " << argv0 << " <scene>" << std::endl
        << "Valid scenes: 1, 2, 3" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    switch (std::stoi(argv[1])) {
        case 1: bouncing_spheres(); break;
        case 2: checkered_spheres(); break;
        case 3: earth(); break;
        default: usage(argv[0]); return 1;
    }
}
