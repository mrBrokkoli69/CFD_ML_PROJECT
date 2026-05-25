#include "normalization_io.h"

#include <fstream>
#include <stdexcept>

void saveNormalizationStats(const NormalizationStats& stats, const std::string& filePath) {
    std::ofstream out(filePath);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open normalization file for writing: " + filePath);
    }

    out << stats.featuresMeans.size() << '\n';
    for (double value : stats.featuresMeans) {
        out << value << ' ';
    }
    out << '\n';

    out << stats.featureStds.size() << '\n';
    for (double value : stats.featureStds) {
        out << value << ' ';
    }
    out << '\n';
}

NormalizationStats loadNormalizationStats(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open normalization file for reading: " + filePath);
    }

    NormalizationStats stats;

    int meanSize = 0;
    in >> meanSize;
    if (!in || meanSize < 0) {
        throw std::runtime_error("Failed to read normalization means size");
    }

    stats.featuresMeans.resize(meanSize, 0.0);
    for (int i = 0; i < meanSize; ++i) {
        in >> stats.featuresMeans[i];
        if (!in) {
            throw std::runtime_error("Failed to read normalization means");
        }
    }

    int stdSize = 0;
    in >> stdSize;
    if (!in || stdSize < 0) {
        throw std::runtime_error("Failed to read normalization stds size");
    }

    stats.featureStds.resize(stdSize, 0.0);
    for (int i = 0; i < stdSize; ++i) {
        in >> stats.featureStds[i];
        if (!in) {
            throw std::runtime_error("Failed to read normalization stds");
        }
    }

    if (stats.featuresMeans.size() != stats.featureStds.size()) {
        throw std::runtime_error("Normalization means/stds size mismatch");
    }

    return stats;
}

Vector normalizeFeatureVector(const Vector& rawFeatures, const NormalizationStats& stats) {
    if (rawFeatures.size() != stats.featuresMeans.size() ||
        rawFeatures.size() != stats.featureStds.size()) {
        throw std::runtime_error("Feature vector size does not match normalization stats");
    }

    Vector normalized(rawFeatures.size(), 0.0);

    for (int i = 0; i < static_cast<int>(rawFeatures.size()); ++i) {
        normalized[i] =
            (rawFeatures[i] - stats.featuresMeans[i]) / stats.featureStds[i];
    }

    return normalized;
}
