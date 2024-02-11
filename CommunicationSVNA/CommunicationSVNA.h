#ifndef COMMUNICATIONSVNA_H
#define COMMUNICATIONSVNA_H

#include<QString>
#include<QList>
#include <QObject>

class CommunicationSVNA : public QObject
{
    Q_OBJECT

public:
    CommunicationSVNA( QObject *parent = nullptr ):QObject(parent){}
    virtual QString cmd(const QList<QString> &cmds, int timeOutMSec = 0 ) = 0;

public slots:
    virtual void connect() = 0;

signals:
    void error(const QString &err);
    void sigDeviceConnected(bool, int, QString);
};

#endif // COMMUNICATIONSVNA_H
