#ifndef OUTPUT_H
#define OUTPUT_H

#include <optional>
#include <memory>
#include <string>
#include <fstream>

#include "image.h"

class OutputHandler {
private:
    std::optional<std::string> base_filename {};
    ImageFormat format {};
    std::unique_ptr<std::ofstream> file_stream_ {};

    static std::unique_ptr<std::ofstream> open_file(
        const std::optional<std::string>& filename,
        ImageFormat format
    ) {
        if (!filename) {
            return nullptr;
        }

        auto file_stream { std::make_unique<std::ofstream>(
            filename.value() + "." + to_extension(format),
            std::ios_base::out | std::ios_base::trunc
        ) };

        if (!file_stream) {
            throw std::runtime_error("Failed to open output stream");
        }

        return file_stream;
    }

public:
    OutputHandler() = delete;
    OutputHandler(
        const std::optional<std::string>& filename,
        ImageFormat format
    )
        : base_filename { filename }
        , format { format }
        , file_stream_ { open_file(base_filename, format) }
    {}

    std::ostream& stream() {
        return file_stream_ ? *file_stream_ : std::cout;
    }

    void write_main_image(
        const Image& image
    ) {
        image.write(stream(), format);
    }

    void write_density_image(
        const std::optional<Image>& density_image
    ) {
        if (!density_image || !base_filename) {
            return;  // Do nothing if density tracking was disabled
        }

        std::ofstream density_output(
            base_filename.value() + ".density." + to_extension(format)
        );
        if (!density_output) {
            std::cerr
                << "Error: Failed to open density output stream: "
                << base_filename.value()
                << std::endl;
            return;
        }

        density_image->write(density_output, format);
    }

    void write(
        const std::map<RendererType, Image>& images
    ) {
        for (const auto& [renderer, image] : images) {
            switch (renderer) {
                case RendererType::Colour:
                    write_main_image(image);
                    break;
                case RendererType::Density:
                    write_density_image(image);
                    break;
            }
        }
    }
};

#endif
