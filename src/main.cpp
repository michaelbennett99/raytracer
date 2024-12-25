#include <memory>

#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"

int main() {
    hittable_list world;

    auto material_ground = std::make_shared<lambertian>(colour(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<lambertian>(colour(0.1, 0.2, 0.5));
    auto material_left = std::make_shared<dielectric>(1.0 / 1.33);
    auto material_right = std::make_shared<metal>(colour(0.8, 0.6, 0.2), 1.0);

    world.add(std::make_shared<sphere>(
        point3(0, -100.5, -1), 100, material_ground
    ));
    world.add(std::make_shared<sphere>(
        point3(0, 0, -1.2), 0.5, material_center
    ));
    world.add(std::make_shared<sphere>(
        point3(-1, 0, -1), 0.5, material_left
    ));
    world.add(std::make_shared<sphere>(
        point3(1, 0, -1), 0.5, material_right
    ));

    camera cam;

    cam.ar = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.render(world);
}
