#ifndef SOCKET_H
#define SOCKET_H

#include "CommunicationSVNA.h"

#include <QObject>
#include <QTcpSocket>
#include <QQueue>
#include <QList>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QObject>
#include <QWidget>
#include <QThread>

enum ConnectStatus
{
    csTcpOff,
    csTcpOnDevOff,
    csTcpOnDevOn,
};
struct DevInfo
{
    QString hostName;
    quint16 tcpPort;
    QString idn;
    QString name;
    QString serial;
    ConnectStatus status = csTcpOff;
};

class Socket : public CommunicationSVNA
{
    Q_OBJECT

public:
    Socket( QObject *parent = Q_NULLPTR );
    Socket( const QString &hostName, quint16 port, QObject *parent = Q_NULLPTR );
    ~Socket();

    void setHostName(const QString &hostName);
    void setPort(quint16 port);

    void setNameDevice(int name) {_nameDevice = name;}
    int getNameDevice(){return _nameDevice;}

    DevInfo getDevInfo() const;
    ConnectStatus getStatus();

    QString cmd(const QList<QString> &cmds, int timeOutMSec = 0 );


public slots:
    void connect();
    void connect(const QString &hostName, quint16 port );

    // запускает конвейер обработки


private slots:
    void checkDevConnect();
    void readData();
    void interceptError(QAbstractSocket::SocketError socketError);
    void stateChanged(QAbstractSocket::SocketState state);

signals:
//    void error(const QString &err);
    void pipelineFinishing();
    void updateStatus();
//    void sigDeviceConnected(bool, int, QString);

private:
    void run( const QString &cmd );
    bool nextPipeline();

    QTimer*     _timer;
    QTcpSocket* _socket;

    DevInfo        _devInfo;
    QString        _readBuf;
    QList<QString> _qCmds; // очередь команд

    int            _nameDevice;

    bool _run;

    static int _portIterator; // для автоматической нумерации портов при создании объектов
};

#endif // SOCKET_H
