#include "postprocessing.h"

#include <iostream>

double computeDragCoefficient(double fx, const SimulationConfig& config, double characteristicLength) {
    return 2.0 * fx / (config.rho0 *(4.0/9.0) *  config.uMax * config.uMax * characteristicLength);
}

double computeLiftCoefficient(double fy, const SimulationConfig& config, double characteristicLength) {
    return 2.0 * fy / (config.rho0 * (4.0/9.0) * config.uMax * config.uMax * characteristicLength);
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
