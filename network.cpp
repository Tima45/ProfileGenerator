#include "network.h"

bool Network::isPowerOfTwo(int value)
{
    return (value & (value - 1)) == 0;
}

Network::Network(unsigned int size)
{
    nSize = size;

    lastN = new Neuron2i();

    middleN1 = new NeuronNi(4);
    middleN2 = new NeuronNi(4);

    firstN1 = new NeuronNi(nSize);
    firstN2 = new NeuronNi(nSize);
    firstN3 = new NeuronNi(nSize);
    firstN4 = new NeuronNi(nSize);

    resultsFirstLayer = new double[4];
}

Network::~Network()
{
    delete lastN;
    delete middleN1;
    delete middleN2;

    delete firstN1;
    delete firstN2;
    delete firstN3;
    delete firstN4;
    delete[] resultsFirstLayer;
}

double Network::work(const double *inputs, unsigned int size)
{
    double result = -1;
    if(nSize == size){
        resultsFirstLayer[0] = firstN1->work(inputs,size);
        resultsFirstLayer[1] = firstN2->work(inputs,size);
        resultsFirstLayer[2] = firstN3->work(inputs,size);
        resultsFirstLayer[3] = firstN4->work(inputs,size);

        double resultMN1 = middleN1->work(resultsFirstLayer,4);
        double resultMN2 = middleN2->work(resultsFirstLayer,4);

        return lastN->work(resultMN1,resultMN2);
    }
    return result;
}

double Network::train(double speed, const double *inputs, unsigned int size, double expectedValue)
{
    double commonError = 0;
    if(nSize == size){
        resultsFirstLayer[0] = firstN1->work(inputs,size);
        resultsFirstLayer[1] = firstN2->work(inputs,size);
        resultsFirstLayer[2] = firstN3->work(inputs,size);
        resultsFirstLayer[3] = firstN4->work(inputs,size);

        double resultMN1 = middleN1->work(resultsFirstLayer,4);
        double resultMN2 = middleN2->work(resultsFirstLayer,4);

        double result = lastN->work(resultMN1,resultMN2);

        commonError = expectedValue - result;

        double correctionForMiddle = lastN->calculateCorrection(speed,commonError,resultMN1,resultMN2);

        double correctionFromMiddle1 = middleN1->calculateCorrection(speed,correctionForMiddle,resultsFirstLayer,4);
        double correctionFromMiddle2 = middleN2->calculateCorrection(speed,correctionForMiddle,resultsFirstLayer,4);

        firstN1->calculateCorrection(speed,correctionFromMiddle1,inputs,4);
        firstN2->calculateCorrection(speed,correctionFromMiddle1,inputs,4);
        firstN3->calculateCorrection(speed,correctionFromMiddle1,inputs,4);
        firstN4->calculateCorrection(speed,correctionFromMiddle1,inputs,4);

        firstN1->calculateCorrection(speed,correctionFromMiddle2,inputs,4);
        firstN2->calculateCorrection(speed,correctionFromMiddle2,inputs,4);
        firstN3->calculateCorrection(speed,correctionFromMiddle2,inputs,4);
        firstN4->calculateCorrection(speed,correctionFromMiddle2,inputs,4);
    }
    apllayResult();
    return commonError;
}

void Network::apllayResult()
{
    lastN->updateWeights();
    middleN1->updateWeights();
    middleN2->updateWeights();
    firstN1->updateWeights();
    firstN2->updateWeights();
    firstN3->updateWeights();
    firstN4->updateWeights();
}

Network::Network()
{
    nSize = 0;
}
