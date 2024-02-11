#ifndef DEVICE_H
#define DEVICE_H
#include <QtGlobal>
#include <QString>
#include <QVector>

struct Device
{
    double stepfreqHz;
    double maxFreqMeg;
    double minFreqMeg;
    quint64 maxPoints;
    quint64 minPoints;
    quint64 maxMeasFilterBand;
    quint64 minMeasFilterBand;
    //QVector<QString> measS;
};

#endif // DEVICE_H
