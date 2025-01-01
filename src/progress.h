#ifndef PROGRESS_H
#define PROGRESS_H

#include <iostream>
#include <mutex>
#include <sstream>
class Progress {
private:
    const int total;
    mutable int current;
    mutable std::mutex mutex;
    const int width = 50;

public:
    explicit Progress(int height) : total(height), current(0) {}

    void update() const {
        std::lock_guard<std::mutex> lock(mutex);
        current++;
    }

    void print() const {
        const double progress = static_cast<double>(current) / total;
        const int progress_width = static_cast<int>(progress * width);
        std::stringstream ss;
        ss << "\rProgress: [";
        for (int i = 0; i < progress_width; ++i) {
            ss << "=";
        }
        ss << ">";
        for (int i = progress_width + 1; i < width; ++i) {
            ss << "-";
        }
        ss << "] " << current << "/" << total << " Scanlines" << std::flush;
        std::lock_guard<std::mutex> lock(mutex);
        std::clog << ss.str();
    }

    void done() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::clog << "\n";
    }

    void reset() const {
        std::lock_guard<std::mutex> lock(mutex);
        current = 0;
    }
};

#endif
