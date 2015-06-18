#ifndef CPLAYER_H
#define CPLAYER_H

#include <QObject>
#include "CNet.h"
#include "CProtoc.h"

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
    void sig_package( const QString msg_name,int code,int err,const QString str,int );
private:
    CNet *m_connector;

    bool send_pkt( const int code,const int err,const PROTO_BUF &buf );
public slots:
    void on_package(quint32 code, quint32 err, const char *buf, int len , int pack_len);
};

#endif // CPLAYER_H
