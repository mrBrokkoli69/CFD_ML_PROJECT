#ifndef INFERENCE_SESSION_H
#define INFERENCE_SESSION_H

#include "mlp.h"
#include "normalization_io.h"

#include <string>

struct InferenceSession {
    bool isLoaded = false;
    MLP model = MLP(1, 1, 1, 1);
    NormalizationStats stats;
};

InferenceSession loadInferenceSession(
    const std::string& modelPath,
    const std::string& normPath
);

#endif
