#include <memory>

#include "raytracing.h"
#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"
#include "movement.h"
#include "constant_medium.h"

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
        colour(0.7, 0.8, 1.0),
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
        colour(0.7, 0.8, 1.0),
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
        colour(0.7, 0.8, 1.0),
        20,
        point3(0, 0, 12),
        point3(0, 0, 0)
    );

    cam.render(world);
}

void perlin_spheres() {
    hittable_list world;

    const auto pertext = std::make_shared<noise_texture>(4);
    const auto sphere_material = std::make_shared<lambertian>(pertext);

    world.add(
        std::make_shared<sphere>(point3(0, -1000, 0), 1000, sphere_material)
    );
    world.add(
        std::make_shared<sphere>(point3(0, 2, 0), 2, sphere_material)
    );

    camera cam(
        16.0 / 9.0,
        400,
        100,
        50,
        colour(0.7, 0.8, 1.0),
        20,
        point3(13, 2, 3),
        point3(0, 0, 0),
        direction3(0, 1, 0)
    );

    cam.render(world);
}

void quads() {
    hittable_list world;

    // Materials
    const auto left_red = std::make_shared<lambertian>(colour(1, 0.2, 0.2));
    const auto back_green = std::make_shared<lambertian>(colour(0.2, 1, 0.2));
    const auto right_blue = std::make_shared<lambertian>(colour(0.2, 0.2, 1));
    const auto upper_orange = std::make_shared<lambertian>(colour(1, 0.5, 0));
    const auto lower_teal = std::make_shared<lambertian>(colour(0.2, 0.8, 0.8));

    // Quads
    world.add(std::make_shared<quad>(
        point3(-3, -2, 5), direction3(0, 0, -4), direction3(0, 4, 0), left_red
    ));
    world.add(std::make_shared<quad>(
        point3(-2, -2, 0), direction3(4, 0, 0), direction3(0, 4, 0), right_blue
    ));
    world.add(std::make_shared<quad>(
        point3(3, -2, 1), direction3(0, 0, 4), direction3(0, 4, 0), back_green
    ));
    world.add(std::make_shared<quad>(
        point3(-2, 3, 1), direction3(4, 0, 0), direction3(0, 0, 4), upper_orange
    ));
    world.add(std::make_shared<quad>(
        point3(-2, -3, 1), direction3(4, 0, 0), direction3(0, 0, 4), lower_teal
    ));

    camera cam(
        1.0,
        400,
        100,
        50,
        colour(0.7, 0.8, 1.0),
        80,
        point3(0, 0, 9),
        point3(0, 0, 0)
    );

    cam.render(world);
}

void triangles() {
    hittable_list world;

    const auto left_red = std::make_shared<lambertian>(colour(1, 0.2, 0.2));
    const auto back_green = std::make_shared<lambertian>(colour(0.2, 1, 0.2));
    const auto right_blue = std::make_shared<lambertian>(colour(0.2, 0.2, 1));

    world.add(std::make_shared<triangle>(
        point3(-3, -2, 5), direction3(0, 0, -4), direction3(0, 4, 0), left_red
    ));
    world.add(std::make_shared<triangle>(
        point3(-2, -2, 0), direction3(4, 0, 0), direction3(0, 4, 0), right_blue
    ));
    world.add(std::make_shared<triangle>(
        point3(3, -2, 1), direction3(0, 0, 4), direction3(0, 4, 0), back_green
    ));

    camera cam(
        1.0,
        400,
        100,
        50,
        colour(0.7, 0.8, 1.0),
        80,
        point3(0, 0, 9),
        point3(0, 0, 0)
    );

    cam.render(world);
}

void ellipses() {
    hittable_list world;

    const auto ellipse_material = std::make_shared<lambertian>(
        colour(0.8, 0.8, 0.0)
    );
    world.add(std::make_shared<ellipse>(
        point3(0, 0, 0),
        direction3(1, 0, 0),
        direction3(0, 1, 0),
        ellipse_material)
    );
    world.add(std::make_shared<ellipse>(
        point3(2.5, 0, 0),
        direction3(1, 0, 0),
        direction3(0, 2, -1),
        ellipse_material)
    );

    world.add(std::make_shared<disc>(
        point3(-2.5, 0, 0),
        direction3(1, 1, 0),
        direction3(0, 1, 0),
        1.5,
        ellipse_material)
    );

    camera cam(
        1.0,
        400,
        100,
        50,
        colour(0.7, 0.8, 1.0),
        80,
        point3(0, 0, 5),
        point3(0, 0, 0)
    );
    cam.render(world);
}

void simple_light() {
    hittable_list world;

    const auto pertext = std::make_shared<noise_texture>(4);
    const auto sphere_material = std::make_shared<lambertian>(pertext);
    world.add(
        std::make_shared<sphere>(point3(0,-1000,0), 1000, sphere_material)
    );
    world.add(std::make_shared<sphere>(point3(0,2,0), 2, sphere_material));

    const auto light = std::make_shared<diffuse_light>(colour(4, 4, 4));
    world.add(std::make_shared<quad>(
        point3(3, 1, -2), direction3(2, 0, 0), direction3(0, 2, 0), light
    ));
    world.add(std::make_shared<sphere>(point3(0, 7, 0), 2, light));

    camera cam(
        16.0 / 9.0,
        400,
        100,
        50,
        colour(0, 0, 0),
        20,
        point3(26, 3, 6),
        point3(0, 2, 0)
    );
    cam.render(world);
}

