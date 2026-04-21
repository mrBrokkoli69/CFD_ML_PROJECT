#pragma once

#include "../lbm/lbm_core.h"

double computeDragCoefficient(double fx, const SimulationConfig& config, double characteristicLength);
double computeLiftCoefficient(double fy, const SimulationConfig& config, double characteristicLength);
void printSimulationSummary(const SimulationResult& result,
                            const SimulationConfig& config,
                            double characteristicLength);

