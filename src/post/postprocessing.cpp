#include "postprocessing.h"

#include <iostream>

double computeMeanInletVelocity(const SimulationConfig& config) {
    return (2.0 / 3.0) * config.uMax;
}

double computeReynoldsNumber(const SimulationConfig& config, double characteristicLength) {
    double nu = CS2 * (config.tau - 0.5);
    double uMean = computeMeanInletVelocity(config);
    return uMean * characteristicLength / nu;
}

double computeDragCoefficient(double fx,
                              const SimulationConfig& config,
                              double characteristicLength) {
    double uMean = computeMeanInletVelocity(config);
    return 2.0 * fx / (config.rho0 * uMean * uMean * characteristicLength);
}

double computeLiftCoefficient(double fy,
                              const SimulationConfig& config,
                              double characteristicLength) {
    double uMean = computeMeanInletVelocity(config);
    return 2.0 * fy / (config.rho0 * uMean * uMean * characteristicLength);
}

void printSimulationSummary(const SimulationResult& result,
                            const SimulationConfig& config,
                            double characteristicLength) {
    double cd = computeDragCoefficient(result.avgFx, config, characteristicLength);
    double cl = computeLiftCoefficient(result.avgFy, config, characteristicLength);

    std::cout << "Final force: Fx = " << result.avgFx
              << ", Fy = " << result.avgFy << std::endl;
    std::cout << "Cd = " << cd << std::endl;
    std::cout << "Cl = " << cl << std::endl;
    std::cout << "Steps done = " << result.stepsDone << std::endl;
    std::cout << "Simulation finished!" << std::endl;
}
