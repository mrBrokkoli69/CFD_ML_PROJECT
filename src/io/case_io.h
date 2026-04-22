#pragma once

#include <string>

struct CasePaths {
    std::string caseName;
    std::string caseDir;
    std::string logPath;
};

CasePaths createCasePaths(const std::string& maskPath);
std::string extractMaskBaseName(const std::string& maskPath);
