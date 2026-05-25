#include "mask_editor/editor.h"
#include "ml/inference_session.h"
#include "ml/predict_cd.h"


#include <cstring>
#include <iostream>
#include <string>


// объявление solver entry point
int runSolverMode(int argc, char* argv[]);

int main(int argc, char* argv[]) {
	const std::string modelPath = "./data/ml/cd_model.txt";
	const std::string normPath = "./data/ml/cd_norm.txt";

	InferenceSession inferenceSession;
	bool mlAvailable = false;
	double lastPredictedCd = 0.0;
	bool hasPredictedCd = false;

	try {
		inferenceSession = loadInferenceSession(modelPath, normPath);
		mlAvailable = inferenceSession.isLoaded;
	} catch (...) {
		mlAvailable = false;
	}

	if (argc >= 2 && std::strcmp(argv[1], "--solver") == 0) {
		return runSolverMode(argc - 1, argv + 1);
	}

	return runEditorApp(inferenceSession, mlAvailable);
}
