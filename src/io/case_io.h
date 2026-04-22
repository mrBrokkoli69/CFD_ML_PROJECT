#pragma once

#include "../lbm/lbm_core.h"

#include <fstream>
#include <memory>
#include <streambuf>
#include <string>

struct CasePaths {
    std::string caseName;
    std::string caseDir;
    std::string logPath;
};

std::string extractMaskBaseName(const std::string& maskPath);
CasePaths createCasePaths(const std::string& maskPath);

void initInteractiveConfig(SimulationConfig& config);
void applyCommandLineArgs(SimulationConfig& config,
                          std::string& maskPath,
                          double& characteristicLength,
                          int argc,
                          char* argv[]);

void validateMaskSize(const Mask& mask, const SimulationConfig& config);

void printCaseHeader(const CasePaths& casePaths,
                     const SimulationConfig& config,
                     const Mask& mask,
                     const std::string& maskPath,
                     double characteristicLength);

class ScopedCaseLogger {
public:
    explicit ScopedCaseLogger(const std::string& logPath);
    ~ScopedCaseLogger();

    ScopedCaseLogger(const ScopedCaseLogger&) = delete;
    ScopedCaseLogger& operator=(const ScopedCaseLogger&) = delete;

private:
    std::ofstream logFile_;
    std::streambuf* oldCoutBuf_ = nullptr;
    std::unique_ptr<std::streambuf> teeBuf_;
};
