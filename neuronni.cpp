#include "neuronni.h"

NeuronNi::NeuronNi(unsigned int inputCount) : Neuron(), inputCount(inputCount)
{
    weights = new double[inputCount];
    deltaWeights = new double[inputCount];
    for(unsigned int i = 0; i < inputCount; i++){
        weights[i] = (rand()/(double)RAND_MAX - 0.5);
        deltaWeights[i] = 0;
    }
    bias = (rand()/(double)RAND_MAX - 0.5);
    deltaBias = 0;
}

NeuronNi::NeuronNi(const NeuronNi &other)
{
    inputCount = other.inputCount;
    weights = new double[inputCount];
    deltaWeights = new double[inputCount];
    for(unsigned int i = 0; i < inputCount; i++){
        weights[i] = other.weights[i];
        deltaWeights[i] = 0;
    }
    bias = other.bias;
    deltaBias = 0;
}

double NeuronNi::work(const double *inputs, unsigned int inputCount)
{
    double result = 0;
    if(this->inputCount == inputCount){
        for(unsigned int i = 0; i < inputCount; i++){
            result += inputs[i]*weights[i];
        }
        result += bias;
        result = Neuron::sigmoid(result);
    }
    return result;
}

double NeuronNi::calculateCorrection(double speed, double error, const double *inputs, unsigned int inputCount)
{
    double correction = 0;
    if(this->inputCount == inputCount){
        for(unsigned int i = 0; i < inputCount; i++){
            correction += inputs[i]*weights[i];
        }
        correction += bias;
        correction = error * Neuron::devSigmoid(correction);

        for(unsigned int i = 0; i < inputCount; i++){
            deltaWeights[i] += correction * speed * inputs[i];
        }
        deltaBias += correction * speed;

    }
    return correction;
}

void NeuronNi::updateWeights()
{
    for(unsigned int i = 0; i < inputCount; i++){
        weights[i] += deltaWeights[i];
        deltaWeights[i] = 0;
    }
    bias += deltaBias;
    deltaBias = 0;
}

NeuronNi::~NeuronNi()
{
    delete[] weights;
    delete[] deltaWeights;
}
