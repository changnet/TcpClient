#ifndef CNET_H
#define CNET_H

#include <QTcpSocket>

class CPlayer;
class CNet : public QTcpSocket
{
public:
    explicit CNet(CPlayer *parent = 0);

private:
    CPlayer *m_player;

signals:

public slots:

};

#endif // CNET_H
