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

bool CPlayer::send_package(const int code, const QString &msg_name, const QString &json)
{
    CProtoc *protoc = CProtoc::instance();

    if ( !protoc->parse_input( msg_name,json ) )
    {
        const QString &err = protoc->get_last_err_str();
        emit sig_msg( err,CL_RED,6000 );

        return false;
    }

    return true;
}
