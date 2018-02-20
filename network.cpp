#include "network.h"

bool Network::isPowerOfTwo(int value)
{
    return (value & (value - 1)) == 0;
}

double Network::log4(double value)
{
    return log(value)/log(4);
}

Network::Network(unsigned int inputCount, unsigned int width, unsigned int height)
{
    this->inputCount = inputCount;
    this->width = width;
    this->height = height;

    lastN = new NeuronNi(height);

    layers = new NeuronNi**[width];
    resultsFromLayers = new double*[width];

    for(unsigned int layerIndex = 0; layerIndex < width-1; layerIndex++){
        resultsFromLayers[layerIndex] = new double[height];
        layers[layerIndex] = new NeuronNi*[height];
        for(unsigned int i = 0; i < height; i++){
            layers[layerIndex][i] = new NeuronNi(height);
        }
    }

    resultsFromLayers[width-1] = new double[height];

    layers[width-1] = new NeuronNi*[height];
    for(unsigned int i = 0; i < height; i++){
        layers[width-1][i] = new NeuronNi(inputCount);
    }
}

Network::~Network()
{
    delete lastN;

    for(unsigned int layerIndex = 0; layerIndex < width; layerIndex++){
        for(unsigned int i = 0; i < height; i++){
            delete layers[layerIndex][i];
        }
        delete[] layers[layerIndex];
        delete[] resultsFromLayers[layerIndex];
    }
    delete[] layers;
    delete[] resultsFromLayers;


}

double Network::work(const double *inputs, unsigned int size)
{
    double result = -1;
    if(inputCount == size){

        for(unsigned int i = 0; i < height;i++){
            resultsFromLayers[width-1][i] = layers[width-1][i]->work(inputs,size);
        }
        for(int layerIndex = width-2; layerIndex >= 0; layerIndex--){
            for(unsigned int i = 0; i < height; i++){
                resultsFromLayers[layerIndex][i] = layers[layerIndex][i]->work(resultsFromLayers[layerIndex+1],height);
            }
        }
        result = lastN->work(resultsFromLayers[0],height);
        lastResult = result;
    }
    return result;
}

double Network::train(double speed, const double *inputs, unsigned int size, double expectedValue, double **errorsFromLayers)
{
    double commonError = 0;
    if(inputCount == size){
        double result = this->work(inputs,size);
        commonError = expectedValue - result;

        double correctionForLastLayer = lastN->calculateCorrection(speed,commonError,resultsFromLayers[0],height);

        for(unsigned int i = 0; i < height; i++){
            errorsFromLayers[0][i] = layers[0][i]->calculateCorrection(speed,correctionForLastLayer,resultsFromLayers[1],height);
        }

        for(unsigned int layerIndex = 1; layerIndex < width-1; layerIndex++){
            for(unsigned int i = 0; i < height; i++){
                errorsFromLayers[layerIndex][i] = 0;
            }
            for(unsigned int i = 0; i < height; i++){
                for(unsigned int j = 0; j < height; j++){
                    errorsFromLayers[layerIndex][i] += layers[layerIndex][i]->calculateCorrection(speed,errorsFromLayers[layerIndex-1][j],resultsFromLayers[layerIndex+1],height);
                }
            }
        }

        for(unsigned int i = 0; i < height; i++){
            for(unsigned int j = 0; j < height; j++){
                layers[width-1][i]->calculateCorrection(speed,errorsFromLayers[width-2][j],inputs,size);
            }
        }
    }
    //apllayResult();
    return commonError;
}

void Network::trainCsv(double speed, const double *inputs, unsigned int size, double error, double **errorsFromLayers)
{
    error = (error > 0)?(1.0-lastResult)*error: lastResult*error;
    double correctionForLastLayer = lastN->calculateCorrection(speed,error,resultsFromLayers[0],height);

    for(unsigned int i = 0; i < height; i++){
        errorsFromLayers[0][i] = layers[0][i]->calculateCorrection(speed,correctionForLastLayer,resultsFromLayers[1],height);
    }

    for(unsigned int layerIndex = 1; layerIndex < width-1; layerIndex++){
        for(unsigned int i = 0; i < height; i++){
            errorsFromLayers[layerIndex][i] = 0;
        }
        for(unsigned int i = 0; i < height; i++){
            for(unsigned int j = 0; j < height; j++){
                errorsFromLayers[layerIndex][i] += layers[layerIndex][i]->calculateCorrection(speed,errorsFromLayers[layerIndex-1][j],resultsFromLayers[layerIndex+1],height);
            }
        }
    }

    for(unsigned int i = 0; i < height; i++){
        for(unsigned int j = 0; j < height; j++){
            layers[width-1][i]->calculateCorrection(speed,errorsFromLayers[width-2][j],inputs,size);
        }
    }
}

void Network::apllayResult()
{
    lastN->updateWeights();

    for(unsigned int layerIndex = 0; layerIndex < width-1; layerIndex++){
        for(unsigned int i = 0; i < height; i++){
            layers[layerIndex][i]->updateWeights();
        }
    }
}

Network::Network()
{
    this->inputCount = 0;
    this->width = 0;
    this->height = 0;
}
