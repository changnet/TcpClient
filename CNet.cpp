#include "CNet.h"

#include <QDataStream>

CNet::CNet(QObject *parent) :
    QTcpSocket(parent)
{
    connect(this,SIGNAL(connected()),this,SLOT(on_connected()));
    connect(this,SIGNAL(disconnected()),this,SLOT(on_disconnected()));
    connect(this,SIGNAL(readyRead()),this,SLOT(on_read()));
    connect(this,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_err(QAbstractSocket::SocketError)));
    connect( this,SIGNAL(bytesWritten(qint64)),this,SLOT(on_bytes_send(qint64)) );
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
    emit sig_msg( "disconnect from host",CL_RED,0 );
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

         //一个消息读完了，开始解析协议
         if( m_r_buf.is_parse_done() )
         {
             parse_protocol();
             m_r_buf.reset();
         }
    }

    //error
    if ( !sucess )
    {
        emit sig_msg( "read fail when read,disconnect",CL_RED,10000 );
        disconnect_host();
    }
}


/**
 * @brief CNet::disconnect_host
 * safely disconnect from host
 */
void CNet::disconnect_host()
{
    disconnectFromHost();
}

void CNet::on_send(quint32 head, quint16 check, quint8 flag, quint32 phead, quint32 code, quint32 err, const char *buf, int len)
{
    /* QByteArray感觉不如直接用char[]，效率也未知 */
    /*
    //以下两种方法都不对
    {
    QDataStream wstream( &m_s_buf,QIODevice::Append );
    wstream << head << check << flag << phead << code << err;
    wstream.writeRawData( buf,len ); // not wstreawm.writeBytes
    }

    m_s_buf.append( QByteArray::number(head) );
    m_s_buf.append( QByteArray::number(check) );
    m_s_buf.append( QByteArray::number(flag) );
    m_s_buf.append( QByteArray::number(phead) );
    m_s_buf.append( QByteArray::number(code) );
    m_s_buf.append( QByteArray::number(err) );
    m_s_buf.append( buf,len );
    */

    m_s_buf.append( reinterpret_cast<const char*>(&head),4 );
    m_s_buf.append( reinterpret_cast<const char*>(&check),2 );
    m_s_buf.append( reinterpret_cast<const char*>(&flag),1 );
    m_s_buf.append( reinterpret_cast<const char*>(&phead),4 );
    m_s_buf.append( reinterpret_cast<const char*>(&code),4 );
    m_s_buf.append( reinterpret_cast<const char*>(&err),4 );
    m_s_buf.append( buf,len );

    on_bytes_send(0);
}

void CNet::on_bytes_send(qint64 bytes)
{
    Q_UNUSED(bytes);

    int _size = m_s_buf.size();
    if  (_size <= 0)
        return;

    int ret = write( m_s_buf );
    if ( -1 == ret ) //error
    {
        emit sig_msg( "write fail when send,disconnect",CL_RED,10000 );
        disconnect_host();
        return;
    }
    else if ( ret < _size )//一次没发完
    {
        QByteArray rightbyte = m_s_buf.right( _size - ret );
        m_s_buf.clear();
        m_s_buf.append(rightbyte);
    }
    else
        m_s_buf.clear();
}

//quint32 head, quint16 check, quint8 flag, quint32 phead, quint32 code, quint32 err, const char *buf, int len
bool CNet::parse_protocol()
{
    //协议头已不在buff内
    char *data = m_r_buf.data();
    data += 2 + 1;  //跳过 check flag

    quint32 phead = *(reinterpret_cast<quint32 *>(data));
    data += 4;
    quint32 code  = *(reinterpret_cast<quint32 *>(data));
    data += 4;
    quint32 err  = *(reinterpret_cast<quint32 *>(data));
    data += 4;

    int len = phead - 8; //减去code err长度

    emit sig_protocol( code,err,data,len );
}
