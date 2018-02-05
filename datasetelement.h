#ifndef DATASETELEMENT_H
#define DATASETELEMENT_H

#include <QVector>

class DatasetElement
{
public:
    DatasetElement();
    QVector<double> xyProfile;
    QVector<QVector<double>> pic;
    double takeToAccount = 1;

};

#endif // DATASETELEMENT_H
