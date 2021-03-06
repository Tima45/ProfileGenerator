#include "trainer.h"

Trainer::Trainer(int width,int height,QObject *parent) : QObject(parent)
{
    this->width = width;
    errorsFromLayers = new double*[width-1];
    for(unsigned int layerIndex = 0; layerIndex < width-1; layerIndex++){
        errorsFromLayers[layerIndex] = new double[height];
    }
}

Trainer::~Trainer()
{
    for(unsigned int layerIndex = 0; layerIndex < width-1; layerIndex++){
        delete[] errorsFromLayers[layerIndex];
    }
    delete[] errorsFromLayers;
}

void Trainer::stratTraining(double speed, int epochCount, double acceptableError, Dataset *dataset)
{
    if(!dataset->elements.isEmpty() && !networks.isEmpty()){
        lock.lockForWrite();
        stop = false;
        lock.unlock();

        lock.lockForRead();
        for(int currentEpoch = 0; (currentEpoch < epochCount) && (!stop); currentEpoch++){
            lock.unlock();
            double commonError = 0;
            for(int i = 0; i < networks.count(); i++){
                //if(fabs(networks.at(i)->lastError) > acceptableError){
                    networks.at(i)->lastError = 0;
                    for(int d = 0; d < dataset->elements.count(); d++){
                        double error = networks.at(i)->train(speed*dataset->elements.at(d)->takeToAccount,
                                              dataset->elements.at(d)->xyProfile.constData(),
                                              dataset->elements.at(d)->xyProfile.count(),
                                              dataset->elements.at(d)->pic[networks.at(i)->y][networks.at(i)->x],
                                              errorsFromLayers);
                        networks.at(i)->lastError += error;
                    }
                    networks.at(i)->lastError /= dataset->elements.count();
                    networks.at(i)->apllayResult();

                //}
                commonError += networks.at(i)->lastError;
                networks.at(i)->epochCount++;
            }
            emit epochFinished(networks.at(0)->epochCount,commonError/(networks.count()));
            lock.lockForRead();
        }
        lock.unlock();

    }else{
        qDebug() << "Trainer::stratTraining empty";
    }
    emit trainingFinished();
}
