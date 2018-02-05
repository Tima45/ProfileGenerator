#include "neuron.h"

double Neuron::sigmoid(double value)
{
    return 1.0/(1.0+exp(-value));
}

double Neuron::devSigmoid(double value)
{
    double fx = sigmoid(value);
    return fx * (1.0 - fx);
}

Neuron::Neuron()
{

}

Neuron::~Neuron()
{

}
