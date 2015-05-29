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
    bool send_package( const int code,const QString &msg_name,const QString &json );
signals:
    void sig_msg(const QString &st,Color color,int timeout);
private:
    CNet *m_connector;
};

#endif // CPLAYER_H
