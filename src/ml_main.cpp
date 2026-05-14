#include "ml/dataset.h"
#include "ml/mlp.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>

int main() {
    try {
        const std::string csvPath = "./data/results/dataset_samples.csv";

        Dataset rawDataset = loadDatasetFromCsv(csvPath);
        Dataset dataset = filterValidSamples(rawDataset);

        TrainTestSplit split = splitDataset(dataset, 0.8);

        NormalizationStats stats = computeNormalizationStats(split.train);
        applyNormalization(split.train, stats);
        applyNormalization(split.test, stats);

        std::cout << "Dataset loaded successfully" << std::endl;
        std::cout << "Raw samples   = " << rawDataset.samples.size() << std::endl;
        std::cout << "Valid samples = " << dataset.samples.size() << std::endl;
        std::cout << "Train samples = " << split.train.samples.size() << std::endl;
        std::cout << "Test samples  = " << split.test.samples.size() << std::endl;

        if (split.train.samples.empty() || split.test.samples.empty()) {
            throw std::runtime_error("Train or test split is empty");
        }

        const int inputSize = static_cast<int>(split.train.samples[0].features.size());
        const int outputSize = static_cast<int>(split.train.samples[0].targets.size());

        std::cout << "Input size    = " << inputSize << std::endl;
        std::cout << "Output size   = " << outputSize << std::endl;

        TrainingConfig config;
        config.epochs = 800;
        config.learningRate = 0.001;

        MLP model(inputSize, 16, 8, outputSize);

        std::mt19937 trainRng(2040);

        for (int epoch = 0; epoch < config.epochs; ++epoch) {
            double trainLossSum = 0.0;

            std::vector<int> trainIndices(split.train.samples.size());
            std::iota(trainIndices.begin(), trainIndices.end(), 0);
            std::shuffle(trainIndices.begin(), trainIndices.end(), trainRng);

            for (int index : trainIndices) {
                const Sample& sample = split.train.samples[index];

                Vector prediction = model.forward(sample.features);
                double loss = computeMSELoss(prediction, sample.targets);
                Vector grad = computeMSEGradient(prediction, sample.targets);

                model.backward(grad, config.learningRate);
                trainLossSum += loss;
            }

            double meanTrainLoss =
                trainLossSum / static_cast<double>(split.train.samples.size());

            if ((epoch + 1) % 10 == 0 || epoch == 0 || epoch == config.epochs - 1) {
                std::cout << "Epoch " << (epoch + 1)
                          << "/" << config.epochs
                          << " train MSE = " << meanTrainLoss
                          << std::endl;
            }
        }

        double testSquaredErrorSum = 0.0;
        double testAbsoluteErrorSum = 0.0;
        double testRelativeErrorSum = 0.0;

        for (const Sample& sample : split.test.samples) {
            Vector prediction = model.predict(sample.features);

            const double trueCd = sample.targets[0];
            const double predCd = prediction[0];
            const double diff = predCd - trueCd;
            const double absError = std::abs(diff);

            testSquaredErrorSum += diff * diff;
            testAbsoluteErrorSum += absError;

            if (std::abs(trueCd) > 1e-12) {
                testRelativeErrorSum += absError / std::abs(trueCd);
            }
        }

        const double testCount = static_cast<double>(split.test.samples.size());

        std::cout << "Test MSE  = " << testSquaredErrorSum / testCount << std::endl;
        std::cout << "Test MAE  = " << testAbsoluteErrorSum / testCount << std::endl;
        std::cout << "Test RMSE = " << std::sqrt(testSquaredErrorSum / testCount) << std::endl;
        std::cout << "Test relative error (%) = "
        << 100.0 * (testRelativeErrorSum / testCount)
        << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
