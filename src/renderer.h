#ifndef RENDERER_H
#define RENDERER_H

#include "image.h"
#include "ray.h"
#include "world.h"
#include "sampler.h"

// A renderer is a class that is directed by the camera to render the image
class Renderer {
protected:
    std::shared_ptr<Sampler> sampler_;

public:
    Renderer() = default;
    Renderer(std::shared_ptr<Sampler> sampler) : sampler_(sampler) {}

    // Prepare the renderer for a new image
    virtual void prepare(const ImageData& image_data) = 0;

    // Start rendering a new pixel
    virtual void start_pixel(int i, int j) = 0;

    // Process a sample for a pixel
    virtual void process_sample(
        int i, int j, const ray& r, const colour& pixel_colour
    ) = 0;

    // Finish rendering a pixel
    virtual void finish_pixel(
        int i, int j, const PixelSampler& pixel_sampler
    ) = 0;

    // Get the final image
    virtual Image get_result() const = 0;

    // Destructor
    virtual ~Renderer() = default;
};

// Basic renderer that just produces the final image
// Still not thread-safe - the current_pixel variable can be accessed by
// multiple threads at once
class MainRenderer : public Renderer {
private:
    Image image;
    colour current_pixel;

public:
    MainRenderer() = default;
    MainRenderer(std::shared_ptr<Sampler> sampler) : Renderer(sampler) {}

    void prepare(const ImageData& image_data) override {
        image = Image(image_data.width, image_data.height);
    }

    void start_pixel(int i, int j) override {
        current_pixel = colour(0,0,0);
    }

    void process_sample(
        int i, int j,
        const ray& r,
        const colour& pixel_colour
    ) override {
        current_pixel += pixel_colour;
    }

    void finish_pixel(
        int i, int j, const PixelSampler& pixel_sampler
    ) override {
        image[j][i] = current_pixel / pixel_sampler.samples();
    }

    Image get_result() const override {
        return image;
    }
};

// Renderer that tracks sampling density
class DensityRenderer : public Renderer {
private:
    Image density_map;

    static colour colour_pixel(double sampling_density) {
        return colour(1, 0, 0) * sampling_density
            + colour(0, 0, 1) * (1 - sampling_density);
    }

public:
    DensityRenderer() = default;
    DensityRenderer(std::shared_ptr<Sampler> sampler) : Renderer(sampler) {}

    void prepare(const ImageData& image_data) override {
        density_map = Image(image_data.width, image_data.height);
    }

    void start_pixel(int i, int j) override {}

    void process_sample(
        int i, int j,
        const ray& r,
        const colour& pixel_colour
    ) override {}

    void finish_pixel(
        int i, int j, const PixelSampler& pixel_sampler
    ) override {
        const double n_samples = pixel_sampler.samples();
        const double sampling_density = n_samples
            / sampler_->get_config().samples_per_pixel;
        density_map[j][i] = colour_pixel(sampling_density);
    }

    Image get_result() const override {
        return density_map;
    }
};

#endif
