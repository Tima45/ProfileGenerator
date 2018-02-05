#include "networksaveable.h"

NetworkSaveable::NetworkSaveable() : Network()
{

}

NetworkSaveable::NetworkSaveable(NetworkSaveable &other)
{
    x = other.x;
    y = other.y;
    lastError = other.lastError;
    epochCount = other.epochCount;

    nSize = other.nSize;

    lastN = new Neuron2i();

    middleN1 = new NeuronNi(4);
    middleN2 = new NeuronNi(4);

    firstN1 = new NeuronNi(nSize);
    firstN2 = new NeuronNi(nSize);
    firstN3 = new NeuronNi(nSize);
    firstN4 = new NeuronNi(nSize);

    resultsFirstLayer = new double[4];

    lastN->w1 = other.lastN->w1;
    lastN->w2 = other.lastN->w2;
    lastN->bias = other.lastN->bias;

    for(unsigned int i = 0; i < 4; i++){
        middleN1->weights[i] = other.middleN1->weights[i];
    }
    middleN1->bias = other.middleN1->bias;

    for(unsigned int i = 0; i < 4; i++){
        middleN2->weights[i] = other.middleN2->weights[i];
    }
    middleN2->bias = other.middleN2->bias;



    for(unsigned int i = 0; i < nSize; i++){
        firstN1->weights[i] = other.firstN1->weights[i];
    }
    firstN1->bias = other.firstN1->bias;

    for(unsigned int i = 0; i < nSize; i++){
        firstN2->weights[i] = other.firstN2->weights[i];
    }
    firstN2->bias = other.firstN2->bias;

    for(unsigned int i = 0; i < nSize; i++){
        firstN3->weights[i] = other.firstN3->weights[i];
    }
    firstN3->bias = other.firstN3->bias;

    for(unsigned int i = 0; i < nSize; i++){
        firstN4->weights[i] = other.firstN4->weights[i];
    }
    firstN4->bias = other.firstN4->bias;

}

NetworkSaveable::NetworkSaveable(unsigned int inputCount,int x, int y) : Network(inputCount), x(x), y(y)
{

}

NetworkSaveable::~NetworkSaveable()
{

}

void NetworkSaveable::save(QDataStream &s)
{
    s << nSize;
    s << epochCount;

    s << firstN1->inputCount;
    for(unsigned int i = 0; i < firstN1->inputCount; i++){
        s << firstN1->weights[i];
    }
    s << firstN1->bias;

    s << firstN2->inputCount;
    for(unsigned int i = 0; i < firstN2->inputCount; i++){
        s << firstN2->weights[i];
    }
    s << firstN2->bias;

    s << firstN3->inputCount;
    for(unsigned int i = 0; i < firstN3->inputCount; i++){
        s << firstN3->weights[i];
    }
    s << firstN3->bias;

    s << firstN4->inputCount;
    for(unsigned int i = 0; i < firstN4->inputCount; i++){
        s << firstN4->weights[i];
    }
    s << firstN4->bias;

    s << middleN1->inputCount;
    for(unsigned int i = 0; i < middleN1->inputCount; i++){
        s << middleN1->weights[i];
    }
    s << middleN1->bias;

    s << middleN2->inputCount;
    for(unsigned int i = 0; i < middleN2->inputCount; i++){
        s << middleN2->weights[i];
    }
    s << middleN2->bias;

    s << lastN->w1;
    s << lastN->w2;
    s << lastN->bias;

}

void NetworkSaveable::load(QDataStream &s)
{
    delete lastN;
    delete middleN1;
    delete middleN2;

    delete firstN1;
    delete firstN2;
    delete firstN3;
    delete firstN4;
    delete[] resultsFirstLayer;

    s >> nSize;
    s >> epochCount;

    lastN = new Neuron2i();

    middleN1 = new NeuronNi(4);
    middleN2 = new NeuronNi(4);

    firstN1 = new NeuronNi(nSize);
    firstN2 = new NeuronNi(nSize);
    firstN3 = new NeuronNi(nSize);
    firstN4 = new NeuronNi(nSize);

    resultsFirstLayer = new double[4];

    s >> firstN1->inputCount;
    for(unsigned int i = 0; i < firstN1->inputCount; i++){
        s >> firstN1->weights[i];
    }
    s >> firstN1->bias;

    s >> firstN2->inputCount;
    for(unsigned int i = 0; i < firstN2->inputCount; i++){
        s >> firstN2->weights[i];
    }
    s >> firstN2->bias;

    s >> firstN3->inputCount;
    for(unsigned int i = 0; i < firstN3->inputCount; i++){
        s >> firstN3->weights[i];
    }
    s >> firstN3->bias;

    s >> firstN4->inputCount;
    for(unsigned int i = 0; i < firstN4->inputCount; i++){
        s >> firstN4->weights[i];
    }
    s >> firstN4->bias;

    s >> middleN1->inputCount;
    for(unsigned int i = 0; i < middleN1->inputCount; i++){
        s >> middleN1->weights[i];
    }
    s >> middleN1->bias;

    s >> middleN2->inputCount;
    for(unsigned int i = 0; i < middleN2->inputCount; i++){
        s >> middleN2->weights[i];
    }
    s >> middleN2->bias;

    s >> lastN->w1;
    s >> lastN->w2;
    s >> lastN->bias;
}
