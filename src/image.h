#ifndef IMAGE_H
#define IMAGE_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"

#include <vector>
#include <ostream>

#include "colour.h"

enum class ImageFormat {
    PPM,
    PNG
};

inline std::string to_string(ImageFormat format) {
    switch (format) {
        case ImageFormat::PPM: return "PPM";
        case ImageFormat::PNG: return "PNG";
    }
}

inline std::ostream& operator<<(std::ostream& os, ImageFormat format) {
    return os << to_string(format);
}

inline std::string to_extension(ImageFormat format) {
    switch (format) {
        case ImageFormat::PPM: return "ppm";
        case ImageFormat::PNG: return "png";
    }
}

struct ImageData {
    int width {};
    int height {};

    double aspect_ratio() const {
        return static_cast<double>(width) / height;
    }
};

class Image {
private:
    ImageData data_ {};
    int max_colour_value_ { default_max_colour_value };
    std::vector<std::vector<Colour>> image_ {};

    static constexpr int default_max_colour_value { 255 };

    void write_ppm(std::ostream& output) const {
        output << "P3\n"
            << data_.width << " " << data_.height << "\n"
            << max_colour_value_ << "\n";

        for (const auto& row : image_) {
            for (const auto& pixel : row) {
                const auto gamma_corrected = pixel.gamma_correct();
                output
                    << static_cast<int>(gamma_corrected.x()) << ' '
                    << static_cast<int>(gamma_corrected.y()) << ' '
                    << static_cast<int>(gamma_corrected.z()) << '\n';
            }
        }
    }

    void write_png(std::ostream& output) const {
        std::vector<unsigned char> bytes {};
        bytes.reserve(data_.width * data_.height * 3);

        for (const auto& row : image_) {
            for (const auto& pixel : row) {
                const auto gamma_corrected { pixel.gamma_correct() };
                bytes.push_back(static_cast<unsigned char>(
                    gamma_corrected.x()
                ));
                bytes.push_back(static_cast<unsigned char>(
                    gamma_corrected.y()
                ));
                bytes.push_back(static_cast<unsigned char>(
                    gamma_corrected.z()
                ));
            }
        }

        stbi_write_png_to_func(
            [](void* context, void* data, int size) {
                std::ostream& output { *static_cast<std::ostream*>(context) };
                output.write(static_cast<char*>(data), size);
            },
            &output,
            data_.width,
            data_.height,
            3,
            bytes.data(),
            data_.width * 3
        );
    }

public:
    Image() = default;

    Image(
        int width,
        double aspect_ratio,
        int max_colour_value = default_max_colour_value
    ) : data_{width, calc_image_height(width, aspect_ratio)},
        max_colour_value_(max_colour_value),
        image_(data_.height, std::vector<Colour>(data_.width)) {}

    Image(
        int width,
        int height,
        int max_colour_value = default_max_colour_value
    ) : data_{width, height},
        max_colour_value_(max_colour_value),
        image_(data_.height, std::vector<Colour>(data_.width)) {}

    int width() const {
        return data_.width;
    }

    int height() const {
        return data_.height;
    }

    double aspect_ratio() const {
        return data_.aspect_ratio();
    }

    Image& resize(int width, int height) {
        data_.width = width;
        data_.height = height;
        image_.resize(data_.height, std::vector<Colour>(data_.width));
        return *this;
    }

    std::vector<Colour>& operator[](int row) {
        return image_[row];
    }

    void set_pixel(int row, int col, const Colour& c) {
        image_[row][col] = c;
    }

    void set_pixels(int row, const std::vector<Colour>& pixels) {
        image_[row] = pixels;
    }

    void write(std::ostream& output, ImageFormat format) const {
        switch (format) {
            case ImageFormat::PPM: write_ppm(output); break;
            case ImageFormat::PNG: write_png(output); break;
        }
    }

    static constexpr int calc_image_height(int width, double aspect_ratio) {
        const auto h { static_cast<int>(width / aspect_ratio) };
        return (h < 1) ? 1 : h;
    }
};

#endif
