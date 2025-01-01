#ifndef PIXEL_RENDERER_H
#define PIXEL_RENDERER_H

#include "colour.h"
#include "image.h"
#include "pixel_sampler.h"
#include "ray.h"

class PixelRenderer {
protected:
    const PixelSampler& pixel_sampler_;
    Image& image_;
    const int i_, j_;
public:
    PixelRenderer(const PixelSampler& pixel_sampler, Image& image, int i, int j)
        : pixel_sampler_(pixel_sampler), image_(image), i_(i), j_(j) {}

    virtual void process_sample(const ray& r, const Colour& pixel_colour) = 0;
    virtual ~PixelRenderer() = default;
};

class ColourPixelRenderer : public PixelRenderer {
private:
    Colour current_colour_;

public:
    ColourPixelRenderer(
        const PixelSampler& pixel_sampler, Image& image, int i, int j
    ) : PixelRenderer(pixel_sampler, image, i, j),
        current_colour_(0, 0, 0) {}

    void process_sample(const ray& r, const Colour& pixel_colour) override {
        current_colour_ += pixel_colour;
    }

    ~ColourPixelRenderer() {
        image_[j_][i_] = current_colour_ / pixel_sampler_.samples();
    }
};

class DensityPixelRenderer : public PixelRenderer {
private:
    static Colour colour_pixel(double sampling_density) {
        return Colour(sampling_density, 0, 1 - sampling_density);
    }
public:
    DensityPixelRenderer(
        const PixelSampler& pixel_sampler, Image& image, int i, int j
    ) : PixelRenderer(pixel_sampler, image, i, j) {}

    void process_sample(const ray& r, const Colour& pixel_colour) override {}

    ~DensityPixelRenderer() {
        image_[j_][i_] = colour_pixel(pixel_sampler_.sampling_density());
    }
};

#endif
