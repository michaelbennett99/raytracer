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
#include "renderer.h"

class Scene {
    private:
        std::shared_ptr<World> world;
        std::shared_ptr<Camera> cam;

    public:
        Scene() = default;
        Scene(
            std::shared_ptr<World> world,
            std::shared_ptr<Camera> cam
        ) : world(world), cam(cam) {}

        std::map<RendererType, Image> render() {
            cam->render(*world);
            return cam->get_results();
        }
};

Scene bouncing_spheres(
    const SamplerConfig& sampler_config,
    const std::vector<RendererType>& renderer_types,
    double ar,
    int image_width
) {
    HittableList world;

    auto ground_material = std::make_shared<Lambertian>(Colour(0.5, 0.5, 0.5));
    world.add(std::make_shared<Sphere>(
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
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Colour::random() * Colour::random();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    auto center2 = center
                        + direction3(0, gen_rand::random_double(0, 0.5), 0);

                    world.add(std::make_shared<Sphere>(
                            center, center2, 0.2, sphere_material
                    ));
                } else if (choose_mat < 0.95) {
                    // Metal
                    auto albedo = Colour::random(0.5, 1);
                    auto fuzz = gen_rand::random_double(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);

                    world.add(std::make_shared<Sphere>(
                        center, 0.2, sphere_material
                    ));
                } else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_shared<Sphere>(
                        center, 0.2, sphere_material
                    ));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Colour(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));

    world = HittableList(std::make_shared<BVHNode>(world));

    auto cam = std::make_shared<Camera>(
        sampler_config,
        renderer_types,
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

    return Scene(
        std::make_shared<World>(world, Colour(0.7, 0.8, 1.0)),
        cam
    );
}

Scene checkered_spheres(
    const SamplerConfig& sampler_config,
    const std::vector<RendererType>& renderer_types,
    double ar,
    int image_width
) {
    HittableList world;

    const auto checker = std::make_shared<checker_texture>(
        0.32,
        Colour(0.2, 0.3, 0.1),
        Colour(0.9, 0.9, 0.9)
    );

    const auto sphere_material = std::make_shared<Lambertian>(checker);

    world.add(std::make_shared<Sphere>(point3(0,-10,0), 10, sphere_material));
    world.add(std::make_shared<Sphere>(point3(0,10,0), 10, sphere_material));

    auto cam = std::make_shared<Camera>(
        sampler_config,
        renderer_types,
        ar,
        image_width,
        50,
        20,
        point3(13, 2, 3),
        point3(0, 0, 0),
        direction3(0, 1, 0)
    );

    return Scene(
        std::make_shared<World>(world, Colour(0.7, 0.8, 1.0)),
        cam
    );
}

Scene earth(
    const SamplerConfig& sampler_config,
    const std::vector<RendererType>& renderer_types,
    double ar,
    int image_width
) {
    HittableList world;

    const auto earth_texture = std::make_shared<image_texture>("map.jpg");
    const auto earth_material = std::make_shared<Lambertian>(earth_texture);
    const auto globe = std::make_shared<Sphere>(
        point3(0, 0, 0), 2, earth_material
    );

    world.add(globe);

    auto cam = std::make_shared<Camera>(
        sampler_config,
        renderer_types,
        ar,
        image_width,
        50,
        20,
        point3(0, 0, 12),
        point3(0, 0, 0)
    );

    return Scene(
        std::make_shared<World>(world, Colour(0.7, 0.8, 1.0)),
        cam
    );
}

Scene perlin_spheres(
    const SamplerConfig& sampler_config,
    const std::vector<RendererType>& renderer_types,
    double ar,
    int image_width
) {
    HittableList world;

    const auto pertext = std::make_shared<noise_texture>(4);
    const auto sphere_material = std::make_shared<Lambertian>(pertext);

    world.add(
        std::make_shared<Sphere>(point3(0, -1000, 0), 1000, sphere_material)
    );
    world.add(
        std::make_shared<Sphere>(point3(0, 2, 0), 2, sphere_material)
    );

    auto cam = std::make_shared<Camera>(
        sampler_config,
        renderer_types,
        ar,
        image_width,
        50,
        20,
        point3(13, 2, 3),
        point3(0, 0, 0),
        direction3(0, 1, 0)
    );

    return Scene(
        std::make_shared<World>(world, Colour(0.7, 0.8, 1.0)),
        cam
    );
}

