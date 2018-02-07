#ifndef NEURONNI_H
#define NEURONNI_H

#include "neuron.h"

class NeuronNi : public Neuron
{
public:
    explicit NeuronNi(unsigned int inputCount);
    NeuronNi(const NeuronNi &other);
    double work(const double *inputs,unsigned int inputCount);
    double calculateCorrection(double speed,double error,const double *inputs,unsigned int inputCount);
    void updateWeights();
    ~NeuronNi();


    unsigned int inputCount;
    double *weights;
    double *deltaWeights;
    double bias = 0;
    double deltaBias = 0;
private:

};

#endif // NEURONNI_H
