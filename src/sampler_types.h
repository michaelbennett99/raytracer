#ifndef SAMPLER_TYPES_H
#define SAMPLER_TYPES_H

#include <iostream>

#include "vec3.h"

struct SamplerConfig {
    int samples_per_pixel = 100;

    struct Random {
        bool enabled = false;
    } random;

    struct Adaptive {
        bool enabled = false;
        int burn_in = 64;
        int check_every = 64;
        double tolerance = 0.05;
        double critical_value = 1.96;
        double epsilon = 1e-16;
    } adaptive;
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
    point3 origin;
    point3 pixel00_loc;
    direction3 pixel_delta_u;
    direction3 pixel_delta_v;
    double defocus_angle;
    direction3 defocus_disk_u;
    direction3 defocus_disk_v;
};

#endif // SAMPLER_TYPES_H
