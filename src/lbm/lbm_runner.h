#pragma once

#include "../lbm/lbm_core.h"

struct CaseDefinition {
    std::string caseName;
    SimulationConfig config;
    Mask mask;
    double characteristicLength;
};





SimulationResult runCase(const SimulationConfig& config,
                         const Mask& mask,
                         double characteristicLength);
