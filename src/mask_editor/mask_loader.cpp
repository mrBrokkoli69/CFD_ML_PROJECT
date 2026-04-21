#include "mask_loader.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

Mask loadMaskFromDat(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open mask file: " + filename);
    }

    int width = 0;
    int height = 0;
    in >> width >> height;

    if (!in.good() || width <= 0 || height <= 0) {
        throw std::runtime_error("Invalid mask header in file: " + filename);
    }

    Mask mask;
    mask.nx = width;
    mask.ny = height;
    mask.solid.assign(height, std::vector<bool>(width, false));

    std::string row;
    for (int y = 0; y < height; ++y) {
        in >> row;

        if (!in.good()) {
            throw std::runtime_error("Failed to read mask row " + std::to_string(y) +
                                     " from file: " + filename);
        }

        if (static_cast<int>(row.size()) != width) {
            throw std::runtime_error("Mask row length mismatch in file: " + filename +
                                     ", row " + std::to_string(y));
        }

        for (int x = 0; x < width; ++x) {
            char c = row[x];

            if (c == '1') {
                mask.solid[y][x] = true;
            } else if (c == '0') {
                mask.solid[y][x] = false;
            } else {
                throw std::runtime_error("Invalid mask character '" + std::string(1, c) +
                                         "' in file: " + filename);
            }
        }
    }

    return mask;
}
