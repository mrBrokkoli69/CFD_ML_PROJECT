#include "ml/dataset.h"
#include "ml/mlp.h"

#include <iostream>
#include <stdexcept>
#include <string>

#include <algorithm>
#include <numeric>
#include <random>

struct TargetNormalizationStats {
	Vector means;
	Vector stds;
};

TargetNormalizationStats computeTargetNormalizationStats(const Dataset& dataset) {
	if (dataset.samples.empty()) {
		throw std::runtime_error("Cannot compute target normalization stats for empty dataset");
	}

	const int targetSize = static_cast<int>(dataset.samples[0].targets.size());

	TargetNormalizationStats stats;
	stats.means.assign(targetSize, 0.0);
	stats.stds.assign(targetSize, 0.0);

	for (const Sample& sample : dataset.samples) {
		for (int j = 0; j < targetSize; ++j) {
			stats.means[j] += sample.targets[j];
		}
	}

	const double sampleCount = static_cast<double>(dataset.samples.size());
	for (int j = 0; j < targetSize; ++j) {
		stats.means[j] /= sampleCount;
	}

	for (const Sample& sample : dataset.samples) {
		for (int j = 0; j < targetSize; ++j) {
			const double diff = sample.targets[j] - stats.means[j];
			stats.stds[j] += diff * diff;
		}
	}

	for (int j = 0; j < targetSize; ++j) {
		stats.stds[j] = std::sqrt(stats.stds[j] / sampleCount);
		if (stats.stds[j] < 1e-12) {
			stats.stds[j] = 1.0;
		}
	}

	return stats;
}

void applyTargetNormalization(Dataset& dataset, const TargetNormalizationStats& stats) {
	for (Sample& sample : dataset.samples) {
		for (int j = 0; j < static_cast<int>(sample.targets.size()); ++j) {
			sample.targets[j] =
				(sample.targets[j] - stats.means[j]) / stats.stds[j];
		}
	}
}

Vector denormalizeTargets(const Vector& normalizedTargets,
		const TargetNormalizationStats& stats) {
	Vector result(normalizedTargets.size(), 0.0);

	for (int j = 0; j < static_cast<int>(normalizedTargets.size()); ++j) {
		result[j] = normalizedTargets[j] * stats.stds[j] + stats.means[j];
	}

	return result;
}

int main() {
	try {
		const std::string csvPath = "./data/results/dataset_samples.csv";

		Dataset rawDataset = loadDatasetFromCsv(csvPath);
		Dataset dataset = filterValidSamples(rawDataset);

		TrainTestSplit split = splitDataset(dataset, 0.8);

		NormalizationStats stats = computeNormalizationStats(split.train);
		applyNormalization(split.train, stats);
		applyNormalization(split.test, stats);

		TargetNormalizationStats targetStats =
			computeTargetNormalizationStats(split.train);

		applyTargetNormalization(split.train, targetStats);
		applyTargetNormalization(split.test, targetStats);

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
		config.epochs = 500;
		config.learningRate = 0.001;

		MLP model(inputSize, 32, 16, outputSize);


		std::mt19937 trainRng(20260430);

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

		double testMSESum = 0.0;
		double testMAESum = 0.0;
		double testRMSESum = 0.0;

		for (const Sample& sample : split.test.samples) {
			Vector predictionNorm = model.predict(sample.features);

			Vector prediction = denormalizeTargets(predictionNorm, targetStats);
			Vector target = denormalizeTargets(sample.targets, targetStats);

			testMSESum += computeMSELoss(prediction, target);
			testMAESum += computeMAE(prediction, target);
			testRMSESum += computeRMSE(prediction, target);
		}

		const double testCount = static_cast<double>(split.test.samples.size());

		std::cout << "Test MSE  = " << testMSESum / testCount << std::endl;
		std::cout << "Test MAE  = " << testMAESum / testCount << std::endl;
		std::cout << "Test RMSE = " << testRMSESum / testCount << std::endl;




		return 0;
	} catch (const std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}
}
