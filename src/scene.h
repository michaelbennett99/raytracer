#ifndef SCENE_H
#define SCENE_H

#include <iostream>

#include "hittable_list.h"
#include "camera.h"
#include "sphere.h"
#include "sampler.h"
#include "bvh.h"
#include "quad.h"
#include "texture.h"
#include "movement.h"
#include "constant_medium.h"
#include "material.h"
#include "world.h"

class scene {
    private:
        std::shared_ptr<World> world;
        std::shared_ptr<camera> cam;

    public:
        scene() = default;
        scene(
            std::shared_ptr<World> world,
            std::shared_ptr<camera> cam
        ) : world(world), cam(cam) {}

        void render() {
            cam->render(*world);
        }
};

scene bouncing_spheres(
    std::shared_ptr<Sampler> sampler,
    std::vector<std::shared_ptr<Renderer>> renderers,
    double ar,
    int image_width
) {
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
        sampler,
        renderers,
        ar,
        image_width,
        50,
        20,
        point3(13,2,3),
        point3(0,0,0),
        direction3(0,1,0),
        0.6,
        10.0
    );

    return scene(
        std::make_shared<World>(world, colour(0.7, 0.8, 1.0)),
        std::make_shared<camera>(cam)
    );
}

scene checkered_spheres(
    std::shared_ptr<Sampler> sampler,
    std::vector<std::shared_ptr<Renderer>> renderers,
    double ar,
    int image_width
) {
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
        sampler,
        renderers,
        ar,
        image_width,
        50,
        20,
        point3(13, 2, 3),
        point3(0, 0, 0),
        direction3(0, 1, 0)
    );

    return scene(
        std::make_shared<World>(world, colour(0.7, 0.8, 1.0)),
        std::make_shared<camera>(cam)
    );
}

scene earth(
    std::shared_ptr<Sampler> sampler,
    std::vector<std::shared_ptr<Renderer>> renderers,
    double ar,
    int image_width
) {
    hittable_list world;

    const auto earth_texture = std::make_shared<image_texture>("map.jpg");
    const auto earth_material = std::make_shared<lambertian>(earth_texture);
    const auto globe = std::make_shared<sphere>(
        point3(0, 0, 0), 2, earth_material
    );

    world.add(globe);

    camera cam(
        sampler,
        renderers,
        ar,
        image_width,
        50,
        20,
        point3(0, 0, 12),
        point3(0, 0, 0)
    );

    return scene(
        std::make_shared<World>(world, colour(0.7, 0.8, 1.0)),
        std::make_shared<camera>(cam)
    );
}

scene perlin_spheres(
    std::shared_ptr<Sampler> sampler,
    std::vector<std::shared_ptr<Renderer>> renderers,
    double ar,
    int image_width
) {
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
        sampler,
        renderers,
        ar,
        image_width,
        50,
        20,
        point3(13, 2, 3),
        point3(0, 0, 0),
        direction3(0, 1, 0)
    );

    return scene(
        std::make_shared<World>(world, colour(0.7, 0.8, 1.0)),
        std::make_shared<camera>(cam)
    );
}

scene quads(
    std::shared_ptr<Sampler> sampler,
    std::vector<std::shared_ptr<Renderer>> renderers,
    double ar,
    int image_width
) {
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
        sampler,
        renderers,
        ar,
        image_width,
        50,
        80,
        point3(0, 0, 9),
        point3(0, 0, 0)
    );

    return scene(
        std::make_shared<World>(world, colour(0.7, 0.8, 1.0)),
        std::make_shared<camera>(cam)
    );
}

scene triangles(
    std::shared_ptr<Sampler> sampler,
    std::vector<std::shared_ptr<Renderer>> renderers,
    double ar,
    int image_width
) {
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
        sampler,
        renderers,
        ar,
        image_width,
        50,
        80,
        point3(0, 0, 9),
        point3(0, 0, 0)
    );

    return scene(
        std::make_shared<World>(world, colour(0.7, 0.8, 1.0)),
        std::make_shared<camera>(cam)
    );
}

scene ellipses(
    std::shared_ptr<Sampler> sampler,
    std::vector<std::shared_ptr<Renderer>> renderers,
    double ar,
    int image_width
) {
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
        sampler,
        renderers,
        ar,
        image_width,
        50,
        80,
        point3(0, 0, 5),
        point3(0, 0, 0)
    );

    return scene(
        std::make_shared<World>(world, colour(0.7, 0.8, 1.0)),
        std::make_shared<camera>(cam)
    );
}

