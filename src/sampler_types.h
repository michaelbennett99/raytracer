#ifndef SAMPLER_TYPES_H
#define SAMPLER_TYPES_H

#include <iostream>

#include "image.h"
#include "vec3.h"

enum class SamplerType {
    Random,
    AdaptiveRandom
};

struct SamplerConfig {
    int samples_per_pixel { 100 };

    struct Random {
        bool enabled { false };
    } random;

    struct Adaptive {
        bool enabled { false };
        int burn_in { 64 };
        int check_every { 64 };
        double tolerance { 0.05 };
        double critical_value { 1.96 };
        double epsilon { 1e-16 };
    } adaptive;

    SamplerType type() const {
        const auto settings_mask { (adaptive.enabled << 1) | random.enabled };
        switch (settings_mask) {
        case 0b01:
            return SamplerType::Random;
        case 0b11:
            return SamplerType::AdaptiveRandom;
        default:
            throw std::runtime_error("No sampler type enabled");
        }
    }
};

std::ostream& operator<<(std::ostream& os, const SamplerConfig::Random& r) {
    os << "Random(\n"
        << "\t\tenabled=" << r.enabled << "\n"
        << "\t)";
    return os;
}

std::ostream& operator<<(std::ostream& os, const SamplerConfig::Adaptive& a) {
    os << "Adaptive(\n"
        << "\t\tenabled=" << a.enabled << "\n"
        << "\t\tburn_in=" << a.burn_in << "\n"
        << "\t\tcheck_every=" << a.check_every << "\n"
        << "\t\ttolerance=" << a.tolerance << "\n"
        << "\t\tcritical_value=" << a.critical_value << "\n"
        << "\t\tepsilon=" << a.epsilon << "\n"
        << "\t)";
    return os;
}

std::ostream& operator<<(std::ostream& os, const SamplerConfig& cfg) {
    os << "SamplerConfig(\n"
        << "\tsamples_per_pixel=" << cfg.samples_per_pixel << "\n"
        << "\trandom=" << cfg.random << "\n"
        << "\tadaptive=" << cfg.adaptive << "\n"
        << ")";
    return os;
}

struct SamplerData {
    Point3 origin;
    Point3 pixel00_loc;
    Direction3 pixel_delta_u;
    Direction3 pixel_delta_v;
    double defocus_angle;
    Direction3 defocus_disk_u;
    Direction3 defocus_disk_v;

    SamplerData() = delete;
    SamplerData(
        const ImageData& image_data,
        const Point3& lookfrom,
        const Point3& lookat,
        const Direction3& vup,
        double vfov,
        double defocus_angle,
        double focus_dist
    ) {
        origin = lookfrom;

        // Viewport dimensions
        const auto theta = degrees_to_radians(vfov);
        const auto h = std::tan(theta / 2);
        const auto viewport_height = 2 * h * focus_dist;
        const auto viewport_width = viewport_height * image_data.aspect_ratio();

        // Calculate orthonormal basis
        const auto w = unit_vector(lookfrom - lookat);
        const auto u = unit_vector(cross(vup, w));
        const auto v = cross(w, u);

        // Edge vectors of viewport
        const auto viewport_u = viewport_width * u; // u is horizontal
        const auto viewport_v = viewport_height * -v; // v is vertical


        // Delta vectors from pixel to pixel
        pixel_delta_u = viewport_u / image_data.width;
        pixel_delta_v = viewport_v / image_data.height;

        // Location of upper left pixel
        const auto viewport_ul = origin
            - (focus_dist * w)
            - 0.5 * (viewport_u + viewport_v);
        pixel00_loc = viewport_ul
            + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Defocus disk
        const auto defocus_radius = focus_dist
            * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }
};

#endif // SAMPLER_TYPES_H