Scene quads(
    const SamplerConfig& sampler_config,
    const std::vector<RendererType>& renderer_types,
    double ar,
    int image_width
) {
    HittableList world;

    // Materials
    const auto left_red = std::make_shared<Lambertian>(Colour(1, 0.2, 0.2));
    const auto back_green = std::make_shared<Lambertian>(Colour(0.2, 1, 0.2));
    const auto right_blue = std::make_shared<Lambertian>(Colour(0.2, 0.2, 1));
    const auto upper_orange = std::make_shared<Lambertian>(Colour(1, 0.5, 0));
    const auto lower_teal = std::make_shared<Lambertian>(Colour(0.2, 0.8, 0.8));

    // Quads
    world.add(std::make_shared<Quad>(
        point3(-3, -2, 5), direction3(0, 0, -4), direction3(0, 4, 0), left_red
    ));
    world.add(std::make_shared<Quad>(
        point3(-2, -2, 0), direction3(4, 0, 0), direction3(0, 4, 0), right_blue
    ));
    world.add(std::make_shared<Quad>(
        point3(3, -2, 1), direction3(0, 0, 4), direction3(0, 4, 0), back_green
    ));
    world.add(std::make_shared<Quad>(
        point3(-2, 3, 1), direction3(4, 0, 0), direction3(0, 0, 4), upper_orange
    ));
    world.add(std::make_shared<Quad>(
        point3(-2, -3, 1), direction3(4, 0, 0), direction3(0, 0, 4), lower_teal
    ));

    auto cam = std::make_shared<Camera>(
        sampler_config,
        renderer_types,
        ar,
        image_width,
        50,
        80,
        point3(0, 0, 9),
        point3(0, 0, 0)
    );

    return Scene(
        std::make_shared<World>(world, Colour(0.7, 0.8, 1.0)),
        cam
    );
}

Scene triangles(
    const SamplerConfig& sampler_config,
    const std::vector<RendererType>& renderer_types,
    double ar,
    int image_width
) {
    HittableList world;

    const auto left_red = std::make_shared<Lambertian>(Colour(1, 0.2, 0.2));
    const auto back_green = std::make_shared<Lambertian>(Colour(0.2, 1, 0.2));
    const auto right_blue = std::make_shared<Lambertian>(Colour(0.2, 0.2, 1));

    world.add(std::make_shared<Triangle>(
        point3(-3, -2, 5), direction3(0, 0, -4), direction3(0, 4, 0), left_red
    ));
    world.add(std::make_shared<Triangle>(
        point3(-2, -2, 0), direction3(4, 0, 0), direction3(0, 4, 0), right_blue
    ));
    world.add(std::make_shared<Triangle>(
        point3(3, -2, 1), direction3(0, 0, 4), direction3(0, 4, 0), back_green
    ));

    auto cam = std::make_shared<Camera>(
        sampler_config,
        renderer_types,
        ar,
        image_width,
        50,
        80,
        point3(0, 0, 9),
        point3(0, 0, 0)
    );

    return Scene(
        std::make_shared<World>(world, Colour(0.7, 0.8, 1.0)),
        cam
    );
}

Scene ellipses(
    const SamplerConfig& sampler_config,
    const std::vector<RendererType>& renderer_types,
    double ar,
    int image_width
) {
    HittableList world;

    const auto ellipse_material = std::make_shared<Lambertian>(
        Colour(0.8, 0.8, 0.0)
    );
    world.add(std::make_shared<Ellipse>(
        point3(0, 0, 0),
        direction3(1, 0, 0),
        direction3(0, 1, 0),
        ellipse_material)
    );
    world.add(std::make_shared<Ellipse>(
        point3(2.5, 0, 0),
        direction3(1, 0, 0),
        direction3(0, 2, -1),
        ellipse_material)
    );

    world.add(std::make_shared<Disc>(
        point3(-2.5, 0, 0),
        direction3(1, 1, 0),
        direction3(0, 1, 0),
        1.5,
        ellipse_material)
    );

    auto cam = std::make_shared<Camera>(
        sampler_config,
        renderer_types,
        ar,
        image_width,
        50,
        80,
        point3(0, 0, 5),
        point3(0, 0, 0)
    );

    return Scene(
        std::make_shared<World>(world, Colour(0.7, 0.8, 1.0)),
        cam
    );
}

