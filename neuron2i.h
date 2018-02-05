#ifndef NEURON2I_H
#define NEURON2I_H

#include "neuron.h"


class Neuron2i : public Neuron
{
public:
    Neuron2i();
    ~Neuron2i();
    double work(double input1,double input2);
    double calculateCorrection(double speed,double error,double input1,double input2);
    void updateWeights();


    double w1;
    double w2;
    double bias;


    double deltaW1 = 0;
    double deltaW2 = 0;
    double deltaBias = 0;


private:


};

#endif // NEURON2I_H
