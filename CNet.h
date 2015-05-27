#ifndef CNET_H
#define CNET_H

#include <QTcpSocket>
#include "color.h"

class CPlayer;
class CNet : public QTcpSocket
{
    Q_OBJECT

public:
    explicit CNet(QObject *parent = 0);
    void connect_host(const QString &host, int port );

private:
    CPlayer *m_player;

signals:
    void sig_msg(const QString &st,Color color,int timeout);
public slots:
    void on_connected();
    void on_read();
    void on_err( QAbstractSocket::SocketError socketError );
};

#endif // CNET_H