Scene simple_light(
    const SamplerConfig& sampler_config,
    const std::vector<RendererType>& renderer_types,
    double ar,
    int image_width
) {
    HittableList world;

    const auto pertext = std::make_shared<noise_texture>(4);
    const auto sphere_material = std::make_shared<Lambertian>(pertext);
    world.add(
        std::make_shared<Sphere>(point3(0,-1000,0), 1000, sphere_material)
    );
    world.add(std::make_shared<Sphere>(point3(0,2,0), 2, sphere_material));

    const auto light = std::make_shared<DiffuseLight>(Colour(4, 4, 4));
    world.add(std::make_shared<Quad>(
        point3(3, 1, -2), direction3(2, 0, 0), direction3(0, 2, 0), light
    ));
    world.add(std::make_shared<Sphere>(point3(0, 7, 0), 2, light));

    auto cam = std::make_shared<Camera>(
        sampler_config,
        renderer_types,
        ar,
        image_width,
        50,
        20,
        point3(26, 3, 6),
        point3(0, 2, 0)
    );

    return Scene(
        std::make_shared<World>(world, Colour(0, 0, 0)),
        cam
    );
}

Scene cornell_box(
    const SamplerConfig& sampler_config,
    const std::vector<RendererType>& renderer_types,
    double ar,
    int image_width
) {
    HittableList world;

    const auto red = std::make_shared<Lambertian>(Colour(0.65, 0.05, 0.05));
    const auto white = std::make_shared<Lambertian>(Colour(0.73, 0.73, 0.73));
    const auto green = std::make_shared<Lambertian>(Colour(0.12, 0.45, 0.09));
    const auto light = std::make_shared<DiffuseLight>(Colour(25, 25, 25));

    world.add(std::make_shared<Quad>(
        point3(555, 0, 0), direction3(0, 555, 0), direction3(0, 0, 555), green
    ));
    world.add(std::make_shared<Quad>(
        point3(0, 0, 0), direction3(0, 555, 0), direction3(0, 0, 555), red
    ));
    world.add(std::make_shared<Quad>(
        point3(343, 554, 332),
        direction3(-130, 0, 0),
        direction3(0, 0, -105),
        light
    ));
    world.add(std::make_shared<Quad>(
        point3(0, 0, 0), direction3(555, 0, 0), direction3(0, 0, 555), white
    ));
    world.add(std::make_shared<Quad>(
        point3(555,555,555), direction3(-555,0,0), direction3(0,0,-555), white
    ));
    world.add(std::make_shared<Quad>(
        point3(0, 0, 555), direction3(555, 0, 0), direction3(0, 555, 0), white
    ));

    std::shared_ptr<Hittable> box1 = box(
        point3(0,0,0), point3(165,330,165), white
    );
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, direction3(265,0,295));
    world.add(box1);

    std::shared_ptr<Hittable> box2 = box(
        point3(0,0,0), point3(165,165,165), white
    );
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, direction3(130,0,65));
    world.add(box2);

    // Add light behind the camera
    world.add(std::make_shared<Quad>(
        point3(0, 0, -1000), direction3(555, 0, 0), direction3(0, 555, 0), light
    ));

    auto cam = std::make_shared<Camera>(
        sampler_config,
        renderer_types,
        ar,
        image_width,
        20,
        40,
        point3(278, 278, -800),
        point3(278, 278, 0)
    );

    return Scene(
        std::make_shared<World>(world),
        cam
    );
}

