#ifndef NORMALIZATION_IO_H
#define NORMALIZATION_IO_H

#include "dataset.h"
#include "mlp.h"

#include <string>

void saveNormalizationStats(const NormalizationStats& stats, const std::string& filePath);
NormalizationStats loadNormalizationStats(const std::string& filePath);

Vector normalizeFeatureVector(const Vector& rawFeatures, const NormalizationStats& stats);

#endif
