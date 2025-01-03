#ifndef SAMPLER_H
#define SAMPLER_H

#include <memory>
#include <iostream>

#include "pixel_sampler.h"
#include "sampler_types.h"

class Sampler {
private:
    SamplerConfigPtr cfg;
    SamplerDataPtr data;
    PixelSamplerFactory sampler_factory;

public:
    Sampler() = delete;
    Sampler(const SamplerConfig& cfg, const SamplerData& data) :
        cfg(std::make_shared<SamplerConfigType>(cfg)),
        data(std::make_shared<SamplerDataType>(data)),
        sampler_factory(cfg.type())
    {}

    Sampler(
        const SamplerConfig& cfg,
        const ImageData& image_data,
        const Point3& lookfrom,
        const Point3& lookat,
        const Direction3& vup,
        double vfov,
        double defocus_angle,
        double focus_dist
    ) : cfg(std::make_shared<SamplerConfigType>(cfg)),
        data(std::make_shared<SamplerDataType>(
            image_data,
            lookfrom,
            lookat,
            vup,
            vfov,
            defocus_angle,
            focus_dist
        )),
        sampler_factory(cfg.type()) {}

    std::unique_ptr<PixelSampler> pixel(int i, int j) const {
        return sampler_factory(data, cfg, i, j);
    }

    const SamplerConfig& get_config() const {
        return *cfg;
    }
};

std::ostream& operator<<(std::ostream& os, const Sampler& s) {
    return os << s.get_config();
}

#endif // SAMPLER_H
