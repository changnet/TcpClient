#include "CProtoc.h"
#include "CConfig.h"

#include <QJsonDocument>
#include <QJsonObject>

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
}

CProtoc::~CProtoc()
{

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

bool CProtoc::parse_input(const QString &msg, const QString &json)
{
    QByteArray &bytes = json.toUtf8();

    const google::protobuf::Descriptor *descriptor = m_importer->pool()->FindMessageTypeByName( msg.toStdString() );
    google::protobuf::Message *msg;
}

bool CProtoc::json_to_pb( const QByteArray &json, google::protobuf::Message *pmsg )
{
    QJsonDocument jd = QJsonDocument::fromJson( json );
    if ( !jd.isObject() ) //to convert to pb,must be object
    {
        return false;
    }

    const QJsonObject &jo = jd.object();
    QJsonObject::const_iterator itr = jo.constBegin();
    while ( itr != jo.end() )
    {
        QString _key = itr.key();
        QJsonValue _value = itr.value();
    }
}



