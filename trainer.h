#ifndef TRAINER_H
#define TRAINER_H

#include <QObject>
#include <QVector>
#include <QReadWriteLock>
#include "networksaveable.h"
#include "dataset.h"

class Trainer : public QObject
{
    Q_OBJECT
public:
    explicit Trainer(int width, int height, QObject *parent = 0);
    ~Trainer();
    QVector<NetworkSaveable *> networks;
    bool stop = false;
    QReadWriteLock lock;
    int id;
    double **errorsFromLayers;
    int width;
signals:
    void epochFinished(ulong currentEpoch,double commonError);
    void trainingFinished();
public slots:
    void stratTraining(double speed,int epochCount,double acceptableError,Dataset * dataset);
};

#endif // TRAINER_H
