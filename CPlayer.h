#ifndef CPLAYER_H
#define CPLAYER_H

#include <QObject>
#include "CNet.h"

class CPlayer : public QObject
{
    Q_OBJECT

public:
    explicit CPlayer( QObject *parent = 0 );
    virtual ~CPlayer();
    CNet *get_connector();
private:
    CNet *m_connector;
};

#endif // CPLAYER_H
