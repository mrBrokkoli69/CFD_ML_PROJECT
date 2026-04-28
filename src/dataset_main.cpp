#include "lbm/lbm_core.h"
#include "lbm/lbm_runner.h"
#include "mask_editor/editor.h"
#include "mask_editor/shape_generator.h"
#include "post/postprocessing.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

enum class ShapeType {
    Circle,
    Rectangle,
    Ellipse,
    NACA0012,
    NACA2412
};

struct GeneratorConfig {
    int samplesPerShape = 150;

    int maxSteps = 5000;
    int coutInterval = 1000;
    int vtkInterval = 0;

    double tauMin = 0.58;
    double tauMax = 0.96;
    double uMaxMin = 0.04;
    double uMaxMax = 0.10;
    double rho0 = 1.0;

    int leftMargin = 45;
    int rightMargin = 20;
    int topMargin = 6;
    int bottomMargin = 6;

    std::string outputCsvPath = "./data/results/dataset_samples.csv";
    unsigned int randomSeed = 1111;
    bool verbose = false;
};

struct SampleSpec {
    ShapeType shapeType{};
    int sampleId = 0;

    int anchorX = -1;
    int anchorY = -1;

    int radius = -1;

    int rectX1 = -1;
    int rectY1 = -1;
    int rectX2 = -1;
    int rectY2 = -1;
    int rectWidth = -1;
    int rectHeight = -1;

    int ellipseRx = -1;
    int ellipseRy = -1;

    int chord = -1;

    int bboxMinX = -1;
    int bboxMaxX = -1;
    int bboxMinY = -1;
    int bboxMaxY = -1;

    int solidCells = 0;

    double characteristicLength = 1.0;
};

class ScopedSilenceCout {
public:
    explicit ScopedSilenceCout(bool enabled) : enabled_(enabled) {
        if (enabled_) {
            oldBuf_ = std::cout.rdbuf(nullBuf_.rdbuf());
        }
    }

    ~ScopedSilenceCout() {
        if (enabled_) {
            std::cout.rdbuf(oldBuf_);
        }
    }

private:
    bool enabled_ = false;
    std::streambuf* oldBuf_ = nullptr;
    std::ostringstream nullBuf_;
};

std::string shapeTypeToString(ShapeType shapeType) {
    switch (shapeType) {
        case ShapeType::Circle: return "circle";
        case ShapeType::Rectangle: return "rectangle";
        case ShapeType::Ellipse: return "ellipse";
        case ShapeType::NACA0012: return "naca0012";
        case ShapeType::NACA2412: return "naca2412";
    }
    return "unknown";
}

Mask convertEditorMaskToSolverMask(const EditorMask& editorMask) {
    Mask mask;
    mask.nx = EditorMask::WIDTH;
    mask.ny = EditorMask::HEIGHT;
    mask.solid.assign(mask.ny, std::vector<bool>(mask.nx, false));

    for (int y = 0; y < mask.ny; ++y) {
        for (int x = 0; x < mask.nx; ++x) {
            mask.solid[y][x] = editorMask.cells[y][x];
        }
    }

    return mask;
}

void computeMaskStats(const EditorMask& mask, SampleSpec& spec) {
    int minX = EditorMask::WIDTH;
    int maxX = -1;
    int minY = EditorMask::HEIGHT;
    int maxY = -1;
    int solidCells = 0;

    for (int y = 0; y < EditorMask::HEIGHT; ++y) {
        for (int x = 0; x < EditorMask::WIDTH; ++x) {
            if (!mask.cells[y][x]) {
                continue;
            }
            ++solidCells;
            minX = std::min(minX, x);
            maxX = std::max(maxX, x);
            minY = std::min(minY, y);
            maxY = std::max(maxY, y);
        }
    }

    spec.solidCells = solidCells;
    if (solidCells > 0) {
        spec.bboxMinX = minX;
        spec.bboxMaxX = maxX;
        spec.bboxMinY = minY;
        spec.bboxMaxY = maxY;
    }
}

