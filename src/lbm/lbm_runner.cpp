#include "lbm_runner.h"

#include <iostream>
#include <stdexcept>

SimulationResult runCase(const SimulationConfig& config,
		const Mask& mask,
		double characteristicLength) {
	if (mask.nx != config.nx || mask.ny != config.ny) {
		throw std::runtime_error("Mask size does not match simulation config");
	}

	double Fx = 0.0;
	double Fy = 0.0;
	double FsumX = 0.0;
	double FsumY = 0.0;
	double FmeanX = 0.0;
	double FmeanY = 0.0;

	LBMField field(config.nx, config.ny);
	initField(field, config.rho0, config.uMax, 0.0);
	loadMaskToLBM(field, mask.solid);

	std::cout << "Starting LBM simulation..." << std::endl;
	std::cout << "Grid: " << config.nx << " x " << config.ny << std::endl;
	std::cout << "tau = " << config.tau << ", uMax = " << config.uMax << std::endl;

	for (int step = 0; step < config.maxSteps; ++step) {
		Fx = 0.0;
		Fy = 0.0;

		computeMacroscopic(field, mask.solid);
		collision(field, config.tau, mask.solid);
		streaming(field, mask.solid, Fx, Fy);

		applyZouHeLeft(field, config.uMax);
		applyOutflowRight(field);
		resetCornersToRest(field);

		FsumX += Fx;
		FsumY += Fy;

		if (config.vtkInterval > 0 && step % config.vtkInterval == 0) {
			writeVTK(field, step, config.outputDir);
		}

		if (config.coutInterval > 0 && (step + 1) % config.coutInterval == 0) {
			FmeanX = FsumX / static_cast<double>(config.coutInterval);
			FmeanY = FsumY / static_cast<double>(config.coutInterval);

			std::cout << "Step " << step + 1
				<< " avg Fx = " << FmeanX
				<< ", avg Fy = " << FmeanY
				<< std::endl;

			FsumX = 0.0;
			FsumY = 0.0;
		}
	}

	if (config.vtkInterval > 0) {
		writeVTK(field, config.maxSteps, config.outputDir);
	}

	SimulationResult result;
	result.fx = Fx;
	result.fy = Fy;
	result.avgFx = FmeanX;
	result.avgFy = FmeanY;
	result.stepsDone = config.maxSteps;
	return result;
}
