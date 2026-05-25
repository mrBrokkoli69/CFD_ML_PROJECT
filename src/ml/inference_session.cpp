#include "inference_session.h"

InferenceSession loadInferenceSession(
    const std::string& modelPath,
    const std::string& normPath
) {
    InferenceSession session;

    session.model = loadModel(modelPath);
    session.stats = loadNormalizationStats(normPath);
    session.isLoaded = true;

    return session;
}
