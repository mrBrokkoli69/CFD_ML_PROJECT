#include "io/case_io.h"
#include "io/results_io.h"
#include "lbm/lbm_core.h"
#include "lbm/lbm_runner.h"
#include "mask_editor/mask_loader.h"
#include "post/postprocessing.h"

#include <iostream>
#include <stdexcept>
#include <string>

int runSolverMode(int argc, char* argv[]) {
	try {
		SimulationConfig config;
		std::string maskPath;
		double characteristicLength = 20.0;

		initInteractiveConfig(config);
		applyCommandLineArgs(config, maskPath, characteristicLength, argc, argv);

		CasePaths casePaths = createCasePaths(maskPath);
		config.outputDir = casePaths.caseDir;

		ScopedCaseLogger logger(casePaths.logPath);
		std::cout << "[DBG] entered solver mode" << std::endl;

		Mask mask = loadMaskFromDat(maskPath);
		std::cout << "[DBG] mask loaded" << std::endl;

		validateMaskSize(mask, config);
		std::cout << "[DBG] mask size validated" << std::endl;

		printCaseHeader(casePaths, config, mask, maskPath, characteristicLength);
		std::cout << "[DBG] case header printed" << std::endl;

		SimulationResult result = runCase(config, mask);
		std::cout << "[DBG] runCase finished" << std::endl;

		printSimulationSummary(result, config, characteristicLength);
		std::cout << "[DBG] summary printed" << std::endl;

		appendResultToCSV("./data/results/simulation_results.csv",
				casePaths.caseName,
				config,
				result,
				characteristicLength);
		std::cout << "[DBG] csv appended" << std::endl;
		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}
}
