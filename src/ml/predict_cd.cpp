#include "predict_cd.h"

#include "normalization_io.h"

#include <stdexcept>

double predictCd(
    InferenceSession& session,
    const std::string& shapeType,
    double tau,
    double uMax,
    double uMean,
    double characteristicLength,
    double reynolds,
    double anchorX,
    double anchorY,
    double radius,
    double rectWidth,
    double rectHeight,
    double ellipseRx,
    double ellipseRy,
    double chord,
    double solidFraction
) {
    if (!session.isLoaded) {
        throw std::runtime_error("Inference session is not loaded");
    }

    Vector rawFeatures = buildFeatureVectorForML(
        shapeType,
        tau,
        uMax,
        uMean,
        characteristicLength,
        reynolds,
        anchorX,
        anchorY,
        radius,
        rectWidth,
        rectHeight,
        ellipseRx,
        ellipseRy,
        chord,
        solidFraction
    );

    Vector normalizedFeatures = normalizeFeatureVector(rawFeatures, session.stats);
    Vector prediction = session.model.predict(normalizedFeatures);

    if (prediction.empty()) {
        throw std::runtime_error("Model returned empty prediction");
    }

    return prediction[0];
}
