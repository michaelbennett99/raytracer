#ifndef SAMPLER_H
#define SAMPLER_H

#include <iterator>

#include "colour.h"
#include "hittable.h"
#include "vec3.h"
#include "ray.h"
#include "raytracing.h"

class sampler;
class ray_iterator;

struct sample_range {
    sampler* s;
    int i, j;
    ray_iterator begin() const;
    ray_iterator end() const;
};

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

class sampler {
    private:
        bool initialised = false;
        int samples_ = 0;;

    protected:
        // Data we need to sample rays
        point3 origin;
        point3 pixel00_loc;
        direction3 pixel_delta_u;
        direction3 pixel_delta_v;
        double defocus_angle;
        direction3 defocus_disk_u;
        direction3 defocus_disk_v;
        SamplerConfig cfg;

        static direction3 sample_square() {
            return direction3(
                gen_rand::random_double(-0.5, 0.5),
                gen_rand::random_double(-0.5, 0.5),
                0
            );
        }

        point3 defocus_disk_sample() const {
            const auto p = random_in_unit_disk();
            return origin + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        virtual point3 sample_pixel(int i, int j) const = 0;

    public:
        sampler() = default;
        virtual ~sampler() = default;

        virtual void set_config(const SamplerConfig& cfg) {
            this->cfg = cfg;
        }

        virtual SamplerConfig get_config() const {
            return cfg;
        }

        virtual bool has_next_sample(int i, int j) const = 0;
        virtual void add_sample(const colour& sample) {}
        virtual void clear() {
            samples_ = 0;
        }

        virtual colour sampling_density_colour() const = 0;

        ray get_ray(int i, int j) {
            samples_++;
            const auto pixel_sample = sample_pixel(i, j);
            const auto ray_origin = (defocus_angle <= 0)
                ? origin
                : defocus_disk_sample();
            const auto ray_direction = pixel_sample - ray_origin;
            const auto ray_time = gen_rand::random_double(0, 1);

            return ray(ray_origin, ray_direction, ray_time);
        }

        sample_range samples(int i, int j) {
            return {this, i, j};
        }

        void initialise(
            point3 origin,
            point3 pixel00_loc,
            direction3 pixel_delta_u,
            direction3 pixel_delta_v,
            double defocus_angle,
            direction3 defocus_disk_u,
            direction3 defocus_disk_v
        ) {
            this->origin = origin;
            this->pixel00_loc = pixel00_loc;
            this->pixel_delta_u = pixel_delta_u;
            this->pixel_delta_v = pixel_delta_v;
            this->defocus_angle = defocus_angle;
            this->defocus_disk_u = defocus_disk_u;
            this->defocus_disk_v = defocus_disk_v;
            initialised = true;
        }

        bool is_initialised() const {
            return initialised;
        }
        int samples() const {
            return samples_;
        }
};

std::ostream& operator<<(std::ostream& os, const sampler& s) {
    return os << s.get_config();
}

class ray_iterator {
    private:
        sampler* sampler_ptr;
        int pixel_i, pixel_j;
        ray current_ray;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = ray;
        using difference_type = std::ptrdiff_t;
        using pointer = const ray*;
        using reference = const ray&;

        ray_iterator(
            sampler* s, int i, int j
        ) : sampler_ptr(s),
            pixel_i(i),
            pixel_j(j) {
            if (sampler_ptr) {
                sampler_ptr->clear();
                current_ray = sampler_ptr->get_ray(pixel_i, pixel_j);
            }
        }

        reference operator*() const { return current_ray; }
        pointer operator->() const { return &current_ray; }

        ray_iterator& operator++() {
            if (sampler_ptr && sampler_ptr->has_next_sample(
                pixel_i, pixel_j
            )) {
                current_ray = sampler_ptr->get_ray(pixel_i, pixel_j);
            } else {
                sampler_ptr = nullptr;
            }
            return *this;
        }

        bool operator==(const ray_iterator& other) const {
            return sampler_ptr == other.sampler_ptr;
        }

        bool operator!=(const ray_iterator& other) const {
            return !(*this == other);
        }
};

class random_sampler : public sampler {
    private:
        point3 sample_pixel(int i, int j) const override {
            const auto offset = sample_square();
            return pixel00_loc
                + (i + offset.x()) * pixel_delta_u
                + (j + offset.y()) * pixel_delta_v;
        }

    public:
        random_sampler() {
            cfg.random.enabled = true;
        };
        random_sampler(const SamplerConfig& cfg) {
            this->cfg = cfg;
            this->cfg.random.enabled = true;
        }

        colour sampling_density_colour() const override {
            return colour(1, 1, 1);
        }

        bool has_next_sample(int i, int j) const override {
            return samples() < cfg.samples_per_pixel;
        }
};

class adaptive_random_sampler : public random_sampler {
    private:
        struct Data {
            point3 s1{0,0,0};  // Sum for each channel
            point3 s2{0,0,0};  // Sum of squares for each channel
        };

        Data data;

        static const colour red;
        static const colour blue;

        point3 mean() const {
            if (samples() == 0) {
                return point3(infinity_d, infinity_d, infinity_d);
            }
            return data.s1 / samples();
        }

        point3 variance() const {
            if (samples() <= 1) {
                return point3(infinity_d, infinity_d, infinity_d);
            }
            const auto s1_squared = data.s1 * data.s1;
            const auto n = static_cast<double>(samples());
            const auto factor = 1.0 / (n - 1);
            return factor * (data.s2 - (s1_squared / n));
        }

        bool should_continue() const {
            if (
                samples() < cfg.adaptive.burn_in
                || samples() % cfg.adaptive.check_every != 0
            ) return true;
            const auto mu = mean();
            const auto var = variance();

            // Check convergence for each channel
            for (int i = 0; i < 3; i++) {
                if (mu[i] < cfg.adaptive.epsilon) continue;
                const auto relative_error = std::sqrt(var[i] / samples())
                    * cfg.adaptive.critical_value / mu[i];
                if (relative_error >= cfg.adaptive.tolerance) return true;
            }
            return false;
        }

    public:
        adaptive_random_sampler() {
            cfg.random.enabled = true;
            cfg.adaptive.enabled = true;
        }
        adaptive_random_sampler(const SamplerConfig& cfg) {
            this->cfg = cfg;
            this->cfg.random.enabled = true;
            this->cfg.adaptive.enabled = true;
        }

        void add_sample(const colour& sample) override {
            data.s1 += sample;
            data.s2 += sample * sample;
        }

        void clear() override {
            sampler::clear();
            data = Data();
        }

        colour sampling_density_colour() const override {
            const auto density = static_cast<double>(samples())
                / cfg.samples_per_pixel;
            return red * density + blue * (1 - density);
        }

        // if we are under the sample limit, every N samples check if we
        // should stop sampling based on the mean and variance of sample
        // luminance values
        bool has_next_sample(int i, int j) const override {
            return random_sampler::has_next_sample(i, j)
                && should_continue();
        }
};

const colour adaptive_random_sampler::red(1, 0, 0);
const colour adaptive_random_sampler::blue(0, 0, 1);

inline ray_iterator sample_range::begin() const {
    return ray_iterator(s, i, j);
}

inline ray_iterator sample_range::end() const {
    return ray_iterator(nullptr, i, j);
}

#endif // SAMPLER_H
