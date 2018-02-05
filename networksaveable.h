#ifndef NETWORKSAVEABLE_H
#define NETWORKSAVEABLE_H

#include "network.h"
#include <QDataStream>

class NetworkSaveable : public Network
{
public:
    NetworkSaveable();
    NetworkSaveable(NetworkSaveable &other);
    NetworkSaveable(unsigned int inputCount,int x, int y);
    ~NetworkSaveable();
    void save(QDataStream &s);
    void load(QDataStream &s);
    int x;
    int y;
    double lastError = 42;



};

#endif // NETWORKSAVEABLE_H
