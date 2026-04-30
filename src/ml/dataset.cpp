#include "dataset.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace {

std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, ',')) {
        tokens.push_back(item);
    }

    return tokens;
}

bool isFiniteNumber(double value) {
    return std::isfinite(value);
}

double parseDoubleSafe(const std::string& text) {
    try {
        size_t pos = 0;
        double value = std::stod(text, &pos);
        if (pos != text.size()) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return value;
    } catch (...) {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

int findColumnIndex(const std::vector<std::string>& header, const std::string& name) {
    for (int i = 0; i < static_cast<int>(header.size()); ++i) {
        if (header[i] == name) {
            return i;
        }
    }
    return -1;
}

double getNumericField(const std::vector<std::string>& row,
                       const std::vector<std::string>& header,
                       const std::string& columnName) {
    int index = findColumnIndex(header, columnName);
    if (index < 0 || index >= static_cast<int>(row.size())) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return parseDoubleSafe(row[index]);
}

std::string getStringField(const std::vector<std::string>& row,
                           const std::vector<std::string>& header,
                           const std::string& columnName) {
    int index = findColumnIndex(header, columnName);
    if (index < 0 || index >= static_cast<int>(row.size())) {
        return "";
    }
    return row[index];
}

std::vector<double> encodeShapeType(const std::string& shapeType) {
    std::vector<double> encoded(5, 0.0);

    if (shapeType == "circle") {
        encoded[0] = 1.0;
    } else if (shapeType == "rectangle") {
        encoded[1] = 1.0;
    } else if (shapeType == "ellipse") {
        encoded[2] = 1.0;
    } else if (shapeType == "naca0012") {
        encoded[3] = 1.0;
    } else if (shapeType == "naca2412") {
        encoded[4] = 1.0;
    }

    return encoded;
}

Sample makeSampleFromRow(const std::vector<std::string>& row,
                         const std::vector<std::string>& header) {
    Sample sample;

    const std::string shapeType = getStringField(row, header, "shape_type");
    const std::vector<double> shapeEncoding = encodeShapeType(shapeType);

    sample.features.reserve(15);
    sample.targets.reserve(2);

    for (double value : shapeEncoding) {
        sample.features.push_back(value);
    }

    sample.features.push_back(getNumericField(row, header, "tau"));
    sample.features.push_back(getNumericField(row, header, "uMax"));
    sample.features.push_back(getNumericField(row, header, "uMean"));
    sample.features.push_back(getNumericField(row, header, "characteristic_length"));
    sample.features.push_back(getNumericField(row, header, "reynolds"));
    sample.features.push_back(getNumericField(row, header, "anchor_x"));
    sample.features.push_back(getNumericField(row, header, "anchor_y"));
    sample.features.push_back(getNumericField(row, header, "radius"));
    sample.features.push_back(getNumericField(row, header, "rect_width"));
    sample.features.push_back(getNumericField(row, header, "rect_height"));
    sample.features.push_back(getNumericField(row, header, "ellipse_rx"));
    sample.features.push_back(getNumericField(row, header, "ellipse_ry"));
    sample.features.push_back(getNumericField(row, header, "chord"));
    sample.features.push_back(getNumericField(row, header, "solid_fraction"));

    sample.targets.push_back(getNumericField(row, header, "Cd"));
    sample.targets.push_back(getNumericField(row, header, "Cl"));

    return sample;
}

bool isValidSample(const Sample& sample) {
    for (double value : sample.features) {
        if (!isFiniteNumber(value)) {
            return false;
        }
    }

    for (double value : sample.targets) {
        if (!isFiniteNumber(value)) {
            return false;
        }
    }

    return true;
}

} // namespace

Dataset loadDatasetFromCsv(const std::string& csvPath) {
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open dataset CSV: " + csvPath);
    }

    std::string headerLine;
    if (!std::getline(file, headerLine)) {
        throw std::runtime_error("Dataset CSV is empty: " + csvPath);
    }

    const std::vector<std::string> header = splitCsvLine(headerLine);

    Dataset dataset;
    dataset.featureNames = {
        "is_circle",
        "is_rectangle",
        "is_ellipse",
        "is_naca0012",
        "is_naca2412",
        "tau",
        "uMax",
        "uMean",
        "characteristic_length",
        "reynolds",
        "anchor_x",
        "anchor_y",
        "radius",
        "rect_width",
        "rect_height",
        "ellipse_rx",
        "ellipse_ry",
        "chord",
        "solid_fraction"
    };

    dataset.targetNames = {
        "Cd",
        "Cl"
    };

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> row = splitCsvLine(line);
        Sample sample = makeSampleFromRow(row, header);
        dataset.samples.push_back(sample);
    }

    return dataset;
}

