#include "results_io.h"
#include "../post/postprocessing.h"

#include <filesystem>
#include <fstream>

void appendResultToCSV(const std::string& csvPath,
                       const std::string& caseName,
                       const SimulationConfig& config,
                       const SimulationResult& result,
                       double characteristicLength) {
    namespace fs = std::filesystem;

    fs::path path(csvPath);

    if (path.has_parent_path()) {
        fs::create_directories(path.parent_path());
    }

    bool fileExists = fs::exists(path);
    bool writeHeader = true;

    if (fileExists) {
        std::ifstream in(csvPath);
        writeHeader = (in.peek() == std::ifstream::traits_type::eof());
    }

    std::ofstream out(csvPath, std::ios::app);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open CSV file for writing: " + csvPath);
    }

    if (writeHeader) {
        out << "case_name,"
            << "nx,ny,"
            << "tau,rho0,uMax,"
            << "characteristic_length,"
            << "steps_done,"
            << "fx,fy,"
            << "avgFx,avgFy,"
            << "Cd,Cl\n";
    }

    double cd = computeDragCoefficient(result.avgFx, config, characteristicLength);
    double cl = computeLiftCoefficient(result.avgFy, config, characteristicLength);

    out << caseName << ","
        << config.nx << ","
        << config.ny << ","
        << config.tau << ","
        << config.rho0 << ","
        << config.uMax << ","
        << characteristicLength << ","
        << result.stepsDone << ","
        << result.fx << ","
        << result.fy << ","
        << result.avgFx << ","
        << result.avgFy << ","
        << cd << ","
        << cl << "\n";
}
