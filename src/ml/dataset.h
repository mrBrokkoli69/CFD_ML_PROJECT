#ifndef DATASET_H
#define DATASET_H

#include <string>
#include <vector>


struct Sample {
	std::vector<double> features;
	std::vector<double> targets;



};


struct Dataset {
	std::vector<Sample> samples;
	std::vector<std::string> featureNames;
	std::vector<std::string> targetNames;


};


struct NormalizationStats {
	std::vector<double> featuresMeans;
	std::vector<double> featureStds;


};

struct TrainTestSplit {
	Dataset train;
	Dataset test;


};


Dataset loadDatasetFromCsv(const std::string& csvPath);
Dataset filterValidSamples(const Dataset& dataset);
TrainTestSplit splitDataset(const Dataset& dataset, double trainRatio);
NormalizationStats computeNormalizationStats(const Dataset& dataset);
void applyNormalization(Dataset& dataset, const NormalizationStats& stats);

#endif
