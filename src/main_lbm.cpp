#include "lbm/lbm_core.h"
#include <iostream>
#include <cmath>
void addCylinder(Mask& mask, int cx, int cy, int r) {
	int ny = mask.ny;
	int nx = mask.nx;
	for (int y = 0; y < ny; y++) {
		for (int x = 0; x < nx; x++) {
			int dx = x - cx;
			int dy = y - cy;
			if (dx*dx + dy*dy <= r*r) {
				mask.solid[y][x] = true;
			}
		}
	}
}
int main() {
	SimulationConfig config;

	config.outputDir = "./data/case_runs/test_case";

	config.nx = 441;
	config.ny = 84;
	config.tau = 0.6;
	config.uMax = 0.05;
	config.rho0 = 1.0;

	config.maxSteps = 6000;
	config.vtkInterval = 100;
	config.coutInterval = 1000;

	int cx = 40;
	int cy = 41;
	int r = 10;

	double nu = CS2 * (config.tau - 0.5);
	double L = 2 * r;
	double Re = (2.0/3.0) * config.uMax * L / nu;	
	std::cout << "Re = " << Re <<"\nUmax =  "<<config.uMax<<"\nL= "<<L<< std::endl;

	double Fx = 0;
	double Fy = 0;
	double FsumX = 0;
	double FsumY = 0;
	double FmeanX = 0 ;
	double FmeanY = 0;

	LBMField field(config.nx, config.ny);
	initField(field, config.rho0, config.uMax, 0.0);

	Mask mask;
	mask.nx = config.nx;
	mask.ny = config.ny;
	mask.solid.assign(config.ny, std::vector<bool>(config.nx, false));

	addCylinder(mask, cx, cy, r);


	// Применяем маску (обнуляем твёрдые ячейки)
	loadMaskToLBM(field, mask.solid);
	std::cout << "Starting LBM simulation..." << std::endl;
	std::cout << "Grid: " << config.nx << " x " << config.ny << std::endl;
	std::cout << "tau = " << config.tau << ", u_max = " << config.uMax << std::endl;

	for (int step = 0; step < config.maxSteps; step++) {
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

		if (step % config.vtkInterval == 0) {
			writeVTK(field, step, config.outputDir);
		}

		if ((step + 1) % config.coutInterval == 0) {
			FmeanX = FsumX / config.coutInterval;
			FmeanY = FsumY / config.coutInterval;

			std::cout << "Step " << step + 1
				<< " avg Fx = " << FmeanX
				<< ", avg Fy = " << FmeanY
				<< std::endl;

			FsumX = 0.0;
			FsumY = 0.0;
		}
	}
	// Сохраняем последний шаг
	writeVTK(field, config.maxSteps, config.outputDir);

	SimulationResult result;
	result.fx = FmeanX;
	result.fy = FmeanY;

	double rho_in = config.rho0;
	result.cd = 2.0 * result.fx / (rho_in * (4.0/9.0) * config.uMax * config.uMax * L);

	std::cout << "Final force: Fx = " << result.fx
		<< ", Fy = " << result.fy << std::endl;
	std::cout << "Cd = " << result.cd << std::endl;
	std::cout << "Simulation finished!" << std::endl;
	return 0;
}
