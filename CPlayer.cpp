#include "CPlayer.h"

CPlayer::CPlayer()
{
    m_connector = new CNet( this );
}

CPlayer::~CPlayer()
{
    delete m_connector;
}

CNet *CPlayer::get_connector()
{
    return m_connector;
}
