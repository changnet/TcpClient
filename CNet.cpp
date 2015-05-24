#include "CNet.h"
#include "CPlayer.h"

CNet::CNet(CPlayer *parent) :
    QTcpSocket(0)
{
    m_player = parent;
}
