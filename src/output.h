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
    std::unique_ptr<std::ofstream> file_stream_ {};

    static std::unique_ptr<std::ofstream> open_file(
        const std::optional<std::string>& filename
    ) {
        if (!filename) {
            return nullptr;
        }

        auto file_stream { std::make_unique<std::ofstream>(
            filename.value() + ".ppm",
            std::ios_base::out | std::ios_base::trunc
        ) };

        if (!file_stream) {
            throw std::runtime_error("Failed to open output stream");
        }

        return file_stream;
    }

public:
    OutputHandler() = delete;
    explicit OutputHandler(const std::optional<std::string>& filename)
        : base_filename { filename }
        , file_stream_ { open_file(base_filename) }
    {}

    std::ostream& stream() {
        return file_stream_ ? *file_stream_ : std::cout;
    }

    void write_main_image(
        const Image& image,
        ImageFormat format
    ) {
        image.write(stream(), format);
    }

    void write_density_image(
        const std::optional<Image>& density_image
    ) {
        if (!density_image || !base_filename) {
            return;  // Do nothing if density tracking was disabled
        }

        std::ofstream density_output(base_filename.value() + ".density.ppm");
        if (!density_output) {
            std::cerr
                << "Error: Failed to open density output stream: "
                << base_filename.value()
                << std::endl;
            return;
        }

        density_image->write(density_output, ImageFormat::PPM);
    }
};

#endif
