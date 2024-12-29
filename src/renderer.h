#ifndef RENDERER_H
#define RENDERER_H

#include "image.h"
#include "ray.h"
#include "world.h"

// A renderer is a class that is directed by the camera to render the image
class Renderer {
public:
    // Prepare the renderer for a new image
    virtual void prepare(const ImageData& image_data) = 0;

    // Start rendering a new pixel
    virtual void start_pixel(int i, int j) = 0;

    // Process a sample for a pixel
    virtual void process_sample(
        int i, int j, const ray& r, const colour& pixel_colour
    ) = 0;

    // Finish rendering a pixel
    virtual void finish_pixel(int i, int j) = 0;

    // Get the final image
    virtual Image get_result() const = 0;

    // Destructor
    virtual ~Renderer() = default;
};

// Basic renderer that just produces the final image
class MainRenderer : public Renderer {
private:
    Image image;
    colour current_pixel;
    int samples;

public:
    void prepare(const ImageData& image_data) override {
        image = Image(image_data.width, image_data.height);
    }

    void start_pixel(int i, int j) override {
        current_pixel = colour(0,0,0);
        samples = 0;
    }

    void process_sample(
        int i, int j, const ray& r, const colour& pixel_colour
    ) override {
        current_pixel += pixel_colour;
        samples++;
    }

    void finish_pixel(int i, int j) override {
        image[j][i] = current_pixel / samples;
        current_pixel = colour(0,0,0);
    }

    Image get_result() const override {
        return image;
    }
};

// Renderer that tracks sampling density
class DensityRenderer : public Renderer {
private:
    Image density_map;
    std::shared_ptr<sampler> sampler_;

public:
    explicit DensityRenderer(std::shared_ptr<sampler> s) : sampler_(s) {}

    void prepare(const ImageData& image_data) override {
        density_map = Image(image_data.width, image_data.height);
    }

    void start_pixel(int i, int j) override {}

    void process_sample(
        int i, int j, const ray& r, const colour& pixel_colour
    ) override {}

    void finish_pixel(int i, int j) override {
        density_map[j][i] = sampler_->sampling_density_colour();
    }

    Image get_result() const override {
        return density_map;
    }
};

#endif
