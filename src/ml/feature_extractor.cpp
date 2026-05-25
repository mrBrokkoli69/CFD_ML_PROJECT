#include "feature_extractor.h"

namespace {

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

} // namespace

Vector buildFeatureVectorForML(
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
    Vector features;
    features.reserve(19);

    const std::vector<double> shapeEncoding = encodeShapeType(shapeType);
    for (double value : shapeEncoding) {
        features.push_back(value);
    }

    features.push_back(tau);
    features.push_back(uMax);
    features.push_back(uMean);
    features.push_back(characteristicLength);
    features.push_back(reynolds);
    features.push_back(anchorX);
    features.push_back(anchorY);
    features.push_back(radius);
    features.push_back(rectWidth);
    features.push_back(rectHeight);
    features.push_back(ellipseRx);
    features.push_back(ellipseRy);
    features.push_back(chord);
    features.push_back(solidFraction);

    return features;
}