scene simple_light(
    std::shared_ptr<Sampler> sampler,
    std::vector<std::shared_ptr<Renderer>> renderers,
    double ar,
    int image_width
) {
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
        sampler,
        renderers,
        ar,
        image_width,
        50,
        20,
        point3(26, 3, 6),
        point3(0, 2, 0)
    );

    return scene(
        std::make_shared<World>(world, colour(0, 0, 0)),
        std::make_shared<camera>(cam)
    );
}

scene cornell_box(
    std::shared_ptr<Sampler> sampler,
    std::vector<std::shared_ptr<Renderer>> renderers,
    double ar,
    int image_width
) {
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

    // Add light behind the camera
    world.add(std::make_shared<quad>(
        point3(0, 0, -1000), direction3(555, 0, 0), direction3(0, 555, 0), light
    ));

    camera cam(
        sampler,
        renderers,
        ar,
        image_width,
        20,
        40,
        point3(278, 278, -800),
        point3(278, 278, 0)
    );

    return scene(
        std::make_shared<World>(world),
        std::make_shared<camera>(cam)
    );
}

scene cornell_smoke(
    std::shared_ptr<Sampler> sampler,
    std::vector<std::shared_ptr<Renderer>> renderers,
    double ar,
    int image_width
) {
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
        sampler,
        renderers,
        ar,
        image_width,
        50,
        40,
        point3(278, 278, -800),
        point3(278, 278, 0)
    };

    return scene(
        std::make_shared<World>(world),
        std::make_shared<camera>(cam)
    );
}

scene final_scene(
    std::shared_ptr<Sampler> sampler,
    std::vector<std::shared_ptr<Renderer>> renderers,
    double ar,
    int image_width
) {
    // Ground of boxes
    hittable_list boxes1;
    auto ground = std::make_shared<lambertian>(colour(0.48, 0.83, 0.53));
    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = gen_rand::random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0,y0,z0), point3(x1,y1,z1), ground));
        }
    }

    hittable_list world;

    // Bounding volume hierarchy for the boxes
    world.add(std::make_shared<bvh_node>(boxes1));

    // Light
    auto light = std::make_shared<diffuse_light>(colour(7, 7, 7));
    world.add(std::make_shared<quad>(
        point3(123,554,147), direction3(300,0,0), direction3(0,0,265), light
    ));

    // Moving sphere
    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + direction3(30,0,0);
    auto sphere_material = std::make_shared<lambertian>(colour(0.7, 0.3, 0.1));
    world.add(std::make_shared<sphere>(center1, center2, 50, sphere_material));

    // Static spheres
    const auto dielectric_material = std::make_shared<dielectric>(1.5);
    const auto metal_material = std::make_shared<metal>(
        colour(0.8, 0.8, 0.9), 1.0
    );
    world.add(std::make_shared<sphere>(
        point3(260, 150, 45), 50, dielectric_material
    ));
    world.add(std::make_shared<sphere>(
        point3(0, 150, 145), 50, metal_material
    ));

    // SSR sphere
    auto boundary = std::make_shared<sphere>(
        point3(360,150,145), 70, dielectric_material
    );
    world.add(boundary);
    world.add(std::make_shared<constant_medium>(
        boundary, 0.2, colour(0.2, 0.4, 0.9)
    ));
    boundary = std::make_shared<sphere>(
        point3(0,0,0), 5000, dielectric_material
    );
    world.add(std::make_shared<constant_medium>(
        boundary, .0001, colour(1,1,1)
    ));

    // World sphere
    auto map = std::make_shared<lambertian>(std::make_shared<image_texture>(
        "map.jpg"
    ));
    world.add(std::make_shared<sphere>(point3(400,200,400), 100, map));

    // Perlin sphere
    const auto pertext = std::make_shared<noise_texture>(0.2);
    const auto pertext_material = std::make_shared<lambertian>(pertext);
    world.add(std::make_shared<sphere>(
        point3(220,280,300), 80, pertext_material
    ));

    // Box of spheres
    hittable_list boxes2;
    const auto white = std::make_shared<lambertian>(colour(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(std::make_shared<sphere>(point3::random(0,165), 10, white));
    }

    // Bounding volume hierarchy for the box of spheres
    world.add(std::make_shared<translate>(
        std::make_shared<rotate_y>(
            std::make_shared<bvh_node>(boxes2), 15),
            direction3(-100,270,395)
        )
    );

    camera cam(
        sampler,
        renderers,
        ar,
        image_width,
        50,
        40,
        point3(478, 278, -600),
        point3(278, 278, 0)
    );

    return scene(
        std::make_shared<World>(world),
        std::make_shared<camera>(cam)
    );
}

#endif