Dataset filterValidSamples(const Dataset& dataset) {
    Dataset filtered;
    filtered.featureNames = dataset.featureNames;
    filtered.targetNames = dataset.targetNames;

    filtered.samples.reserve(dataset.samples.size());

    for (const Sample& sample : dataset.samples) {
        if (isValidSample(sample)) {
            filtered.samples.push_back(sample);
        }
    }

    return filtered;
}

TrainTestSplit splitDataset(const Dataset& dataset, double trainRatio) {
    if (trainRatio <= 0.0 || trainRatio >= 1.0) {
        throw std::runtime_error("trainRatio must be in (0, 1)");
    }

    TrainTestSplit split;
    split.train.featureNames = dataset.featureNames;
    split.train.targetNames = dataset.targetNames;
    split.test.featureNames = dataset.featureNames;
    split.test.targetNames = dataset.targetNames;

    std::vector<int> indices(dataset.samples.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::mt19937 rng(123456);
    std::shuffle(indices.begin(), indices.end(), rng);

    const int trainSize = static_cast<int>(trainRatio * static_cast<double>(indices.size()));

    for (int i = 0; i < static_cast<int>(indices.size()); ++i) {
        const Sample& sample = dataset.samples[indices[i]];
        if (i < trainSize) {
            split.train.samples.push_back(sample);
        } else {
            split.test.samples.push_back(sample);
        }
    }

    return split;
}

NormalizationStats computeNormalizationStats(const Dataset& dataset) {
    if (dataset.samples.empty()) {
        throw std::runtime_error("Cannot compute normalization stats for empty dataset");
    }

    const int featureCount = static_cast<int>(dataset.samples[0].features.size());

    NormalizationStats stats;
    stats.featuresMeans.assign(featureCount, 0.0);
    stats.featureStds.assign(featureCount, 0.0);

    for (const Sample& sample : dataset.samples) {
        if (static_cast<int>(sample.features.size()) != featureCount) {
            throw std::runtime_error("Inconsistent feature size in dataset");
        }

        for (int j = 0; j < featureCount; ++j) {
            stats.featuresMeans[j] += sample.features[j];
        }
    }

    const double sampleCount = static_cast<double>(dataset.samples.size());
    for (int j = 0; j < featureCount; ++j) {
        stats.featuresMeans[j] /= sampleCount;
    }

    for (const Sample& sample : dataset.samples) {
        for (int j = 0; j < featureCount; ++j) {
            const double diff = sample.features[j] - stats.featuresMeans[j];
            stats.featureStds[j] += diff * diff;
        }
    }

    for (int j = 0; j < featureCount; ++j) {
        stats.featureStds[j] = std::sqrt(stats.featureStds[j] / sampleCount);

        if (stats.featureStds[j] < 1e-12) {
            stats.featureStds[j] = 1.0;
        }
    }

    return stats;
}

void applyNormalization(Dataset& dataset, const NormalizationStats& stats) {
    if (dataset.samples.empty()) {
        return;
    }

    const int featureCount = static_cast<int>(dataset.samples[0].features.size());

    if (static_cast<int>(stats.featuresMeans.size()) != featureCount ||
        static_cast<int>(stats.featureStds.size()) != featureCount) {
        throw std::runtime_error("Normalization stats size does not match dataset feature size");
    }

    for (Sample& sample : dataset.samples) {
        if (static_cast<int>(sample.features.size()) != featureCount) {
            throw std::runtime_error("Inconsistent feature size in dataset");
        }

        for (int j = 0; j < featureCount; ++j) {
            sample.features[j] =
                (sample.features[j] - stats.featuresMeans[j]) / stats.featureStds[j];
        }
    }
}
