#ifndef HANDLER_H
#define HANDLER_H

#include"InteractionSVNA/interactionsvna.h"
#include"InteractionSVNA/InteractionSVNAbySocket.h"
#include "Device.h"
#include <QObject>
#include <QString>
#include <QtGlobal>
#include <QtDebug>
#include <QVector>
#include <QPointF>
#include <QtMath>
#include <QThread>
#include <QTimer>

class Handler : public QObject
{
    Q_OBJECT
public:
    explicit Handler(QObject *parent = nullptr);
    bool isWork = false;
    bool RFState = true;

signals:
    void sgnl_WorkStatus(bool);
    void sgnl_RFState(bool);
    void sgnl_XYpoints(int tr, QVector<QPointF> points);
    void sgnl_error(const QString &err);


public slots:
    void slt_StartFreqChanged(quint64 freq);
    void slt_StopFreqChanged(quint64 freq);
    void slt_CntPointsChanged(quint64 points);
    void slt_MeasFilterBandChanged(quint64 freq);
    void slt_MeasParamChanged(int tr, int port, const QString& param);
    void slt_MeasFormatChanged(int tr, const QString& format);
    void slt_CntTraceChanged(int tr);
    void slt_RFStateChanged();
    void slt_StartStop();

private slots:
    void timerIteration();

private:
    QTimer* _timer;
    InteractionSVNA *_IntrSVNAbySckt;
};

#endif // HANDLER_H
