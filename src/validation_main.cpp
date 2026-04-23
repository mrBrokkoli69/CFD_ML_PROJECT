#include "geometry/geometry.h"
#include "lbm/lbm_core.h"
#include "lbm/lbm_runner.h"
#include "post/postprocessing.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <chrono>

SimulationConfig createValidationConfig() {
	SimulationConfig config;

	config.nx = 441;
	config.ny = 84;

	config.tau = 0.6;
	config.rho0 = 1.0;
	config.uMax = 0.05;

	config.maxSteps = 5000;
	config.vtkInterval = 0;
	config.coutInterval = 1000;

	config.outputDir = "./data/case_runs/val_case";

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
	try {
		SimulationConfig config = createValidationConfig();

		int cx = 0;
		int cy = 0;
		int r = 0;
		Mask mask = createValidationMask(config, cx, cy, r);

		double characteristicLength = 2.0 * r;

		double re = computeReynoldsNumber(config, characteristicLength);

		std::cout << "Validation case started" << std::endl;
		std::cout << "Grid  = " << config.nx << " x " << config.ny << std::endl;
		std::cout << "tau   = " << config.tau << std::endl;
		std::cout << "rho0  = " << config.rho0 << std::endl;
		std::cout << "Umax  = " << config.uMax << std::endl;
		std::cout << "Re    = " << re << std::endl;

		auto startTime = std::chrono::steady_clock::now();

		SimulationResult result = runCase(config, mask);

		auto endTime = std::chrono::steady_clock::now();
		auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
		std::cout << "Solver elapsed time: " << elapsedMs.count() << " ms" << std::endl;



		printSimulationSummary(result, config, characteristicLength);

		double cd = computeDragCoefficient(result.avgFx, config, characteristicLength);

		const double cdRef = 5.55;
		const double tolerance = 0.30;

		std::cout << "Reference Cd = " << cdRef << std::endl;
		std::cout << "Tolerance    = +/- " << tolerance << std::endl;

		if (std::abs(cd - cdRef) > tolerance) {
			std::cerr << "VALIDATION FAILED: Cd = " << cd
				<< ", expected around " << cdRef << std::endl;
			return 1;
		}

		std::cout << "VALIDATION PASSED" << std::endl;
		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}
}
