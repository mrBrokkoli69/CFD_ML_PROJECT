#ifndef MLP_H
#define MLP_H

#include <vector>
#include <string>


using Vector = std::vector<double>;
using Matrix = std::vector<std::vector<double>>;

struct TrainingConfig {
	int epochs = 500;
	double learningRate = 0.001;


};
class MLP;
class LinearLayer {
	public:
		LinearLayer(int inputSize, int outputSize);

		Vector forward(const Vector& input);
		Vector backward(const Vector& gradOutput, double learningRate);

		int getInputSize() const;
		int getOutputSize() const;

		const Matrix& getWeights() const;
		const Vector& getBiases() const;

		void setWeights(const Matrix& weights);
		void setBiases(const Vector& biases);

	private:
		int inputSize_;
		int outputSize_;

		Matrix weights_;
		Vector biases_;

		Vector lastInput_;
		friend void saveModel(const MLP& model, const std::string& filePath);
};

class ReLULayer {
	public:
		Vector forward(const Vector& input);
		Vector backward(const Vector& gradOutput);

	private:
		Vector lastInput_;

};

class MLP{
	public:
		MLP(int inputSize, int hiddenSize1, int hiddenSize2, int outputSize);

		Vector forward(const Vector& input);
		void backward(const Vector& gradOutput, double learningRate);

		Vector predict(const Vector& input);

		const LinearLayer& getLayer1() const;
		const LinearLayer& getLayer2() const;
		const LinearLayer& getOutputLayer() const;

		LinearLayer& accessLayer1();
		LinearLayer& accessLayer2();
		LinearLayer& accessOutputLayer();

	private:
		LinearLayer layer1_;
		ReLULayer relu1_;
		LinearLayer layer2_;
		ReLULayer relu2_;
		LinearLayer outputLayer_;

		friend void saveModel(const MLP& model, const std::string& filePath);
};


double computeMSELoss (const Vector& predictions, const Vector& target);
Vector computeMSEGradient (const Vector& predictions, const Vector& target);

double computeMAE (const Vector& predictions, const Vector& target);
double computeRMSE (const Vector& predictions, const Vector& target);

void saveModel(const MLP& model, const std::string& filePath);
MLP loadModel(const std::string& filePath);



#endif
