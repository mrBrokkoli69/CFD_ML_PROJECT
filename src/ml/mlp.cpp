#include "mlp.h"

#include <cmath>
#include <random>
#include <stdexcept>

namespace {

	double randomWeight(std::mt19937& rng, int inputSize) {
		const double limit = std::sqrt(2.0 / static_cast<double>(inputSize));
		std::uniform_real_distribution<double> dist(-limit, limit);


		return dist(rng);
	}
	double dotProduct(const Vector& a, const Vector& b) {
		if (a.size() != b.size()) {
			throw std::runtime_error("dotProduct size mismatch");
		}

		double sum = 0.0;
		for (int i = 0; i < static_cast<int>(a.size()); ++i) {
			sum += a[i] * b[i];
		}
		return sum;
	}

} 


LinearLayer::LinearLayer(int inputSize, int outputSize): inputSize_(inputSize), outputSize_(outputSize) {
	if(inputSize<0 || outputSize<0) {
		throw std::runtime_error("sizes must be positive");

	}
	std::mt19937 rng(12345);

	weights_.assign(outputSize_, Vector(inputSize_, 0.0));
	biases_.assign(outputSize_, 0.0);

	for (int i = 0; i < outputSize_; ++i) {
		for (int j = 0; j < inputSize_; ++j) {
			weights_[i][j] = randomWeight(rng, inputSize_);
		}
	}
}

int LinearLayer::getInputSize() const {
	return inputSize_;
}

int LinearLayer::getOutputSize() const {
	return outputSize_;
}

Vector LinearLayer::forward(const Vector& input) {
    if (static_cast<int>(input.size()) != inputSize_) {
        throw std::runtime_error("LinearLayer::forward input size mismatch");
    }

    lastInput_ = input;

    Vector output(outputSize_, 0.0);

    for (int i = 0; i < outputSize_; ++i) {
        output[i] = dotProduct(weights_[i], input) + biases_[i];
    }

    return output;
}

Vector LinearLayer::backward(const Vector& gradOutput, double learningRate) {
    if (static_cast<int>(gradOutput.size()) != outputSize_) {
        throw std::runtime_error("LinearLayer::backward gradOutput size mismatch");
    }

    if (static_cast<int>(lastInput_.size()) != inputSize_) {
        throw std::runtime_error("LinearLayer::backward missing or invalid cached input");
    }

    Vector gradInput(inputSize_, 0.0);

    for (int j = 0; j < inputSize_; ++j) {
        for (int i = 0; i < outputSize_; ++i) {
            gradInput[j] += weights_[i][j] * gradOutput[i];
        }
    }

    for (int i = 0; i < outputSize_; ++i) {
        for (int j = 0; j < inputSize_; ++j) {
            const double gradWeight = gradOutput[i] * lastInput_[j];
            weights_[i][j] -= learningRate * gradWeight;
        }

        const double gradBias = gradOutput[i];
        biases_[i] -= learningRate * gradBias;
    }

    return gradInput;
}

Vector ReLULayer::forward(const Vector& input) {
    lastInput_ = input;

    Vector output(input.size(), 0.0);

    for (int i = 0; i < static_cast<int>(input.size()); ++i) {
        if (input[i] > 0.0) {
            output[i] = input[i];
        } else {
            output[i] = 0.0;
        }
    }

    return output;
}

Vector ReLULayer::backward(const Vector& gradOutput) {
    if (gradOutput.size() != lastInput_.size()) {
        throw std::runtime_error("ReLULayer::backward size mismatch");
    }

    Vector gradInput(gradOutput.size(), 0.0);

    for (int i = 0; i < static_cast<int>(gradOutput.size()); ++i) {
        if (lastInput_[i] > 0.0) {
            gradInput[i] = gradOutput[i];
        } else {
            gradInput[i] = 0.0;
        }
    }

    return gradInput;
}

MLP::MLP(int inputSize, int hiddenSize1, int hiddenSize2, int outputSize)
    : layer1_(inputSize, hiddenSize1),
      relu1_(),
      layer2_(hiddenSize1, hiddenSize2),
      relu2_(),
      outputLayer_(hiddenSize2, outputSize) {
    if (inputSize <= 0 || hiddenSize1 <= 0 || hiddenSize2 <= 0 || outputSize <= 0) {
        throw std::runtime_error("MLP sizes must be positive");
    }
}



Vector MLP::forward(const Vector& input) {
    Vector z1 = layer1_.forward(input);
    Vector a1 = relu1_.forward(z1);

    Vector z2 = layer2_.forward(a1);
    Vector a2 = relu2_.forward(z2);

    Vector output = outputLayer_.forward(a2);
    return output;
}

void MLP::backward(const Vector& gradOutput, double learningRate) {
    Vector gradA2 = outputLayer_.backward(gradOutput, learningRate);
    Vector gradZ2 = relu2_.backward(gradA2);

    Vector gradA1 = layer2_.backward(gradZ2, learningRate);
    Vector gradZ1 = relu1_.backward(gradA1);

    layer1_.backward(gradZ1, learningRate);
}

Vector MLP::predict(const Vector& input) {
    return forward(input);
}

double computeMSELoss(const Vector& prediction, const Vector& target) {
    if (prediction.size() != target.size()) {
        throw std::runtime_error("computeMSELoss size mismatch");
    }

    double sum = 0.0;
    for (int i = 0; i < static_cast<int>(prediction.size()); ++i) {
        const double diff = prediction[i] - target[i];
        sum += diff * diff;
    }

    return sum / static_cast<double>(prediction.size());
}

Vector computeMSEGradient(const Vector& prediction, const Vector& target) {
    if (prediction.size() != target.size()) {
        throw std::runtime_error("computeMSEGradient size mismatch");
    }

    Vector gradient(prediction.size(), 0.0);
    const double n = static_cast<double>(prediction.size());

    for (int i = 0; i < static_cast<int>(prediction.size()); ++i) {
        gradient[i] = 2.0 * (prediction[i] - target[i]) / n;
    }

    return gradient;
}

double computeMAE(const Vector& prediction, const Vector& target) {
    if (prediction.size() != target.size()) {
        throw std::runtime_error("computeMAE size mismatch");
    }

    double sum = 0.0;
    for (int i = 0; i < static_cast<int>(prediction.size()); ++i) {
        sum += std::abs(prediction[i] - target[i]);
    }

    return sum / static_cast<double>(prediction.size());
}

double computeRMSE(const Vector& prediction, const Vector& target) {
    return std::sqrt(computeMSELoss(prediction, target));
}



