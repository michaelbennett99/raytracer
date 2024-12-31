#ifndef SAMPLER_H
#define SAMPLER_H

#include <memory>
#include <iostream>

#include "pixel_sampler.h"
#include "sampler_types.h"

class Sampler {
private:
    using SamplerConfigPtr = std::shared_ptr<SamplerConfig>;
    using SamplerDataPtr = std::shared_ptr<SamplerData>;

    bool initialised = false;
    SamplerConfigPtr cfg;
    SamplerDataPtr data;
    std::function<
        std::shared_ptr<PixelSampler>(
            SamplerDataPtr,
            SamplerConfigPtr,
            int,
            int
        )
    > sampler_factory;

    // Returns the function that creates the pixel sampler based on the config
    void set_pixel_sampler_factory() {
        if (cfg->random.enabled) {
            if (cfg->adaptive.enabled) {
                sampler_factory = [](auto data, auto cfg, int i, int j) {
                    return std::make_shared<AdaptiveRandomPixelSampler>(
                        data, cfg, i, j
                    );
                };
            } else {
                sampler_factory = [](auto data, auto cfg, int i, int j) {
                    return std::make_shared<RandomPixelSampler>(data, cfg, i, j);
                };
            }
        } else {
            throw std::runtime_error("No pixel sampler enabled");
        }
    }

public:
    Sampler(const SamplerConfig& cfg)
        : cfg(std::make_shared<SamplerConfig>(cfg))
    {
        set_pixel_sampler_factory();
    }
    ~Sampler() = default;

    std::shared_ptr<PixelSampler> pixel(int i, int j) {
#ifndef NDEBUG
        if (!is_initialised()) {
            throw std::runtime_error("Sampler not initialised");
        }
#endif
        return sampler_factory(data, cfg, i, j);
    }

    void initialise(const SamplerData& data) {
        this->data = std::make_shared<SamplerData>(data);
        initialised = true;
    }

    const SamplerConfig& get_config() const {
        return *cfg;
    }

    bool is_initialised() const {
        return initialised;
    }
};

std::ostream& operator<<(std::ostream& os, const Sampler& s) {
    return os << s.get_config();
}

#endif // SAMPLER_H
