#include "geometry/geometry.h"
#include "io/results_io.h"
#include "lbm/lbm_core.h"
#include "lbm/lbm_runner.h"
#include "post/postprocessing.h"

#include <cmath>
#include <iostream>
#include <vector>

SimulationConfig createValidationConfigRe20() {
    SimulationConfig config;
    config.outputDir = "./data/case_runs/validation_re20";

    config.nx = 441;
    config.ny = 84;
    config.tau = 0.6;
    config.rho0 = 1.0;
    config.uMax = 0.05;

    config.maxSteps = 3000;
    config.vtkInterval = 0;
    config.coutInterval = 1000;

    return config;
}

SimulationConfig createValidationConfigVariant() {
    SimulationConfig config;
    config.outputDir = "./data/case_runs/validation_variant";

    config.nx = 441;
    config.ny = 84;
    config.tau = 0.6;
    config.rho0 = 1.0;
    config.uMax = 0.04;

    config.maxSteps = 10000;
    config.vtkInterval = 0;
    config.coutInterval = 1000;

    return config;
}

Mask createCylinderValidationMask(const SimulationConfig& config, int& cx, int& cy, int& r) {
    cx = 40;
    cy = 41;
    r = 10;

    Mask mask = createEmptyMask(config.nx, config.ny);
    addCylinder(mask, cx, cy, r);

    return mask;
}

struct BatchCase {
    std::string caseName;
    SimulationConfig config;
};

int main() {
    std::vector<BatchCase> cases = {
        {"validation_re20", createValidationConfigRe20()},
        {"validation_variant", createValidationConfigVariant()}
    };

    const std::string csvPath = "./data/results/simulation_results.csv";

    for (const BatchCase& batchCase : cases) {
        int cx = 0;
        int cy = 0;
        int r = 0;

        Mask mask = createCylinderValidationMask(batchCase.config, cx, cy, r);

        double characteristicLength = 2.0 * r;
        double nu = CS2 * (batchCase.config.tau - 0.5);
        double uMean = (2.0 / 3.0) * batchCase.config.uMax;
        double re = uMean * characteristicLength / nu;

        std::cout << "\n==============================\n";
        std::cout << "Running case: " << batchCase.caseName << std::endl;
        std::cout << "Umax  = " << batchCase.config.uMax << std::endl;
        std::cout << "Umean = " << uMean << std::endl;
        std::cout << "Re    = " << re << std::endl;

        SimulationResult result = runCase(batchCase.config, mask, characteristicLength);

        printSimulationSummary(result, batchCase.config, characteristicLength);

        appendResultToCSV(csvPath,
                          batchCase.caseName,
                          batchCase.config,
                          result,
                          characteristicLength);

        if (batchCase.caseName == "validation_re20") {
            double cd = computeDragCoefficient(result.avgFx, batchCase.config, characteristicLength);
            double cdRef = 5.55;
            double tolerance = 0.15;

            if (std::abs(cd - cdRef) > tolerance) {
                std::cerr << "Validation FAILED for " << batchCase.caseName
                          << ": Cd = " << cd
                          << ", expected around " << cdRef
                          << " +/- " << tolerance << std::endl;
            }

            std::cout << "Validation PASSED for " << batchCase.caseName << std::endl;
        }
    }

    return 0;
}
