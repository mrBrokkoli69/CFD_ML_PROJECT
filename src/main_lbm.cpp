#include "lbm/lbm_core.h"
#include "geometry/geometry.h"
#include "lbm/lbm_runner.h"
#include "post/postprocessing.h"
#include <iostream>


SimulationConfig createValidationConfig() {
    SimulationConfig config;
    config.outputDir = "../data/case_runs/validation_re20";

    config.nx = 441;
    config.ny = 84;
    config.tau = 0.6;
    config.rho0 = 1.0;
    config.uMax = 0.05;

    config.maxSteps = 4000;
    config.vtkInterval = 0;
    config.coutInterval = 1000;

    return config;
}

Mask createValidationMask(const SimulationConfig& config, int& cx, int& cy, int& r) {
    cx = 40;
    cy = 41;
    r = 10;

    Mask mask = createEmptyMask(config.nx, config.ny);
    addCylinder(mask, cx, cy, r);

    return mask;
}



int main() {
    SimulationConfig config = createValidationConfig();

    int cx = 0;
    int cy = 0;
    int r = 0;
    Mask mask = createValidationMask(config, cx, cy, r);

    double characteristicLength = 2.0 * r;

    SimulationResult result = runCase(config, mask, characteristicLength);

    printSimulationSummary(result, config, characteristicLength);

    return 0;
}

