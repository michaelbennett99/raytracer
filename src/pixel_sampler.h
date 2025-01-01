#ifndef PIXEL_SAMPLER_H
#define PIXEL_SAMPLER_H

#include <memory>

#include "colour.h"
#include "sampler_types.h"
#include "ray.h"

// Pixel sampler class
class PixelSampler : public std::enable_shared_from_this<PixelSampler> {
protected:
    int samples_ = 0;
    std::shared_ptr<SamplerData> data;
    std::shared_ptr<SamplerConfig> cfg;
    int i, j;

    point3 get_pixel_point(const direction3& offset) const {
        return data->pixel00_loc
            + (i + offset.x()) * data->pixel_delta_u
            + (j + offset.y()) * data->pixel_delta_v;
    }

    point3 sample_defocus_disk() const {
        if (data->defocus_angle <= 0) {
            return data->origin;
        }
        const auto p = random_in_unit_disk();
        return data->origin
            + (p[0] * data->defocus_disk_u)
            + (p[1] * data->defocus_disk_v);
    }

    virtual point3 sample_pixel() const = 0;

public:
    PixelSampler() = default;
    PixelSampler(
        std::shared_ptr<SamplerData> data,
        std::shared_ptr<SamplerConfig> cfg,
        int i,
        int j
    ) : data(data), cfg(cfg), i(i), j(j) {}
    virtual ~PixelSampler() = default;

    virtual bool has_next_sample() const = 0;
    virtual void add_sample(const colour& sample) {}
    virtual ray sample() {
        const auto pixel_sample = sample_pixel();
        const auto ray_origin = sample_defocus_disk();
        const auto ray_direction = pixel_sample - ray_origin;
        const auto ray_time = gen_rand::random_double(0, 1);
        samples_++;
        return ray(ray_origin, ray_direction, ray_time);
    };

    int samples() const { return samples_; }
    int max_samples() const { return cfg->samples_per_pixel; }
    double sampling_density() const {
        return static_cast<double>(samples()) / max_samples();
    }

    class Iterator {
    private:
        std::shared_ptr<PixelSampler> pixel_sampler;
        ray current_ray;

    public:
        explicit Iterator(
            std::shared_ptr<PixelSampler> pixel_sampler
        ) : pixel_sampler(pixel_sampler) {
            if (pixel_sampler) {
                current_ray = pixel_sampler->sample();
            }
        }

        Iterator& operator++() {
            if (pixel_sampler && pixel_sampler->has_next_sample()) {
                current_ray = pixel_sampler->sample();
            } else {
                pixel_sampler = nullptr;
            }
            return *this;
        }

        bool operator==(const Iterator& other) const {
            return pixel_sampler == other.pixel_sampler;
        }
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
        const ray& operator*() const { return current_ray; }
        const ray* operator->() const { return &current_ray; }
    };

    Iterator begin() {
        return Iterator(shared_from_this());
    }

    Iterator end() const {
        return Iterator(nullptr);
    }
};

class RandomPixelSampler : public virtual PixelSampler {
private:
    static direction3 sample_square() {
        return direction3(
            gen_rand::random_double(-0.5, 0.5),
            gen_rand::random_double(-0.5, 0.5),
            0
        );
    }

    point3 sample_pixel() const override {
        return get_pixel_point(sample_square());
    }

public:
    RandomPixelSampler() = default;
    RandomPixelSampler(
        std::shared_ptr<SamplerData> data,
        std::shared_ptr<SamplerConfig> cfg,
        int i,
        int j
    ) : PixelSampler(data, cfg, i, j) {};

    bool has_next_sample() const override {
        return samples() < cfg->samples_per_pixel;
    }
};

class AdaptiveRandomPixelSampler : public RandomPixelSampler {
private:
    struct Data {
        point3 s1{0,0,0};  // Sum for each channel
        point3 s2{0,0,0};  // Sum of squares for each channel
    };

    Data sampling_data;

    point3 mean() const {
        if (samples() == 0) {
            return point3(infinity_d, infinity_d, infinity_d);
        }
        return sampling_data.s1 / samples();
    }

    point3 variance() const {
        if (samples() <= 1) {
            return point3(infinity_d, infinity_d, infinity_d);
        }
        const auto s1_squared = sampling_data.s1 * sampling_data.s1;
        const auto n = static_cast<double>(samples());
        const auto factor = 1.0 / (n - 1);
        return factor * (sampling_data.s2 - (s1_squared / n));
    }

    bool should_continue() const {
        if (
            samples() < cfg->adaptive.burn_in
            || samples() % cfg->adaptive.check_every != 0
        ) return true;
        const auto mu = mean();
        const auto var = variance();

        // Check convergence for each channel
        for (int i = 0; i < 3; i++) {
            if (mu[i] < cfg->adaptive.epsilon) continue;

            const auto relative_error = std::sqrt(var[i] / samples())
                * cfg->adaptive.critical_value / mu[i];

            if (relative_error >= cfg->adaptive.tolerance) {
                return true;
            }
        }
        return false;
    }

public:
    AdaptiveRandomPixelSampler(
        std::shared_ptr<SamplerData> data,
        std::shared_ptr<SamplerConfig> cfg,
        int i,
        int j
    ) : PixelSampler(data, cfg, i, j) {};

    void add_sample(const colour& sample) override {
        sampling_data.s1 += sample;
        sampling_data.s2 += sample * sample;
    }

    bool has_next_sample() const override {
        return RandomPixelSampler::has_next_sample()
            && should_continue();
    }
};

class PixelSamplerFactory {
private:
    const SamplerType type_;

public:
    explicit PixelSamplerFactory(SamplerType type) : type_(type) {}

    std::shared_ptr<PixelSampler> operator()(
        std::shared_ptr<SamplerData> data,
        std::shared_ptr<SamplerConfig> cfg,
        int i,
        int j
    ) const {
        switch (type_) {
        case SamplerType::Random:
            return std::make_shared<RandomPixelSampler>(data, cfg, i, j);
        case SamplerType::AdaptiveRandom:
            return std::make_shared<AdaptiveRandomPixelSampler>(data, cfg, i, j);
        default:
            throw std::runtime_error("Unknown sampler type");
        }
    }
};

#endif // PIXEL_SAMPLER_H
