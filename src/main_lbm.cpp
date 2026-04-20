#include "lbm/lbm_core.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

void addCylinder(Mask& mask, int cx, int cy, int r) {
    for (int y = 0; y < mask.ny; ++y) {
        for (int x = 0; x < mask.nx; ++x) {
            int dx = x - cx;
            int dy = y - cy;

            if (dx * dx + dy * dy <= r * r) {
                mask.solid[y][x] = true;
            }
        }
    }
}

SimulationResult runCase(const SimulationConfig& config, const Mask& mask, double characteristicLength) {
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

    writeVTK(field, config.maxSteps, config.outputDir);

    SimulationResult result;
    result.fx = FmeanX;
    result.fy = FmeanY;
    result.cd = 2.0 * result.fx
              / (config.rho0 * config.uMax * (4.0/9.0) * config.uMax * characteristicLength);

    return result;
}

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

    Mask mask;
    mask.nx = config.nx;
    mask.ny = config.ny;
    mask.solid.assign(config.ny, std::vector<bool>(config.nx, false));

    addCylinder(mask, cx, cy, r);

    std::cout << "Re = " << Re << std::endl;

    SimulationResult result = runCase(config, mask, characteristicLength);

    std::cout << "Final force: Fx = " << result.fx
              << ", Fy = " << result.fy << std::endl;
    std::cout << "Cd = " << result.cd << std::endl;
    std::cout << "Simulation finished!" << std::endl;

    return 0;
}
