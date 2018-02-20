#ifndef DATASET_H
#define DATASET_H

#include <QObject>
#include <QtCore>
#include <QVector>
#include <QDir>
#include <QFileInfo>
#include <cv.hpp>
#include "datasetelement.h"
#include "profiledata.h"

using namespace cv;

class Dataset : public QObject
{
    Q_OBJECT
public:
    Dataset(QObject *parent = 0);
    QString datasetType;
    QVector<DatasetElement*> elements;
    ~Dataset();

signals:

public slots:
    void load(QString path);
    void clear();
};

#endif // DATASET_H
