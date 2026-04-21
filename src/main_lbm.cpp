#include "lbm/lbm_core.h"
#include "geometry/geometry.h"
#include "lbm/lbm_runner.h"

#include <iostream>


int main() {
	SimulationConfig config;
	config.outputDir = "./data/case_runs/test_case";

	config.nx = 200;
	config.ny = 100;
	config.tau = 0.55;
	config.rho0 = 1.0;
	config.uMax = 0.08;

	config.maxSteps = 3000;
	config.vtkInterval = 10;
	config.coutInterval = 100;

	int cx = 56;
	int cy = config.ny / 2;
	int r = 6;

	double characteristicLength = 2.0 * r;
	double nu = CS2 * (config.tau - 0.5);
	double Re = config.uMax * characteristicLength * (2.0/3.0) / nu;

	Mask mask = createEmptyMask(config.nx, config.ny);
	addCylinder(mask,cx,cy,r);

	std::cout << "Re = " << Re << std::endl;

	SimulationResult result = runCase(config, mask, characteristicLength);

	std::cout << "Final force: Fx = " << result.fx
		<< ", Fy = " << result.fy << std::endl;
	std::cout << "Cd = " << result.cd << std::endl;
	std::cout << "Simulation finished!" << std::endl;

	return 0;

}
