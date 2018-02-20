#ifndef NETWORK_H
#define NETWORK_H

#include <math.h>
#include "neuron2i.h"
#include "neuronni.h"


class Network
{
public:
    static bool isPowerOfTwo(int value);
    static double log4(double value);
    explicit Network(unsigned int inputCount,unsigned int width, unsigned int height);
    virtual ~Network();
    double work(const double *inputs,unsigned int size);
    double train(double speed, const double *inputs, unsigned int size, double expectedValue, double **errorsFromLayers); // returns error
    void trainCsv(double speed, const double *inputs, unsigned int size, double error,double **errorsFromLayers);
    void apllayResult();

    NeuronNi ***layers;
    NeuronNi *lastN;
    double **resultsFromLayers;


    unsigned int inputCount = 0;
    unsigned int width = 0;
    unsigned int height = 0;

    unsigned int epochCount = 0;
    Network();

    double lastResult = 0;

};

#endif // NETWORK_H
