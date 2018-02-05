#include "neuron2i.h"

Neuron2i::Neuron2i() : Neuron()
{
    w1 = (rand()/(double)RAND_MAX - 0.5);
    w2 = (rand()/(double)RAND_MAX - 0.5);
    bias = (rand()/(double)RAND_MAX - 0.5);
    deltaW1 = 0;
    deltaW2 = 0;
    deltaBias = 0;
}

Neuron2i::~Neuron2i()
{

}

double Neuron2i::work(double input1, double input2)
{
    return Neuron::sigmoid(w1*input1 + w2*input2 + bias);
}

double Neuron2i::calculateCorrection(double speed, double error, double input1, double input2)
{
    double correction = error * Neuron::devSigmoid(w1*input1 + w2*input2 + bias);
    deltaW1 += correction * speed * input1;
    deltaW2 += correction * speed * input2;
    deltaBias += correction * speed;
    return correction;
}

void Neuron2i::updateWeights()
{
    w1 += deltaW1;
    w2 += deltaW2;
    bias += deltaBias;
    deltaW1 = 0;
    deltaW2 = 0;
    deltaBias = 0;
}