void cornell_box() {
    hittable_list world;

    const auto red = std::make_shared<lambertian>(colour(0.65, 0.05, 0.05));
    const auto white = std::make_shared<lambertian>(colour(0.73, 0.73, 0.73));
    const auto green = std::make_shared<lambertian>(colour(0.12, 0.45, 0.09));
    const auto light = std::make_shared<diffuse_light>(colour(25, 25, 25));

    world.add(std::make_shared<quad>(
        point3(555, 0, 0), direction3(0, 555, 0), direction3(0, 0, 555), green
    ));
    world.add(std::make_shared<quad>(
        point3(0, 0, 0), direction3(0, 555, 0), direction3(0, 0, 555), red
    ));
    world.add(std::make_shared<quad>(
        point3(343, 554, 332),
        direction3(-130, 0, 0),
        direction3(0, 0, -105),
        light
    ));
    world.add(std::make_shared<quad>(
        point3(0, 0, 0), direction3(555, 0, 0), direction3(0, 0, 555), white
    ));
    world.add(std::make_shared<quad>(
        point3(555,555,555), direction3(-555,0,0), direction3(0,0,-555), white
    ));
    world.add(std::make_shared<quad>(
        point3(0, 0, 555), direction3(555, 0, 0), direction3(0, 555, 0), white
    ));

    std::shared_ptr<hittable> box1 = box(
        point3(0,0,0), point3(165,330,165), white
    );
    box1 = std::make_shared<rotate_y>(box1, 15);
    box1 = std::make_shared<translate>(box1, direction3(265,0,295));
    world.add(box1);

    std::shared_ptr<hittable> box2 = box(
        point3(0,0,0), point3(165,165,165), white
    );
    box2 = std::make_shared<rotate_y>(box2, -18);
    box2 = std::make_shared<translate>(box2, direction3(130,0,65));
    world.add(box2);

    camera cam(
        1.0,
        600,
        1000,
        20,
        colour(0, 0, 0),
        40,
        point3(278, 278, -800),
        point3(278, 278, 0)
    );
    cam.render(world);
}

void cornell_smoke() {
    hittable_list world;

    auto red   = std::make_shared<lambertian>(colour(.65, .05, .05));
    auto white = std::make_shared<lambertian>(colour(.73, .73, .73));
    auto green = std::make_shared<lambertian>(colour(.12, .45, .15));
    auto light = std::make_shared<diffuse_light>(colour(7, 7, 7));

    world.add(std::make_shared<quad>(
        point3(555,0,0), direction3(0,555,0), direction3(0,0,555), green
    ));
    world.add(std::make_shared<quad>(
        point3(0,0,0), direction3(0,555,0), direction3(0,0,555), red
    ));
    world.add(std::make_shared<quad>(
        point3(113,554,127), direction3(330,0,0), direction3(0,0,305), light
    ));
    world.add(std::make_shared<quad>(
        point3(0,555,0), direction3(555,0,0), direction3(0,0,555), white
    ));
    world.add(std::make_shared<quad>(
        point3(0,0,0), direction3(555,0,0), direction3(0,0,555), white
    ));
    world.add(std::make_shared<quad>(
        point3(0,0,555), direction3(555,0,0), direction3(0,555,0), white
    ));

    std::shared_ptr<hittable> box1 = box(
        point3(0,0,0), point3(165,330,165), white
    );
    box1 = std::make_shared<rotate_y>(box1, 15);
    box1 = std::make_shared<translate>(box1, direction3(265,0,295));

    std::shared_ptr<hittable> box2 = box(
        point3(0,0,0), point3(165,165,165), white
    );
    box2 = std::make_shared<rotate_y>(box2, -18);
    box2 = std::make_shared<translate>(box2, direction3(130,0,65));

    world.add(std::make_shared<constant_medium>(box1, 0.01, colour(0,0,0)));
    world.add(std::make_shared<constant_medium>(box2, 0.01, colour(1,1,1)));

    camera cam {
        1.0,
        600,
        200,
        50,
        colour(0, 0, 0),
        40,
        point3(278, 278, -800),
        point3(278, 278, 0)
    };

    cam.render(world);
}

static void usage(const char* argv0) {
    std::cerr << "Usage: " << argv0 << " <scene>" << std::endl
        << "Valid scenes: 1, 2, 3, 4, 5, 6, 7, 8, 9, 10" << std::endl;
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
        case 4: perlin_spheres(); break;
        case 5: quads(); break;
        case 6: triangles(); break;
        case 7: ellipses(); break;
        case 8: simple_light(); break;
        case 9: cornell_box(); break;
        case 10: cornell_smoke(); break;
        default: usage(argv[0]); return 1;
    }
}
