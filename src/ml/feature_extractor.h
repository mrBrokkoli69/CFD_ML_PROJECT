#ifndef FEATURE_EXTRACTOR_H
#define FEATURE_EXTRACTOR_H

#include "mlp.h"

#include <string>

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
);

#endif
