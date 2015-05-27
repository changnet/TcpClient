#include "CPlayer.h"
#include "CProtoc.h"

CPlayer::CPlayer(QObject *parent)
    :QObject(parent)
{
    m_connector = new CNet();
}

CPlayer::~CPlayer()
{
    delete m_connector;
}

CNet *CPlayer::get_connector()
{
    return m_connector;
}
