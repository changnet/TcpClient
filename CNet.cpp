#include "CNet.h"
#include "CPlayer.h"

CNet::CNet(QObject *parent) :
    QTcpSocket(parent)
{
    connect(this,SIGNAL(connected()),this,SLOT(on_connected()));
    connect(this,SIGNAL(readyRead()),this,SLOT(on_read()));
    connect(this,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_err(QAbstractSocket::SocketError)));
}

void CNet::on_connected()
{
    emit sig_msg( "connect success",CL_GREEN,0 );
}

void CNet::connect_host(const QString &host, int port)
{
    abort();
    connectToHost( host,port );
}

void CNet::on_err(QAbstractSocket::SocketError socketError)
{
    emit sig_msg( QString("error(%1):%2").arg(socketError).arg(this->errorString()),
                  CL_RED,10000
                );
}

void CNet::on_read()
{

}
