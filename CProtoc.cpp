#include "CProtoc.h"
#include "CConfig.h"
#include "err_code.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

CProtoc *CProtoc::m_instance = NULL;
CProtoc::CProtoc()
{
    CConfig *config = CConfig::instance();

    google::protobuf::compiler::DiskSourceTree disk_source;
    disk_source.MapPath( "",config->proto_source() );

    m_importer = new google::protobuf::compiler::Importer( &disk_source,NULL );

    const QList<std::string> &files = config->proto_files();
    QList<std::string>::const_iterator itr = files.constBegin();
    while ( itr != files.end() )
    {
        m_importer->Import( *itr );
        itr ++;
    }

    m_factor = new google::protobuf::DynamicMessageFactory();
}

CProtoc::~CProtoc()
{
    if ( m_importer )
        delete m_importer;
    m_importer = NULL;

    if ( m_factor )
        delete m_factor;
    m_factor = NULL;
}

CProtoc *CProtoc::instance()
{
    if (!m_instance)
        m_instance = new CProtoc();

    return m_instance;
}

void CProtoc::uninstance()
{
    if ( m_instance )
        delete m_instance;
    m_instance = NULL;
}

bool CProtoc::parse_input(const QString &msg_name, const QString &json)
{
    google::protobuf::Message *msg = get_msg( msg_name );
    if ( !msg )
    {
        m_str_err = EC_NO_MSG;
        return false;
    }

    const QByteArray &bytes = json.toUtf8();

    return json_to_pb( bytes,msg );
}

bool CProtoc::json_to_pb( const QByteArray &json, google::protobuf::Message *pmsg )
{
    QJsonParseError jpe;
    QJsonDocument jd = QJsonDocument::fromJson( json,&jpe );
    if ( QJsonParseError::NoError != jpe.error )
    {
        m_str_err = jpe.errorString();
        return false;
    }

    if ( !jd.isObject() ) //to convert to pb,must be object
    {
        m_str_err = EC_JS_NOT_OBJ;
        return false;
    }

    const QJsonObject &jo = jd.object();
    QJsonObject::const_iterator itr = jo.constBegin();
    while ( itr != jo.end() )
    {
        QString _key = itr.key();
        QJsonValue _value = itr.value();

        itr ++;
    }

    return true;
}

google::protobuf::Message *CProtoc::get_msg(const QString &msg_name)
{
    if ( !m_msg_list.contains( msg_name ) )
    {
        const google::protobuf::Descriptor *descriptor = m_importer->pool()->FindMessageTypeByName( msg_name.toStdString() );
        if ( !descriptor )  //can't find such a message
            return NULL;

        const google::protobuf::Message *message = m_factor->GetPrototype( descriptor );
        google::protobuf::Message *_msg = message->New();

        m_msg_list.insert( msg_name,_msg );

        return _msg;
    }

    return m_msg_list[msg_name];//if msg_name not exist,map will insert pair that key is msg_name.even in std::map
}

const QString &CProtoc::get_last_err_str()
{
    return m_str_err;
}


