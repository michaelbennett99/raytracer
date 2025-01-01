#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vector>
#include <iterator>

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
    Renderer(
        const ImageData& image_data,
        RendererType type = RendererType::Colour
    ) : image_(image_data.width, image_data.height), type_(type) {}

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
        default:
            throw std::runtime_error("Invalid renderer type");
        }
    }

    RendererType type() const {
        return type_;
    }

    // Get the final image
    Image image() const {
        return image_;
    };
};

class Renderers : public std::vector<Renderer> {
public:
    Renderers(
        const ImageData& image_data,
        std::vector<RendererType> renderer_types
    ) {
        for (const auto& type : renderer_types) {
            emplace_back(Renderer(image_data, type));
        }
    }

    std::vector<std::unique_ptr<PixelRenderer>> create_pixel_renderers(
        int i, int j, const PixelSampler& pixel_sampler
    ) {
        std::vector<std::unique_ptr<PixelRenderer>> pixel_renderer_ptrs;
        for (auto& renderer : *this) {
            pixel_renderer_ptrs.emplace_back(
                renderer.create_pixel_renderer(i, j, pixel_sampler)
            );
        }
        return pixel_renderer_ptrs;
    }
};

#endif
