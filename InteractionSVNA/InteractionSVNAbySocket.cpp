#include "InteractionSVNAbySocket.h"


InteractionSVNAbySocket::InteractionSVNAbySocket(QObject *parent =nullptr):InteractionSVNA(parent)
{
    _commSVNA = new Socket(this);
    ConnectSlotsSignals();
}

QString InteractionSVNAbySocket::SVNAID()
{
    return SendWithResponse(QList<QString>()<<"*IDN?");
}

void InteractionSVNAbySocket::ResetSVNA()
{
    Send(QList<QString>()<<"SYST:PRES\n");
}

void InteractionSVNAbySocket::TrigSing()
{
    SendWithResponse(QList<QString>()
         << "TRIG:SING\n"
         << "*OPC?\n");
}

void InteractionSVNAbySocket::SetSource()
{
    Send(QList<QString>()<<"TRIG:SOUR BUS\n");
}

QVector<double> InteractionSVNAbySocket::GetVectorAmplitude(int tr)
{
    QString dataString = SendWithResponse(QList<QString>()<<"CALC1:TRAC"+QString::number(tr)+":DATA:FDAT?\n");
    QVector<double> amplVector = Converters::ConvertStringToTVector<double>(dataString,',');
    return amplVector;
}

QVector<double> InteractionSVNAbySocket::GetVectorFreq()
{
   QString dataString = SendWithResponse(QList<QString>()<<"SENS1:FREQ:DATA?\n");
   QVector<double> freqVector = Converters::ConvertStringToTVector<double>(dataString,',');
   return freqVector;
}



void InteractionSVNAbySocket::SetParam(int Tr,int Port,const QString S)
{
    Send(QList<QString>()<<"CALC1:PAR"+ QString::number(Tr) +":DEF " + S +"\n");
      if(Port!=-1)
    Send(QList<QString>()<<"CALC1:PAR"+ QString::number(Tr) +":SPOR "+QString::number(Port)+"\n");
}

void InteractionSVNAbySocket::SetFormat(int Tr, const QString Form)
{
    Send(QList<QString>()<<"CALC1:TRAC"+ QString::number(Tr) +":FORM " + Form +"\n");
}

void InteractionSVNAbySocket::SetStartFreq(qint64 freq)
{
    Send(QList<QString>()<<"SENS1:FREQ:STAR " + QString::number(freq)+"\n");
}

void InteractionSVNAbySocket::SetStopFreq(qint64 freq)
{
    Send(QList<QString>()<<"SENS1:FREQ:STOP " + QString::number(freq)+"\n");
}

void InteractionSVNAbySocket::SetCntPoints(qint64 points)
{
    Send(QList<QString>()<<"SENS1:SWE:POIN "+ QString::number(points)+"\n");
}

void InteractionSVNAbySocket::SetBandFreq(qint64 freq)
{
    Send(QList<QString>()<<"SENS1:BAND "+ QString::number(freq)+"\n");
}

void InteractionSVNAbySocket::SetGraphsCount(int tr)
{
    Send(QList<QString>()<<"CALC1:PAR:COUN "+ QString::number(tr)+"\n");
}

void InteractionSVNAbySocket::SetRFState(bool state)
{
    QString strstate = state ? "ON" : "OFF";
    Send(QList<QString>()<<"OUTP "+ strstate +"\n");
}


QString InteractionSVNAbySocket::SendWithResponse(const QList<QString>& cmds, int timeout)
{
    QString resp = (*_commSVNA).cmd(cmds,timeout);
    if(*(resp.end()-1)=='\n')
        resp.remove(resp.size()-1,1);
    return resp;
}

void InteractionSVNAbySocket::Send(const QList<QString>& cmds)
{
   (*_commSVNA).cmd(cmds);
}
