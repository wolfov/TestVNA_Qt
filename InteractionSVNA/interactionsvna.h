#ifndef INTERACTIONSVNA_H
#define INTERACTIONSVNA_H
#include "CommunicationSVNA/CommunicationSVNA.h"
#include "appsettings.h"
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QtGlobal>


class InteractionSVNA: public QObject
{
    Q_OBJECT
public:
//    explicit InteractionSVNA(CommunicationSVNA &commSVNA,QObject *parent = nullptr);
    explicit InteractionSVNA(QObject *parent = nullptr):QObject(parent){}
    ~InteractionSVNA(){}

    //void CheckThreadID(){qInfo()<<"InteractionSVNA thread: "<<QThread::currentThreadId();}
    void Connect();
    void PrepairToWork();

    virtual QString SVNAID()=0;                     //*IDN?
    virtual void ResetSVNA()=0;                     //*RST
    virtual void TrigSing()=0;                      //*TRG \ *OPC?
    virtual void SetSource()=0;                     //TRIG:SOUR
    virtual QVector<double> GetVectorAmplitude(int tr) =0;//CALC:DATA:FDAT?
    virtual QVector<double> GetVectorFreq() =0;     //SENS:FREQ:DATA?
    virtual void SetParam(int tr, int port, const QString param)=0;   //CALC:PAR:DEF \ CALC:PAR:SPOR
    virtual void SetFormat(int tr, const QString format)=0;  //CALC:FORM
    virtual void SetStartFreq(qint64)=0;            //SENS:FREQ:STAR
    virtual void SetStopFreq(qint64)=0;             //SENS:FREQ:STOP
    virtual void SetCntPoints(qint64)=0;            //SENS:SWE:POIN
    virtual void SetBandFreq(qint64)=0;             //SENS:BAND
    virtual void SetGraphsCount(int tr)=0;          //CALC:PAR:COUN
    virtual void SetRFState(bool)=0;                //OUTP

signals:
   void sgnl_connect();
   void sgnl_error( const QString &err );

protected:
   CommunicationSVNA *_commSVNA;
   void ConnectSlotsSignals();

private slots:
   void slt_DeviceConnected(bool, int, QString);

private:
   virtual QString SendWithResponse(const QList<QString>&, int timeout=0)=0;
   virtual void Send(const QList<QString>&)=0;
};

#endif // INTERACTIONSVNA_H
