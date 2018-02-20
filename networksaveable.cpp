#include "networksaveable.h"

NetworkSaveable::NetworkSaveable() : Network()
{

}

NetworkSaveable::NetworkSaveable(const NetworkSaveable &other)
{
    lastError = other.lastError;
    epochCount = other.epochCount;
    inputCount = other.inputCount;
    width = other.width;
    height = other.height;

    lastN = new NeuronNi(*other.lastN);

    layers = new NeuronNi**[width];
    resultsFromLayers = new double*[width];

    for(unsigned int layerIndex = 0; layerIndex < width-1; layerIndex++){
        resultsFromLayers[layerIndex] = new double[height];
        layers[layerIndex] = new NeuronNi*[height];
        for(unsigned int i = 0; i < height; i++){
            layers[layerIndex][i] = new NeuronNi(*other.layers[layerIndex][i]);
        }
    }

    resultsFromLayers[width-1] = new double[height];


    layers[width-1] = new NeuronNi*[height];
    for(unsigned int i = 0; i < height; i++){
        layers[width-1][i] = new NeuronNi(*other.layers[width-1][i]);
    }

}

NetworkSaveable::NetworkSaveable(unsigned int inputCount, int x, int y, int width, int height) : Network(inputCount,width,height), x(x), y(y)
{

}

NetworkSaveable::~NetworkSaveable()
{

}

void NetworkSaveable::save(QDataStream &s)
{
    s << inputCount;
    s << width;
    s << height;
    s << epochCount;
    s << x;
    s << y;
    s << lastError;



    for(unsigned int i = 0; i < height; i++){
        s << lastN->weights[i];
    }
    s << lastN->bias;

    for(unsigned int layerIndex = 0; layerIndex < width; layerIndex++){
        for(unsigned int i = 0; i < height; i++){
            for(unsigned int j = 0; j < layers[layerIndex][i]->inputCount; j++){
                s << layers[layerIndex][i]->weights[j];
            }
            s << layers[layerIndex][i]->bias;
        }
    }

}

void NetworkSaveable::load(QDataStream &s)
{

    s >> inputCount;
    s >> width;
    s >> height;
    s >> epochCount;
    s >> x;
    s >> y;
    s >> lastError;


    for(unsigned int i = 0; i < height; i++){
        s >> lastN->weights[i];
    }
    s >> lastN->bias;


    for(unsigned int layerIndex = 0; layerIndex < width-1; layerIndex++){
        for(unsigned int i = 0; i < height; i++){
            for(unsigned int j = 0; j < layers[layerIndex][i]->inputCount; j++){
                s >> layers[layerIndex][i]->weights[j];
            }
            s >> layers[layerIndex][i]->bias;
        }
    }

    for(unsigned int i = 0; i < height; i++){
        for(unsigned int j = 0; j < layers[width-1][i]->inputCount; j++){
            s >> layers[width-1][i]->weights[j];
        }
        s >> layers[width-1][i]->bias;
    }
}
