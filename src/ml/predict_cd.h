#ifndef PREDICT_CD_H
#define PREDICT_CD_H

#include "feature_extractor.h"
#include "inference_session.h"

#include <string>

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
);

#endif