SimulationConfig makeSimulationConfig(const GeneratorConfig& generatorConfig,
                                      double tau,
                                      double rho0,
                                      double uMax) {
    SimulationConfig config;
    config.nx = EditorMask::WIDTH;
    config.ny = EditorMask::HEIGHT;
    config.maxSteps = generatorConfig.maxSteps;
    config.vtkInterval = generatorConfig.vtkInterval;
    config.coutInterval = generatorConfig.coutInterval;
    config.tau = tau;
    config.rho0 = rho0;
    config.uMax = uMax;
    config.outputDir.clear();
    return config;
}

double sampleReal(double minValue, double maxValue, std::mt19937& rng) {
    std::uniform_real_distribution<double> dist(minValue, maxValue);
    return dist(rng);
}

SampleSpec sampleCircleSpec(int sampleId, const GeneratorConfig& cfg, std::mt19937& rng) {
    SampleSpec spec;
    spec.sampleId = sampleId;
    spec.shapeType = ShapeType::Circle;

    std::uniform_int_distribution<int> radiusDist(6, 14);
    spec.radius = radiusDist(rng);

    std::uniform_int_distribution<int> xDist(cfg.leftMargin + spec.radius,
                                             EditorMask::WIDTH - cfg.rightMargin - spec.radius - 1);
    std::uniform_int_distribution<int> yDist(cfg.topMargin + spec.radius,
                                             EditorMask::HEIGHT - cfg.bottomMargin - spec.radius - 1);
    spec.anchorX = xDist(rng);
    spec.anchorY = yDist(rng);
    spec.characteristicLength = static_cast<double>(2 * spec.radius);
    return spec;
}

SampleSpec sampleRectangleSpec(int sampleId, const GeneratorConfig& cfg, std::mt19937& rng) {
    SampleSpec spec;
    spec.sampleId = sampleId;
    spec.shapeType = ShapeType::Rectangle;

    std::uniform_int_distribution<int> widthDist(12, 40);
    std::uniform_int_distribution<int> heightDist(8, 22);
    spec.rectWidth = widthDist(rng);
    spec.rectHeight = heightDist(rng);

    std::uniform_int_distribution<int> x1Dist(cfg.leftMargin,
                                              EditorMask::WIDTH - cfg.rightMargin - spec.rectWidth - 1);
    std::uniform_int_distribution<int> y1Dist(cfg.topMargin,
                                              EditorMask::HEIGHT - cfg.bottomMargin - spec.rectHeight - 1);
    spec.rectX1 = x1Dist(rng);
    spec.rectY1 = y1Dist(rng);
    spec.rectX2 = spec.rectX1 + spec.rectWidth - 1;
    spec.rectY2 = spec.rectY1 + spec.rectHeight - 1;
    spec.anchorX = spec.rectX1;
    spec.anchorY = spec.rectY1;
    spec.characteristicLength = static_cast<double>(spec.rectWidth);
    return spec;
}

SampleSpec sampleEllipseSpec(int sampleId, const GeneratorConfig& cfg, std::mt19937& rng) {
    SampleSpec spec;
    spec.sampleId = sampleId;
    spec.shapeType = ShapeType::Ellipse;

    std::uniform_int_distribution<int> rxDist(8, 20);
    std::uniform_int_distribution<int> ryDist(5, 14);
    spec.ellipseRx = rxDist(rng);
    spec.ellipseRy = ryDist(rng);

    std::uniform_int_distribution<int> xDist(cfg.leftMargin + spec.ellipseRx,
                                             EditorMask::WIDTH - cfg.rightMargin - spec.ellipseRx - 1);
    std::uniform_int_distribution<int> yDist(cfg.topMargin + spec.ellipseRy,
                                             EditorMask::HEIGHT - cfg.bottomMargin - spec.ellipseRy - 1);
    spec.anchorX = xDist(rng);
    spec.anchorY = yDist(rng);
    spec.characteristicLength = static_cast<double>(2 * spec.ellipseRx);
    return spec;
}

SampleSpec sampleNacaSpec(int sampleId, const GeneratorConfig& cfg, std::mt19937& rng, ShapeType shapeType) {
    SampleSpec spec;
    spec.sampleId = sampleId;
    spec.shapeType = shapeType;

    std::uniform_int_distribution<int> chordDist(30, 80);
    spec.chord = chordDist(rng);

    std::uniform_int_distribution<int> xDist(cfg.leftMargin,
                                             EditorMask::WIDTH - cfg.rightMargin - spec.chord - 1);
    std::uniform_int_distribution<int> yDist(cfg.topMargin + 8,
                                             EditorMask::HEIGHT - cfg.bottomMargin - 9);
    spec.anchorX = xDist(rng);
    spec.anchorY = yDist(rng);
    spec.characteristicLength = static_cast<double>(spec.chord);
    return spec;
}

