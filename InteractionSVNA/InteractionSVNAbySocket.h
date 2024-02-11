#include"InteractionSVNA/interactionsvna.h"
#include"CommunicationSVNA/socket.h"

#ifndef INTERACTIONSVNABYSOCKET_H
#define INTERACTIONSVNABYSOCKET_H

#include<QString>
#include<QVector>
#include"Converters.h"

class InteractionSVNAbySocket: public InteractionSVNA
{
public:
    InteractionSVNAbySocket(QObject *parent);

    QString SVNAID() override;
    void ResetSVNA() override;
    void TrigSing() override;
    void SetSource() override;
    QVector<double> GetVectorAmplitude(int tr) override;
    QVector<double> GetVectorFreq() override;
    void SetParam(int Tr, int Port, const QString param) override;
    void SetFormat(int, const QString) override;
    void SetStartFreq(qint64) override;
    void SetStopFreq(qint64) override;
    void SetCntPoints(qint64) override;
    void SetBandFreq(qint64) override;
    void SetGraphsCount(int) override;
    void SetRFState(bool) override;

private:
    QString SendWithResponse(const QList<QString>&cmds, int timeout=0) override;
    void Send(const QList<QString>&cmd) override;
};

#endif // INTERACTIONSVNABYSOCKET_H
