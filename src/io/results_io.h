#pragma once

#include "../lbm/lbm_core.h"

#include <string>

void appendResultToCSV(const std::string& csvPath,
                       const std::string& caseName,
                       const SimulationConfig& config,
                       const SimulationResult& result,
                       double characteristicLength);
