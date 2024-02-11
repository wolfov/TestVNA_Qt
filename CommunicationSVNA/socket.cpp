#include "Socket.h"

int Socket::_portIterator = 0;

Socket::Socket(QObject *parent) : Socket( "127.0.0.1", quint16(5025 + _portIterator), parent )
{}

Socket::Socket(const QString &hostName, quint16 port, QObject *parent) : CommunicationSVNA( parent )
{
    _run = false;
    _devInfo.name = QString::number( port );
    _devInfo.hostName = hostName;
    _devInfo.tcpPort = port;

    _socket = new QTcpSocket( this );

    QObject::connect(_socket, &QIODevice::readyRead, this, &Socket::readData );
    QObject::connect(_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Socket::interceptError);
    //QObject::connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(Socket::interceptError));
    QObject::connect(_socket, &QTcpSocket::stateChanged,this, &Socket::stateChanged );

    ++_portIterator;

    _timer = new QTimer( this );
    _timer->setInterval( 500 );
    QObject::connect(_timer, SIGNAL(timeout()), this, SLOT(checkDevConnect()));
}

Socket::~Socket()
{
    _socket->close();
    _timer->stop();
    emit pipelineFinishing();
    --_portIterator;
}

void Socket::setHostName(const QString &hostName)
{
    _devInfo.hostName = hostName;
}

void Socket::setPort(quint16 port)
{
    _devInfo.tcpPort = port;
}

DevInfo Socket::getDevInfo() const
{
    return _devInfo;
}

void Socket::connect()
{
    //qInfo()<<"SOCKET connect threadid: "<<QThread::currentThreadId();
    //qInfo() << "Device::connect(): " << _devInfo.hostName << ":" << _devInfo.tcpPort;
    _devInfo.serial = "";
    if( _socket->isOpen() )
        _socket->abort();
    _socket->connectToHost( _devInfo.hostName, _devInfo.tcpPort );
}

void Socket::connect(const QString &hostName, quint16 port)
{
    setHostName(hostName);
    setPort(port);
    connect();
}

QString Socket::cmd(const QList<QString> &cmds, int timeOutMSec )
{
    //qInfo()<<"SOCKET cmd threadid: "<<QThread::currentThreadId();
    _run = true;
    _qCmds = cmds;

    QEventLoop wait;
    QObject::connect( this, SIGNAL( pipelineFinishing() ), &wait, SLOT( quit() ) );

    QTimer timeOutTimer;
    if( timeOutMSec > 0 ) {
        timeOutTimer.setSingleShot( true );
        timeOutTimer.setInterval( timeOutMSec );
        QObject::connect( &timeOutTimer, SIGNAL( timeout() ), &wait, SLOT( quit() ) );
        timeOutTimer.start();
    }

    if( nextPipeline() )
        wait.exec();

    _run = false;
    if( timeOutMSec > 0 ) {
        // Проверяем, выполнился ли тайм-аут
        if( !timeOutTimer.isActive() ) {
           _readBuf.clear();
           return "TIMEOUT";
        } else {
            // Если таймаут не наступил, останавливаем таймер
            timeOutTimer.stop();
        }
    }
    return _readBuf;
}

void Socket::checkDevConnect()
{
        if(!_run && ( _devInfo.status == csTcpOnDevOn ||_devInfo.status == csTcpOnDevOff))
        {
            ConnectStatus status = getStatus();
            if( _devInfo.status != status )
            {
                _devInfo.status = status;
                emit updateStatus();
            }
        }
}

void Socket::run(const QString &cmd)
{
    if( _socket->state() == QAbstractSocket::ConnectedState )
    {
        _readBuf.clear();
        _socket->write( cmd.toLatin1() );
    }
    else
    {
        interceptError(_socket->error());
    }
}

void Socket::readData()
{
    QByteArray read = _socket->readAll();

    _readBuf += QString::fromLatin1(read);

    if( ( *(_readBuf.end() - 1) == '\n') ) //почему
    {
        nextPipeline(); // продолжить обработку
    }
}

void Socket::interceptError(QAbstractSocket::SocketError socketError)
{
    QString err = tr("RemoteHostClosedError");
    switch (socketError)
    {
        case QAbstractSocket::RemoteHostClosedError: break;
        case QAbstractSocket::HostNotFoundError:
            err = tr("The host was not found. Please check the host name and port settings.");
            break;
        case QAbstractSocket::ConnectionRefusedError:
            err =  tr("The connection was refused by the peer. Make sure the fortune server is running, "
"and check that the host name and port settings are correct.");
            break;
        default:
            err = QString(tr("The following error occurred: %1.")).arg( _socket->errorString() );
    }
    emit error(err );
}

void Socket::stateChanged(QAbstractSocket::SocketState state)
{
    _timer->stop();

    switch( state )
    {
        case QAbstractSocket::UnconnectedState: {
            //qInfo() << _devInfo.name << ": UnconnectedState";
            QTimer::singleShot( 500, this, SLOT(connect()) );
        } break;
        case QAbstractSocket::ConnectedState:   {
        _timer->start();
        _devInfo.idn = cmd( QList<QString>() << "*IDN?\n" );
        QStringList list = _devInfo.idn.split(',');
        if( list.size() > 3 )
        {
             _devInfo.name = list[ 1 ].trimmed();
             _devInfo.serial = list[ 2 ].trimmed();
        }
        //qInfo() << _devInfo.name << ": ConnectedState: " << _devInfo.idn;
    } break;
        case QAbstractSocket::HostLookupState:  qInfo() << _devInfo.name << ": HostLookupState"; break;
        case QAbstractSocket::ConnectingState:  qInfo() << _devInfo.name << ": ConnectingState"; break;
        case QAbstractSocket::BoundState:       qInfo() << _devInfo.name << ": BoundState";       break;
        case QAbstractSocket::ListeningState:   qInfo() << _devInfo.name << ": ListeningState";   break;
        case QAbstractSocket::ClosingState:     qInfo() << _devInfo.name << ": ClosingState";  break;
    }
    if((_devInfo.status != getStatus()) && (getStatus() == csTcpOnDevOn))
        sigDeviceConnected(true, getNameDevice(), _devInfo.idn);

    if((_devInfo.status != getStatus()) && (getStatus() != csTcpOnDevOn))
        sigDeviceConnected(false, getNameDevice(), _devInfo.idn);


    _devInfo.status = getStatus();
    emit updateStatus();
}

bool Socket::nextPipeline()
{
    forever {
        if( _qCmds.isEmpty() )
        {
            emit pipelineFinishing();
            return false;
        }

        QString cmd = _qCmds.takeFirst();
        run( cmd );

        if( cmd.size() > 2 && cmd.contains("?", Qt::CaseInsensitive))
            return true;
    }
}

ConnectStatus Socket::getStatus()
{
    if( (_socket != nullptr) && (_socket->state() == QAbstractSocket::ConnectedState) )
    {
        if( _devInfo.serial != "" )
            return csTcpOnDevOn;
        else
            return csTcpOnDevOff;
    }
    else
        return csTcpOff;
}
