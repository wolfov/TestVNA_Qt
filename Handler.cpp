#include "Handler.h"

Handler::Handler(QObject *parent):QObject(parent)
{
    _timer = new QTimer(this);
    _timer->setInterval(500);
    _timer->setSingleShot(true);
    _IntrSVNAbySckt = new InteractionSVNAbySocket(this);
    connect(_timer,SIGNAL(timeout()),
            this,SLOT(timerIteration()));
    connect(_IntrSVNAbySckt,&InteractionSVNA::sgnl_error,
            this,&Handler::sgnl_error);
    _IntrSVNAbySckt->Connect();
}

void Handler::slt_StartStop()
{
    isWork=!isWork;
    emit sgnl_WorkStatus(isWork);

    if(isWork)
        _timer->start();
    else
        _timer->stop();
}

void Handler::slt_RFStateChanged()
{
    RFState = !RFState;
    _IntrSVNAbySckt->SetRFState(RFState);
    emit sgnl_RFState(RFState);
}

void Handler::slt_StartFreqChanged(quint64 startfreq)
{
    _IntrSVNAbySckt->SetStartFreq(startfreq);
}

void Handler::slt_StopFreqChanged(quint64 stopfreq)
{
    _IntrSVNAbySckt->SetStopFreq(stopfreq);
}

void Handler::slt_CntPointsChanged(quint64 cntpoints)
{
    _IntrSVNAbySckt->SetCntPoints(cntpoints);
}

void Handler::slt_MeasFilterBandChanged(quint64 bandfreq)
{
    _IntrSVNAbySckt->SetBandFreq(bandfreq);
}

void Handler::slt_MeasParamChanged(int tr, int port, const QString& param)
{
    _IntrSVNAbySckt->SetParam(tr,port,param);
}

void Handler::slt_MeasFormatChanged(int tr ,const QString& Format)
{
    _IntrSVNAbySckt->SetFormat(tr, Format);
}

void Handler::slt_CntTraceChanged(int tr)
{
    _IntrSVNAbySckt->SetGraphsCount(tr);
}

void Handler::timerIteration()
{
    _IntrSVNAbySckt->TrigSing();

    QVector<double> freqVec = _IntrSVNAbySckt->GetVectorFreq();
    for(int i =1;i<= AppSettings::Graphs.count();i++)
    {
        QVector<QPointF> points(freqVec.size());
        QVector<double> amplVec = _IntrSVNAbySckt->GetVectorAmplitude(i);

        if(freqVec.size()*2==amplVec.size())
        {
            for(int ii = 0 ; ii<freqVec.size() ; ++ii)
                points[ii] = QPointF(freqVec[ii],amplVec[ii*2]);
            emit sgnl_XYpoints(i,points);
        }
    }

    if(isWork)
        _timer->start();
}
