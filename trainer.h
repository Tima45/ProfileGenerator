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
    explicit Trainer(QObject *parent = 0);
    QVector<NetworkSaveable *> networks;
    bool stop = false;
    QReadWriteLock lock;
    int id;
signals:
    void epochFinished(ulong currentEpoch,double commonError);
    void trainingFinished();
public slots:
    void stratTraining(double speed,int epochCount,double acceptableError,Dataset * dataset);
};

#endif // TRAINER_H
