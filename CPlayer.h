#ifndef CPLAYER_H
#define CPLAYER_H

#include "CNet.h"

class CPlayer
{
public:
    CPlayer();
    virtual ~CPlayer();
    CNet *get_connector();
private:
    CNet *m_connector;
};

#endif // CPLAYER_H
