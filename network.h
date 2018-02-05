#ifndef NETWORK_H
#define NETWORK_H

#include <math.h>
#include "neuron2i.h"
#include "neuronni.h"


class Network
{
public:
    static bool isPowerOfTwo(int value);
    Network(unsigned int size);
    virtual ~Network();
    double work(const double *inputs,unsigned int size);
    double train(double speed,const double *inputs,unsigned int size,double expectedValue); // returns error
    void apllayResult();

    Neuron2i *lastN;

    NeuronNi *middleN1;
    NeuronNi *middleN2;

    NeuronNi *firstN1;
    NeuronNi *firstN2;
    NeuronNi *firstN3;
    NeuronNi *firstN4;

    double *resultsFirstLayer;

    unsigned int nSize = 0;

    unsigned int epochCount = 0;
    Network();
};

#endif // NETWORK_H
