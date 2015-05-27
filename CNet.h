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

signals:
    void sig_msg(const QString &st,Color color,int timeout);
public slots:
    void on_connected();
    void on_disconnected();
    void on_read();
    void on_err( QAbstractSocket::SocketError socketError );
};

#endif // CNET_H
