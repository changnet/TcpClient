#ifndef CNET_H
#define CNET_H

#include <QTcpSocket>
#include "CBuf.h"
#include "color.h"

class CNet : public QTcpSocket
{
    Q_OBJECT

public:
    explicit CNet(QObject *parent = 0);
    void connect_host(const QString &host, int port );
    void disconnect_host();

private:
    CBuf       m_r_buf;
    CBuf       m_s_buf;

    bool parse_protocol();

signals:
    void sig_msg(const QString &st,Color color,int timeout);
    void sig_protocol( quint32 code,quint32 err,const char *buf,int len );
public slots:
    void on_connected();
    void on_disconnected();
    void on_read();
    void on_send( quint32 head,quint16 check,quint8 flag,quint32 phead,quint32 code,quint32 err,const char *buf,int len );
    void on_err( QAbstractSocket::SocketError socketError );
    void on_bytes_send(qint64 bytes);
};

#endif // CNET_H
