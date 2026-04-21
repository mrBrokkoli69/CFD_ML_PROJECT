#pragma once

#include "lbm_core.h"

SimulationResult runCase(const SimulationConfig& config,
                         const Mask& mask,
                         double characteristicLength);
