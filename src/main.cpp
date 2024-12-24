#include <iostream>

int main() {
    // Image
    const int width = 256;
    const int height = 256;
    const int max_colour_value = 255;
    const double colour_multiplier = max_colour_value + 0.999;

    // Render

    std::cout
        << "P3\n"
        << width << " " << height << "\n"
        << max_colour_value << "\n";

    for (int j = 0; j < height; j++) {
        std::clog
            << "\rScanlines remaining: " << (height - j) << " " << std::flush;
        for (int i = 0; i < width; i++) {
            auto r = double(i) / (width - 1);
            auto g = double(j) / (height - 1);
            auto b = 0.25;

            int ir = static_cast<int>(colour_multiplier * r);
            int ig = static_cast<int>(colour_multiplier * g);
            int ib = static_cast<int>(colour_multiplier * b);

            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
    std::clog << "\rDone.                    \n";
}
