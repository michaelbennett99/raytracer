#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>
#include <fstream>
#include "image.h"

class OutputHandler {
private:
    std::optional<std::string> base_filename;
    std::unique_ptr<std::ofstream> file_stream_;

public:
    explicit OutputHandler(const std::optional<std::string>& filename)
        : base_filename(filename) {
        if (base_filename) {
            file_stream_ = std::make_unique<std::ofstream>(
                base_filename.value() + ".ppm",
                std::ios_base::out | std::ios_base::trunc
            );

            if (!file_stream_) {
                std::cerr
                    << "Error: Failed to open output stream: "
                    << base_filename.value()
                    << std::endl;
                throw std::runtime_error("Failed to open output stream");
            }
        }
    }

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
            throw std::runtime_error("Failed to open density output stream");
        }
        density_image->write(density_output, ImageFormat::PPM);
    }
};

#endif
