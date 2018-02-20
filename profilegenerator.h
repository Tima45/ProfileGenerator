#ifndef PROFILEGENERATOR_H
#define PROFILEGENERATOR_H

#include <QObject>
#include <QtCore>
#include <QThread>
#include <QMessageBox>
#include <QReadWriteLock>
#include <QDebug>
#include <cv.hpp>
#include "networksaveable.h"
#include "dataset.h"
#include "trainer.h"

using namespace cv;

class ProfileGenerator : public QObject
{
    Q_OBJECT
public:
    ProfileGenerator(unsigned int profileSize,QObject *parent = 0);
    ~ProfileGenerator();
    Mat generateProfile(QVector<double> xyProfile);
    int trainersCount = 4;
    QList<QList<NetworkSaveable*>> networks;
    unsigned int profileSize;
private:



    QVector<Trainer*> trainers;
    QVector<QThread*> threads;
    QReadWriteLock lock;
    bool stop;


    char readyTrainers = 0;
signals:
    //-------------------------
    void epochFinished(ulong currentEpoch,double commonError,int index);
    void trainingFinished();
    void showProfiles(QVector<double> expectedProf,QVector<double> realProf);
    void csvEpochFinished(int value);
    //=======================
    void stratTrainers(double speed, int epochCount,double acceptableError,Dataset *dataset);
    void startCsvTrain(double speed, int epochCount,Dataset *dataset);


public slots:
    void save(QString path);
    void load(QString path);
    void startTraining(double speed, int epochCount,double acceptableError,Dataset *dataset);
    void interapt();
    //=======================

    void trainerStopped();
    void someTrainerEpochFinished(ulong currentEpoch,double commonError);


};

#endif // PROFILEGENERATOR_H
