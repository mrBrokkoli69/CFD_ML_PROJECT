#include "case_io.h"
#include "../post/postprocessing.h"

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace fs = std::filesystem;

namespace {

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

} // namespace

std::string extractMaskBaseName(const std::string& maskPath) {
    fs::path path(maskPath);
    return path.stem().string();
}

CasePaths createCasePaths(const std::string& maskPath) {
    fs::create_directories("data/case_runs");

    std::string maskBase = extractMaskBaseName(maskPath);

    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &nowTime);
#else
    localtime_r(&nowTime, &localTime);
#endif

    std::ostringstream nameBuilder;
    nameBuilder << maskBase << "_"
                << std::put_time(&localTime, "%Y%m%d_%H%M%S");

    std::string caseName = nameBuilder.str();
    std::string caseDir = "data/case_runs/" + caseName;
    std::string logPath = caseDir + "/run.log";

    fs::create_directories(caseDir);

    CasePaths paths;
    paths.caseName = caseName;
    paths.caseDir = caseDir;
    paths.logPath = logPath;
    return paths;
}

void initInteractiveConfig(SimulationConfig& config) {
    config.nx = 300;
    config.ny = 80;

    config.tau = 0.6;
    config.rho0 = 1.0;
    config.uMax = 0.05;

    config.maxSteps = 3000;
    config.vtkInterval = 0;
    config.coutInterval = 1000;

    config.outputDir.clear();
}

void applyCommandLineArgs(SimulationConfig& config,
                          std::string& maskPath,
                          double& characteristicLength,
                          int argc,
                          char* argv[]) {
    maskPath = "./mask.dat";
    characteristicLength = 20.0;

    if (argc >= 2) maskPath = argv[1];
    if (argc >= 3) config.tau = std::atof(argv[2]);
    if (argc >= 4) config.uMax = std::atof(argv[3]);
    if (argc >= 5) config.rho0 = std::atof(argv[4]);
    if (argc >= 6) config.maxSteps = std::atoi(argv[5]);
    if (argc >= 7) config.coutInterval = std::atoi(argv[6]);
    if (argc >= 8) config.vtkInterval = std::atoi(argv[7]);
    if (argc >= 9) characteristicLength = std::atof(argv[8]);
}

void validateMaskSize(const Mask& mask, const SimulationConfig& config) {
    if (mask.nx != config.nx || mask.ny != config.ny) {
        throw std::runtime_error(
            "Loaded mask size does not match config: mask = " +
            std::to_string(mask.nx) + "x" + std::to_string(mask.ny) +
            ", config = " +
            std::to_string(config.nx) + "x" + std::to_string(config.ny)
        );
    }
}

void printCaseHeader(const CasePaths& casePaths,
                     const SimulationConfig& config,
                     const Mask& mask,
                     const std::string& maskPath,
                     double characteristicLength) {
    double uMean = computeMeanInletVelocity(config);
    double re = computeReynoldsNumber(config, characteristicLength);

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
}

ScopedCaseLogger::ScopedCaseLogger(const std::string& logPath) {
    logFile_.open(logPath);
    if (!logFile_.is_open()) {
        throw std::runtime_error("Cannot open log file: " + logPath);
    }

    teeBuf_ = std::make_unique<TeeBuf>(std::cout.rdbuf(), logFile_.rdbuf());
    oldCoutBuf_ = std::cout.rdbuf(teeBuf_.get());
}

ScopedCaseLogger::~ScopedCaseLogger() {
    if (oldCoutBuf_ != nullptr) {
        std::cout.rdbuf(oldCoutBuf_);
    }
}
