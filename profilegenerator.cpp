#include "profilegenerator.h"

ProfileGenerator::ProfileGenerator(unsigned int profileSize, QObject *parent) : QObject(parent), profileSize(profileSize)
{
    if(Network::isPowerOfTwo(profileSize)){

        for(int i = 0;i < trainersCount;i++){
            Trainer *trainer = new Trainer;
            trainer->id = i;
            QThread *thread = new QThread(this);

            trainer->moveToThread(thread);

            connect(this,SIGNAL(stratTrainers(double,int,double,Dataset*)),trainer,SLOT(stratTraining(double,int,double,Dataset*)),Qt::QueuedConnection);
            connect(trainer,SIGNAL(trainingFinished()),this,SLOT(trainerStopped()),Qt::QueuedConnection);
            connect(trainer,SIGNAL(epochFinished(ulong,double)),this,SLOT(someTrainerEpochFinished(ulong,double)));
            trainers.append(trainer);
            threads.append(thread);
        }



        int trainerIndex = 0;
        unsigned int counter = 0;

        NetworkSaveable *tamplateNetwork = new NetworkSaveable(profileSize*2,0,0,3,10);

        for(unsigned int y = 0; y < profileSize; y++){
            networks.append(QList<NetworkSaveable*>());
            for(unsigned int x = 0; x < profileSize; x++){
                NetworkSaveable *n = new NetworkSaveable(*tamplateNetwork);

                n->x = x;
                n->y = y;
                networks.last().append(n);
                trainers.at(trainerIndex)->networks.append(n);
                counter++;
                if(counter == (profileSize*profileSize)/trainersCount){
                    trainerIndex++;
                    counter = 0;
                }
            }
        }

        delete tamplateNetwork;

        for(int i = 0;i < trainersCount;i++){
            threads.at(i)->start();
        }
        threads.at(0)->start();

    }else{
        qDebug() << "ProfileGenerator::ProfileGenerator profileSize not power of 2";
    }
}

ProfileGenerator::~ProfileGenerator()
{
    for(int i = 0; i < threads.count(); i++){
        threads.at(i)->terminate();
    }
    for(int i = 0; i < trainers.count(); i++){
        trainers.at(i)->deleteLater();
    }
    for(unsigned int y = 0; y < profileSize; y++){
        for(unsigned int x = 0; x < profileSize; x++){
            delete networks[y][x];
        }
    }
}

Mat ProfileGenerator::generateProfile(QVector<double> xyProfile)
{
    Mat newPic(profileSize,profileSize,CV_8UC1);
    if(xyProfile.count() == 2*(int)profileSize){
        double maxAbs = 0;
        for(int i = 0; i < xyProfile.count(); i++){
            if(fabs(xyProfile.at(i)) > maxAbs){
                maxAbs = fabs(xyProfile.at(i));
            }
        }
        for(int i = 0; i < xyProfile.count(); i++){
            xyProfile[i] /= maxAbs;
        }

        for(unsigned int y = 0; y < profileSize; y++){
            for(unsigned int x = 0; x < profileSize; x++){
                double pixelValue = networks.at(y).at(x)->work(xyProfile.constData(),xyProfile.count());
                newPic.at<uchar>(y,x) = (uchar)(pixelValue*255);
            }
        }
    }else{
        qDebug() << "ProfileGenerator::generateProfile != profileSize";
    }
    return newPic;
}

void ProfileGenerator::save(QString path)
{
    QFile f(path);
    if(f.open(QIODevice::WriteOnly)){
        QDataStream s(&f);

        s << profileSize;
        for(unsigned int y = 0; y < profileSize; y++){
            for(unsigned int x = 0; x < profileSize; x++){
                networks.at(y).at(x)->save(s);
            }
        }

        f.close();
    }else{
        qDebug() << "wrong file";
    }
}

void ProfileGenerator::load(QString path)
{
    QFile f(path);
    if(f.open(QIODevice::ReadOnly)){
        QDataStream s(&f);

        s >> profileSize;
        unsigned int counter = 0;
        int trainerIndex = 0;

        for(int i = 0; i < trainersCount; i++){
            trainers.at(i)->networks.clear();
        }


        for(unsigned int y = 0; y < profileSize; y++){
            for(unsigned int x = 0; x < profileSize; x++){
                networks.at(y).at(x)->load(s);

                trainers.at(trainerIndex)->networks.append(networks.at(y).at(x));
                counter++;
                if(counter == (profileSize*profileSize)/trainersCount){
                    trainerIndex++;
                    counter = 0;
                }
            }
        }

        f.close();




    }else{
        qDebug() << "wrong file";
    }
}

void ProfileGenerator::startTraining(double speed, int epochCount, double acceptableError, Dataset *dataset)
{
    if(!dataset->elements.isEmpty()){
        readyTrainers = 0;
        emit stratTrainers(speed,epochCount,acceptableError,dataset);
    }else{
        qDebug() << "ProfileGenerator::startTraining dataset epmty";
        emit trainingFinished();
    }
}

void ProfileGenerator::interapt()
{
    for(int i = 0; i < trainers.count(); i++){
        lock.lockForWrite();
        trainers[i]->stop = true;
        lock.unlock();
    }
}

void ProfileGenerator::trainerStopped()
{
    readyTrainers++;
    if(readyTrainers == trainersCount){
        emit trainingFinished();
    }
}

void ProfileGenerator::someTrainerEpochFinished(ulong currentEpoch, double commonError)
{
    Trainer*t = (Trainer*)sender();
    emit epochFinished(currentEpoch,commonError,t->id);
}
