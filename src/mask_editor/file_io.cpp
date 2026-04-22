#include "file_io.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

std::string getNextMaskFilename() {
    fs::create_directories("data/masks");

    int maxIndex = 0;
    for (const auto& entry : fs::directory_iterator("data/masks")) {
        std::string name = entry.path().filename().string();

        if (name.rfind("mask_", 0) == 0 && name.find(".dat") != std::string::npos) {
            try {
                int index = std::stoi(name.substr(5, name.find(".dat") - 5));
                maxIndex = std::max(maxIndex, index);
            } catch (...) {
            }
        }
    }

    return "mask_" + std::to_string(maxIndex + 1) + ".dat";
}

std::string buildMaskPath(const std::string& filename) {
    return "data/masks/" + filename;
}

static std::string normalizeMaskFilename(std::string name) {
    if (name.empty()) {
        return name;
    }

    for (char& c : name) {
        if (c == ' ') {
            c = '_';
        }
    }

    if (name.size() < 4 || name.substr(name.size() - 4) != ".dat") {
        name += ".dat";
    }

    return name;
}

static std::string writeMaskToFile(const EditorMask& mask, const std::string& filename) {
    fs::create_directories("data/masks");

    std::string normalized = normalizeMaskFilename(filename);
    std::string fullPath = buildMaskPath(normalized);

    std::ofstream out(fullPath);
    if (!out) {
        return "";
    }

    out << EditorMask::WIDTH << " " << EditorMask::HEIGHT << "\n";
    for (int y = 0; y < EditorMask::HEIGHT; ++y) {
        for (int x = 0; x < EditorMask::WIDTH; ++x) {
            out << (mask.cells[y][x] ? '1' : '0');
        }
        out << "\n";
    }

    return fullPath;
}

std::string saveMask(const EditorMask& mask) {
    return writeMaskToFile(mask, getNextMaskFilename());
}

std::string saveMaskAs(const EditorMask& mask, const std::string& baseName) {
    return writeMaskToFile(mask, baseName);
}

bool loadMask(EditorMask& mask, const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        return false;
    }

    int width = 0;
    int height = 0;
    in >> width >> height;

    if (width != EditorMask::WIDTH || height != EditorMask::HEIGHT) {
        return false;
    }

    for (int y = 0; y < EditorMask::HEIGHT; ++y) {
        std::string row;
        in >> row;

        if (static_cast<int>(row.size()) != EditorMask::WIDTH) {
            return false;
        }

        for (int x = 0; x < EditorMask::WIDTH; ++x) {
            mask.cells[y][x] = (row[x] == '1');
        }
    }

    return true;
}

std::vector<std::string> getMaskList() {
    std::vector<std::string> masks;

    if (!fs::exists("data/masks")) {
        return masks;
    }

    for (const auto& entry : fs::directory_iterator("data/masks")) {
        if (entry.path().extension() == ".dat") {
            masks.push_back(entry.path().filename().string());
        }
    }

    std::sort(masks.begin(), masks.end());
    return masks;
}

bool deleteMaskFile(const std::string& filename) {
    std::string fullPath = buildMaskPath(filename);

    if (!fs::exists(fullPath)) {
        return false;
    }

    return fs::remove(fullPath);
}
