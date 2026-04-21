#include "io/results_io.h"
#include "lbm/lbm_core.h"
#include "lbm/lbm_runner.h"
#include "mask_editor/mask_loader.h"
#include "post/postprocessing.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

SimulationConfig createDatRunConfig() {
    SimulationConfig config;
    config.outputDir = "./data/case_runs/mask_case";

    // Эти размеры должны совпадать с размерами маски в .dat
    config.nx = 300;
    config.ny = 80;

    config.tau = 0.6;
    config.rho0 = 1.0;
    config.uMax = 0.05;

    config.maxSteps = 3000;
    config.vtkInterval = 10;
    config.coutInterval = 500;

    return config;
}

int main() {
    try {
        SimulationConfig config = createDatRunConfig();

        // Путь к .dat маске, сохранённой редактором
        const std::string maskPath = "data/masks/mask_8.dat";

        Mask mask = loadMaskFromDat(maskPath);

        if (mask.nx != config.nx || mask.ny != config.ny) {
            throw std::runtime_error(
                "Loaded mask size does not match config: mask = " +
                std::to_string(mask.nx) + "x" + std::to_string(mask.ny) +
                ", config = " +
                std::to_string(config.nx) + "x" + std::to_string(config.ny)
            );
        }

        // Пока characteristicLength задаём вручную.
        // Если это цилиндр радиуса 10, то characteristicLength = 20.
        // Позже можно будет вычислять его из маски автоматически или задавать через UI.
        double characteristicLength = 20.0;

        double nu = CS2 * (config.tau - 0.5);
        double uMean = (2.0 / 3.0) * config.uMax;
        double re = uMean * characteristicLength / nu;

        std::cout << "Loaded mask from: " << maskPath << std::endl;
        std::cout << "Mask size: " << mask.nx << " x " << mask.ny << std::endl;
        std::cout << "Umax  = " << config.uMax << std::endl;
        std::cout << "Umean = " << uMean << std::endl;
        std::cout << "Re    = " << re << std::endl;

        SimulationResult result = runCase(config, mask, characteristicLength);

        printSimulationSummary(result, config, characteristicLength);

        appendResultToCSV("./data/results/simulation_results.csv",
                          "dat_mask_case",
                          config,
                          result,
                          characteristicLength);

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
