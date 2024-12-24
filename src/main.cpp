#include <iostream>

#include "color.h"

int main() {
    // Image
    const int width = 256;
    const int height = 256;
    const int max_colour_value = 255;

    // Render

    std::cout
        << "P3\n"
        << width << " " << height << "\n"
        << max_colour_value << "\n";

    for (int j = 0; j < height; j++) {
        std::clog
            << "\rScanlines remaining: " << (height - j) << " " << std::flush;
        for (int i = 0; i < width; i++) {
            auto pixel_colour = colour(
                double(i) / (width - 1), double(j) / (height - 1), 0.25
            );
            write_colour(std::cout, pixel_colour);
        }
    }
    std::clog << "\rDone.                    \n";
}