SampleSpec makeSampleSpec(ShapeType shapeType,
                          int sampleId,
                          const GeneratorConfig& cfg,
                          std::mt19937& rng) {
    switch (shapeType) {
        case ShapeType::Circle:
            return sampleCircleSpec(sampleId, cfg, rng);
        case ShapeType::Rectangle:
            return sampleRectangleSpec(sampleId, cfg, rng);
        case ShapeType::Ellipse:
            return sampleEllipseSpec(sampleId, cfg, rng);
        case ShapeType::NACA0012:
            return sampleNacaSpec(sampleId, cfg, rng, ShapeType::NACA0012);
        case ShapeType::NACA2412:
            return sampleNacaSpec(sampleId, cfg, rng, ShapeType::NACA2412);
    }
    throw std::runtime_error("Unsupported shape type");
}

void drawShapeForSample(EditorMask& mask, const SampleSpec& spec) {
    switch (spec.shapeType) {
        case ShapeType::Circle:
            drawCircle(mask, spec.anchorX, spec.anchorY, spec.radius);
            return;
        case ShapeType::Rectangle:
            drawRectangle(mask, spec.rectX1, spec.rectY1, spec.rectX2, spec.rectY2);
            return;
        case ShapeType::Ellipse:
            drawEllipse(mask, spec.anchorX, spec.anchorY, spec.ellipseRx, spec.ellipseRy);
            return;
        case ShapeType::NACA0012:
            drawNACA0012(mask, spec.anchorX, spec.anchorY, spec.chord);
            return;
        case ShapeType::NACA2412:
            drawNACA2412(mask, spec.anchorX, spec.anchorY, spec.chord);
            return;
    }
}

void ensureCsvHeader(const std::string& csvPath) {
    fs::path path(csvPath);
    if (path.has_parent_path()) {
        fs::create_directories(path.parent_path());
    }

    bool writeHeader = true;
    if (fs::exists(path)) {
        std::ifstream in(csvPath);
        writeHeader = (in.peek() == std::ifstream::traits_type::eof());
    }

    if (!writeHeader) {
        return;
    }

    std::ofstream out(csvPath, std::ios::app);
    out << "sample_id,shape_type,"
        << "nx,ny,"
        << "tau,rho0,uMax,uMean,maxSteps,coutInterval,"
        << "characteristic_length,reynolds,"
        << "anchor_x,anchor_y,"
        << "radius,"
        << "rect_x1,rect_y1,rect_x2,rect_y2,rect_width,rect_height,"
        << "ellipse_rx,ellipse_ry,"
        << "chord,"
        << "bbox_min_x,bbox_max_x,bbox_min_y,bbox_max_y,"
        << "solid_cells,solid_fraction,"
        << "fx,fy,avgFx,avgFy,Cd,Cl,status,elapsed_ms\n";
}

void appendDatasetRow(const std::string& csvPath,
                      const SampleSpec& spec,
                      const SimulationConfig& config,
                      const SimulationResult& result,
                      const std::string& status,
                      long long elapsedMs) {
    std::ofstream out(csvPath, std::ios::app);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open dataset CSV: " + csvPath);
    }

    const double uMean = computeMeanInletVelocity(config);
    const double reynolds = computeReynoldsNumber(config, spec.characteristicLength);
    const double cd = computeDragCoefficient(result.avgFx, config, spec.characteristicLength);
    const double cl = computeLiftCoefficient(result.avgFy, config, spec.characteristicLength);
    const double solidFraction = static_cast<double>(spec.solidCells)
        / static_cast<double>(config.nx * config.ny);

    out << spec.sampleId << ","
        << shapeTypeToString(spec.shapeType) << ","
        << config.nx << ","
        << config.ny << ","
        << config.tau << ","
        << config.rho0 << ","
        << config.uMax << ","
        << uMean << ","
        << config.maxSteps << ","
        << config.coutInterval << ","
        << spec.characteristicLength << ","
        << reynolds << ","
        << spec.anchorX << ","
        << spec.anchorY << ","
        << spec.radius << ","
        << spec.rectX1 << ","
        << spec.rectY1 << ","
        << spec.rectX2 << ","
        << spec.rectY2 << ","
        << spec.rectWidth << ","
        << spec.rectHeight << ","
        << spec.ellipseRx << ","
        << spec.ellipseRy << ","
        << spec.chord << ","
        << spec.bboxMinX << ","
        << spec.bboxMaxX << ","
        << spec.bboxMinY << ","
        << spec.bboxMaxY << ","
        << spec.solidCells << ","
        << solidFraction << ","
        << result.fx << ","
        << result.fy << ","
        << result.avgFx << ","
        << result.avgFy << ","
        << cd << ","
        << cl << ","
        << status << ","
        << elapsedMs << "\n";
}

