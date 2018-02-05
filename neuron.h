#ifndef NEURON_H
#define NEURON_H

#include <math.h>
#include <stdlib.h>

class Neuron
{
public:
    static double sigmoid(double value);
    static double devSigmoid(double value);
    Neuron();
    virtual ~Neuron();
};

#endif // NEURON_H