Scene cornell_smoke(
    const SamplerConfig& sampler_config,
    const std::vector<RendererType>& renderer_types,
    double ar,
    int image_width
) {
    HittableList world;

    auto red   = std::make_shared<Lambertian>(Colour(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Colour(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Colour(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Colour(7, 7, 7));

    world.add(std::make_shared<Quad>(
        point3(555,0,0), direction3(0,555,0), direction3(0,0,555), green
    ));
    world.add(std::make_shared<Quad>(
        point3(0,0,0), direction3(0,555,0), direction3(0,0,555), red
    ));
    world.add(std::make_shared<Quad>(
        point3(113,554,127), direction3(330,0,0), direction3(0,0,305), light
    ));
    world.add(std::make_shared<Quad>(
        point3(0,555,0), direction3(555,0,0), direction3(0,0,555), white
    ));
    world.add(std::make_shared<Quad>(
        point3(0,0,0), direction3(555,0,0), direction3(0,0,555), white
    ));
    world.add(std::make_shared<Quad>(
        point3(0,0,555), direction3(555,0,0), direction3(0,555,0), white
    ));

    std::shared_ptr<Hittable> box1 = box(
        point3(0,0,0), point3(165,330,165), white
    );
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, direction3(265,0,295));

    std::shared_ptr<Hittable> box2 = box(
        point3(0,0,0), point3(165,165,165), white
    );
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, direction3(130,0,65));

    world.add(std::make_shared<ConstantMedium>(box1, 0.01, Colour(0,0,0)));
    world.add(std::make_shared<ConstantMedium>(box2, 0.01, Colour(1,1,1)));

    auto cam = std::make_shared<Camera>(
        sampler_config,
        renderer_types,
        ar,
        image_width,
        50,
        40,
        point3(278, 278, -800),
        point3(278, 278, 0)
    );

    return Scene(
        std::make_shared<World>(world),
        cam
    );
}

Scene final_scene(
    const SamplerConfig& sampler_config,
    const std::vector<RendererType>& renderer_types,
    double ar,
    int image_width
) {
    // Ground of boxes
    HittableList boxes1;
    auto ground = std::make_shared<Lambertian>(Colour(0.48, 0.83, 0.53));
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

    HittableList world;

    // Bounding volume hierarchy for the boxes
    world.add(std::make_shared<BVHNode>(boxes1));

    // Light
    auto light = std::make_shared<DiffuseLight>(Colour(7, 7, 7));
    world.add(std::make_shared<Quad>(
        point3(123,554,147), direction3(300,0,0), direction3(0,0,265), light
    ));

    // Moving Sphere
    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + direction3(30,0,0);
    auto sphere_material = std::make_shared<Lambertian>(Colour(0.7, 0.3, 0.1));
    world.add(std::make_shared<Sphere>(center1, center2, 50, sphere_material));

    // Static spheres
    const auto dielectric_material = std::make_shared<Dielectric>(1.5);
    const auto metal_material = std::make_shared<Metal>(
        Colour(0.8, 0.8, 0.9), 1.0
    );
    world.add(std::make_shared<Sphere>(
        point3(260, 150, 45), 50, dielectric_material
    ));
    world.add(std::make_shared<Sphere>(
        point3(0, 150, 145), 50, metal_material
    ));

    // SSR Sphere
    auto boundary = std::make_shared<Sphere>(
        point3(360,150,145), 70, dielectric_material
    );
    world.add(boundary);
    world.add(std::make_shared<ConstantMedium>(
        boundary, 0.2, Colour(0.2, 0.4, 0.9)
    ));
    boundary = std::make_shared<Sphere>(
        point3(0,0,0), 5000, dielectric_material
    );
    world.add(std::make_shared<ConstantMedium>(
        boundary, .0001, Colour(1,1,1)
    ));

    // World Sphere
    auto map = std::make_shared<Lambertian>(std::make_shared<image_texture>(
        "map.jpg"
    ));
    world.add(std::make_shared<Sphere>(point3(400,200,400), 100, map));

    // Perlin Sphere
    const auto pertext = std::make_shared<noise_texture>(0.2);
    const auto pertext_material = std::make_shared<Lambertian>(pertext);
    world.add(std::make_shared<Sphere>(
        point3(220,280,300), 80, pertext_material
    ));

    // Box of spheres
    HittableList boxes2;
    const auto white = std::make_shared<Lambertian>(Colour(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(std::make_shared<Sphere>(point3::random(0,165), 10, white));
    }

    // Bounding volume hierarchy for the box of spheres
    world.add(std::make_shared<Translate>(
        std::make_shared<RotateY>(
            std::make_shared<BVHNode>(boxes2), 15),
            direction3(-100,270,395)
        )
    );

    auto cam = std::make_shared<Camera>(
        sampler_config,
        renderer_types,
        ar,
        image_width,
        50,
        40,
        point3(478, 278, -600),
        point3(278, 278, 0)
    );

    return Scene(
        std::make_shared<World>(world),
        cam
    );
}

#endif
