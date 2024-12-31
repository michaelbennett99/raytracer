#ifndef RENDERER_H
#define RENDERER_H

#include "image.h"
#include "ray.h"
#include "world.h"
#include "sampler.h"
#include "pixel_renderer.h"

enum class RendererType {
    Colour,
    Density
};

// A renderer is a class that is directed by the camera to render an image
class Renderer {
protected:
    Image image_;
    const RendererType type_;

public:
    explicit Renderer(
        RendererType type = RendererType::Colour
    ) : type_(type) {}

    // Prepare the renderer for a new image
    void prepare(const ImageData& image_data) {
        image_ = Image(image_data.width, image_data.height);
    }

    std::unique_ptr<PixelRenderer> create_pixel_renderer(
        int i, int j, const PixelSampler& pixel_sampler
    ) {
        switch (type_) {
        case RendererType::Colour:
            return std::make_unique<ColourPixelRenderer>(
                pixel_sampler, image_, i, j
            );
        case RendererType::Density:
            return std::make_unique<DensityPixelRenderer>(
                pixel_sampler, image_, i, j
            );
        }
    }

    // Get the final image
    Image image() {
        return image_;
    };
};

#endif
