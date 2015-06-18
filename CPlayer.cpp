#include "CPlayer.h"
#include "CConfig.h"

CPlayer::CPlayer(QObject *parent)
    :QObject(parent)
{
    m_connector = new CNet();

    connect( m_connector,SIGNAL(sig_protocol(quint32,quint32,const char*,int,int)),this,SLOT(on_package(quint32,quint32,const char*,int,int)) );
}

CPlayer::~CPlayer()
{
    delete m_connector;
}

CNet *CPlayer::get_connector()
{
    return m_connector;
}

bool CPlayer::send_package(const int code, const QString &msg_name, const QString &json)
{
    if ( !m_connector->is_valid() )
    {
        emit sig_msg( "connect is not valid",CL_RED,6000 );
        return false;
    }

    CProtoc *protoc = CProtoc::instance();

    if ( !protoc->parse_input( msg_name,json ) )
    {
        const QString &err = protoc->get_last_err_str();
        emit sig_msg( err,CL_RED,6000 );

        return false;
    }

    const struct PROTO_BUF &buf = protoc->get_proto_buf();


    return send_pkt( code,0,buf );
}

/*
head(4byte)|check(2byte)|flag(1byte)|phead(4byte)|cmd(4byte)|err(4byte)|content(Nbyte)
1、head ：大的数据包的包头，一个大的包包含N个分包
2、check：扩展字段
3、flag：是否经过压缩
4、phead：分包长度。
5、error：服务器返回给客户端的错误号
6、check(2) +flag(1)+ phead(4) + cmd(4) + error(4)+ N,即不包括自身的长度
7、phead = cmd +error + N ,也不包括自身的长度
*/
bool CPlayer::send_pkt(const int code, const int err, const PROTO_BUF &buf)
{
    quint32 head = 2 + 1 + 4 + 4 + 4 + buf.len;
    quint16 check = 0;
    quint8 flag   = 0;
    quint32 phead = 4 + 4 + buf.len;

    m_connector->on_send( head,check,flag,phead,code,err,buf.buf,buf.len );

    return true;
}

void CPlayer::on_package(quint32 code, quint32 err, const char *buf, int len,int pack_len)
{
    CConfig *config = CConfig::instance();
    const QString &msg_name = config->get_msg( code );
    if ( msg_name.isEmpty() )
    {
        emit sig_msg( QString("receive msg(%1),no proto message name found").arg(code),CL_RED,6000 );
        return;
    }

    CProtoc *protoc = CProtoc::instance();
    if ( !protoc->parse_package(msg_name,buf,len) )
    {
        const QString &err = protoc->get_last_err_str();
        emit sig_msg( err,CL_RED,6000 );

        return;
    }

    const QString &package_str = protoc->get_package_str();

    emit sig_package(msg_name,code,err,package_str,pack_len);
}
