#include "profilegenerator.h"

ProfileGenerator::ProfileGenerator(unsigned int profileSize, QObject *parent) : QObject(parent), profileSize(profileSize)
{
    if(Network::isPowerOfTwo(profileSize)){

        for(int i = 0;i < trainersCount;i++){
            Trainer *trainer = new Trainer(3,10);
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
        /*
        double maxAbs = 0;
        for(int i = 0; i < xyProfile.count(); i++){
            if(fabs(xyProfile.at(i)) > maxAbs){
                maxAbs = fabs(xyProfile.at(i));
            }
        }
        for(int i = 0; i < xyProfile.count(); i++){
            xyProfile[i] /= maxAbs;
        }*/

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
        if(dataset->datasetType == "pic"){
            readyTrainers = 0;
            emit stratTrainers(speed,epochCount,acceptableError,dataset);
        }else if(dataset->datasetType == "csv"){
            lock.lockForWrite();
            stop = false;
            lock.unlock();


            QVector<double> xyDiffs;
            for(int i = 0; i < 256; i++){
                xyDiffs.append(0);
            }
            QVector<double> xyProf;
            for(int i = 0; i < 256; i++){
                xyProf.append(0);
            }

            lock.lockForRead();
            for(int i = 0; (i < epochCount) && (!stop);i++){
                lock.unlock();


                for(int dataSetIndex = 0; dataSetIndex < dataset->elements.count(); dataSetIndex++){
                    for(int i = 0; i < 256; i++){
                        xyProf[i] = 0;
                    }
                    Mat newPic(profileSize,profileSize,CV_8UC1);
                    for(unsigned int y = 0; y < profileSize; y++){
                        for(unsigned int x = 0; x < profileSize; x++){
                            double pixelValue = networks.at(y).at(x)->work(dataset->elements.at(dataSetIndex)->xyProfile.constData(),dataset->elements.at(dataSetIndex)->xyProfile.count());
                            newPic.at<uchar>(y,x) = (uchar)(pixelValue*255);
                            xyProf[x] += pixelValue/128.0;
                            xyProf[128+y] += pixelValue/128.0;
                        }
                    }

                    /*
                    double max = 0;
                    for(int i = 0; i < 256; i++){
                        if(fabs(xyProf[i]) > max){
                            max = xyProf[i];
                        }
                    }

                    for(int i = 0; i < 256; i++){
                        xyProf[i] /= max;
                    }*/


                    imshow("CsvTraining",newPic);
                    emit showProfiles(dataset->elements.at(dataSetIndex)->xyProfile,xyProf);



                    for(int i = 0; i < 256; i++){
                        xyDiffs[i] = dataset->elements.at(dataSetIndex)->xyProfile[i] - xyProf[i];
                    }
                    for(int i = 0; i < 128; i++){
                        for(unsigned int y = 0; y < profileSize; y++){
                            networks.at(y).at(i)->trainCsv(speed,dataset->elements.at(dataSetIndex)->xyProfile.constData(),dataset->elements.at(dataSetIndex)->xyProfile.count(),xyDiffs[i]/128.0,trainers.at(0)->errorsFromLayers);
                        }
                    }

                    for(int i = 128; i < 256; i++){
                        for(unsigned int x = 0; x < profileSize; x++){
                            networks.at(i-128).at(x)->trainCsv(speed,dataset->elements.at(dataSetIndex)->xyProfile.constData(),dataset->elements.at(dataSetIndex)->xyProfile.count(),xyDiffs[i]/128.0,trainers.at(0)->errorsFromLayers);
                        }
                    }
                }

                for(unsigned int y = 0; y < profileSize; y++){
                    for(unsigned int x = 0; x < profileSize; x++){
                        networks.at(y).at(x)->apllayResult();
                        networks.at(y).at(x)->epochCount++;
                    }
                }

                emit csvEpochFinished(networks.at(0).at(0)->epochCount);
                lock.lockForRead();
            }
            lock.unlock();

            emit trainingFinished();
        }
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
    lock.lockForWrite();
    stop = true;
    lock.unlock();

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
    Trainer *t = (Trainer*)sender();
    emit epochFinished(currentEpoch,commonError,t->id);
}
