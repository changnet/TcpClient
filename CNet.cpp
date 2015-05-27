#include "CNet.h"

CNet::CNet(QObject *parent) :
    QTcpSocket(parent)
{
    connect(this,SIGNAL(connected()),this,SLOT(on_connected()));
    connect(this,SIGNAL(disconnected()),this,SLOT(on_disconnected()));
    connect(this,SIGNAL(readyRead()),this,SLOT(on_read()));
    connect(this,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_err(QAbstractSocket::SocketError)));
}

void CNet::on_connected()
{
    //clear old data
    m_r_buf.reset();
    m_s_buf.reset();

    emit sig_msg( "connect success",CL_GREEN,0 );
}

void CNet::on_disconnected()
{
    emit sig_msg( "disconnect from host",CL_GREEN,0 );
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
    //read until no data avilable
    bool sucess = true;
    while ( !atEnd() && sucess )
    {
         m_r_buf.append( read(m_r_buf.expect_len()) );
         sucess = m_r_buf.parse();
    }

    //error
    if ( !sucess )
        disconnect_host();
}


/**
 * @brief CNet::disconnect_host
 * safely disconnect from host
 */
void CNet::disconnect_host()
{
    disconnectFromHost();
}