SimulationResult makeFailedResult() {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    SimulationResult result;
    result.fx = nan;
    result.fy = nan;
    result.avgFx = nan;
    result.avgFy = nan;
    result.stepsDone = 0;
    return result;
}

int main() {
    try {
        const GeneratorConfig generatorConfig;
        ensureCsvHeader(generatorConfig.outputCsvPath);

        std::mt19937 rng(generatorConfig.randomSeed);
        const std::vector<ShapeType> shapeTypes = {
            ShapeType::Circle,
            ShapeType::Rectangle,
            ShapeType::Ellipse,
            ShapeType::NACA0012,
            ShapeType::NACA2412
        };

        int globalSampleId = 0;
        const auto allStart = std::chrono::steady_clock::now();

        for (ShapeType shapeType : shapeTypes) {
            for (int localIndex = 0; localIndex < generatorConfig.samplesPerShape; ++localIndex) {
                ++globalSampleId;

                SampleSpec spec = makeSampleSpec(shapeType, globalSampleId, generatorConfig, rng);
                EditorMask editorMask;
                drawShapeForSample(editorMask, spec);
                computeMaskStats(editorMask, spec);

                Mask solverMask = convertEditorMaskToSolverMask(editorMask);

                const double tau = sampleReal(generatorConfig.tauMin, generatorConfig.tauMax, rng);
                const double uMax = sampleReal(generatorConfig.uMaxMin, generatorConfig.uMaxMax, rng);
                SimulationConfig config = makeSimulationConfig(generatorConfig,
                                                              tau,
                                                              generatorConfig.rho0,
                                                              uMax);

                std::cout << "[DATASET] sample " << globalSampleId
                          << " shape=" << shapeTypeToString(shapeType)
                          << " tau=" << tau
                          << " uMax=" << uMax
                          << " charL=" << spec.characteristicLength
                          << std::endl;

                SimulationResult result = makeFailedResult();
                std::string status = "ok";
                long long elapsedMs = -1;

                try {
                    const auto caseStart = std::chrono::steady_clock::now();
                    {
                        ScopedSilenceCout silence(!generatorConfig.verbose);
                        result = runCase(config, solverMask);
                    }
                    const auto caseEnd = std::chrono::steady_clock::now();
                    elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(caseEnd - caseStart).count();
                }
                catch (const std::exception& e) {
                    status = std::string("failed: ") + e.what();
                }

                std::cout << "[DATASET] finished sample " << globalSampleId
                          << " shape=" << shapeTypeToString(shapeType)
                          << " status=" << status
                          << " time=" << elapsedMs << " ms";

                if (status == "ok") {
                    const double cd = computeDragCoefficient(result.avgFx, config, spec.characteristicLength);
                    const double cl = computeLiftCoefficient(result.avgFy, config, spec.characteristicLength);
                    std::cout << " Cd=" << cd
                              << " Cl=" << cl;
                }

                std::cout << std::endl;

                appendDatasetRow(generatorConfig.outputCsvPath, spec, config, result, status, elapsedMs);
            }
        }

        const auto allEnd = std::chrono::steady_clock::now();
        const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(allEnd - allStart);

        std::cout << "Dataset generation finished" << std::endl;
        std::cout << "CSV path      = " << generatorConfig.outputCsvPath << std::endl;
        std::cout << "Total samples = " << globalSampleId << std::endl;
        std::cout << "Elapsed time  = " << elapsedMs.count() << " ms" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
