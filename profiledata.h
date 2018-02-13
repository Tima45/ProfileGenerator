#ifndef PROFILEDATA_H
#define PROFILEDATA_H

#include <QObject>
#include <QtCore>
#include <QMessageBox>
#include <QRegExp>
#include <waveletspectrum.h>

class ProfileData : public QObject
{
    Q_OBJECT
public:
    ProfileData(QObject *parent = 0);
    ProfileData(QString inFileName,QObject *parent);
    ~ProfileData();
    bool errors;
    QString fileName;
    QColor color;



    QVector<double> xyProfile;

private:
    //------------------------------------
    QVector<int> borders;
    QVector<double> rawAmpers;
    QVector<double> smoothAmpers;
    QVector<double> rawAngles;


    QVector<double> xProfile;
    QVector<double> yProfile;



    QString scanDate;

    void readData(QString fileName);
    void findBorders();
    void loadFrames();
    void resize(QVector<double> &vector,int size);
    QVector<double> denoiseAngles(QVector<double> angles);
};

#endif // PROFILEDATA_H
