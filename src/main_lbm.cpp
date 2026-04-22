#include "io/case_io.h"
#include "io/results_io.h"
#include "lbm/lbm_core.h"
#include "lbm/lbm_runner.h"
#include "mask_editor/mask_loader.h"
#include "post/postprocessing.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <streambuf>
#include <string>

class TeeBuf : public std::streambuf {
	public:
		TeeBuf(std::streambuf* first, std::streambuf* second)
			: first_(first), second_(second) {}

	protected:
		int overflow(int c) override {
			if (c == EOF) {
				return !EOF;
			}

			const int r1 = first_ ? first_->sputc(static_cast<char>(c)) : c;
			const int r2 = second_ ? second_->sputc(static_cast<char>(c)) : c;

			return (r1 == EOF || r2 == EOF) ? EOF : c;
		}

		int sync() override {
			int s1 = first_ ? first_->pubsync() : 0;
			int s2 = second_ ? second_->pubsync() : 0;
			return (s1 == 0 && s2 == 0) ? 0 : -1;
		}

	private:
		std::streambuf* first_;
		std::streambuf* second_;
};

SimulationConfig createDatRunConfig() {
	SimulationConfig config;

	// Фиксированный размер mask editor-а
	config.nx = 300;
	config.ny = 80;

	config.tau = 0.6;
	config.rho0 = 1.0;
	config.uMax = 0.05;

	config.maxSteps = 5000;
	config.vtkInterval = 0;
	config.coutInterval = 1000;

	return config;
}

int main(int argc, char* argv[]) {
	try {
		SimulationConfig config = createDatRunConfig();

		std::string maskPath = "./mask.dat";
		if (argc >= 2) {
			maskPath = argv[1];
		}

		if (argc >= 3) config.tau = std::atof(argv[2]);
		if (argc >= 4) config.uMax = std::atof(argv[3]);
		if (argc >= 5) config.rho0 = std::atof(argv[4]);
		if (argc >= 6) config.maxSteps = std::atoi(argv[5]);
		if (argc >= 7) config.coutInterval = std::atoi(argv[6]);
		if (argc >= 8) config.vtkInterval = std::atoi(argv[7]);

		double characteristicLength = 20.0;
		if (argc >= 9) {
			characteristicLength = std::atof(argv[8]);
		}

		CasePaths casePaths = createCasePaths(maskPath);
		config.outputDir = casePaths.caseDir;

		std::ofstream logFile(casePaths.logPath);
		if (!logFile.is_open()) {
			throw std::runtime_error("Cannot open log file: " + casePaths.logPath);
		}

		TeeBuf teeBuf(std::cout.rdbuf(), logFile.rdbuf());
		std::streambuf* oldCoutBuf = std::cout.rdbuf(&teeBuf);

		Mask mask = loadMaskFromDat(maskPath);

		if (mask.nx != config.nx || mask.ny != config.ny) {
			std::cout.rdbuf(oldCoutBuf);
			throw std::runtime_error(
					"Loaded mask size does not match config: mask = " +
					std::to_string(mask.nx) + "x" + std::to_string(mask.ny) +
					", config = " +
					std::to_string(config.nx) + "x" + std::to_string(config.ny)
					);
		}

		double nu = CS2 * (config.tau - 0.5);
		double uMean = (2.0 / 3.0) * config.uMax;
		double re = uMean * characteristicLength / nu;

		std::cout << "Case name: " << casePaths.caseName << std::endl;
		std::cout << "Case dir : " << casePaths.caseDir << std::endl;
		std::cout << "Mask path: " << maskPath << std::endl;
		std::cout << "Mask size: " << mask.nx << " x " << mask.ny << std::endl;
		std::cout << "tau   = " << config.tau << std::endl;
		std::cout << "rho0  = " << config.rho0 << std::endl;
		std::cout << "Umax  = " << config.uMax << std::endl;
		std::cout << "Umean = " << uMean << std::endl;
		std::cout << "Re    = " << re << std::endl;
		std::cout << "maxSteps        = " << config.maxSteps << std::endl;
		std::cout << "coutInterval    = " << config.coutInterval << std::endl;
		std::cout << "vtkInterval     = " << config.vtkInterval << std::endl;
		std::cout << "charLength      = " << characteristicLength << std::endl;

		SimulationResult result = runCase(config, mask, characteristicLength);

		printSimulationSummary(result, config, characteristicLength);

		appendResultToCSV("./data/results/simulation_results.csv",
				casePaths.caseName,
				config,
				result,
				characteristicLength);

		std::cout.rdbuf(oldCoutBuf);
		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}
}
