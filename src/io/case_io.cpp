#include "case_io.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

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
