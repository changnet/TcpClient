#include "CProtoc.h"
#include "CConfig.h"
#include "err_code.h"

#include <QStringBuilder>
#include <QDebug>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/text_format.h>

static void ProtoLogHandler(google::protobuf::LogLevel level, const char *filename, int line, const std::string &message)
{
    //v3编译v2这里还有一个warning，不处理
    if ( google::protobuf::LOGLEVEL_ERROR > level )
        return;

    Q_UNUSED(filename);
    Q_UNUSED(line);

    CProtoc::instance()->on_proto_err( message.c_str() );
}

CProtoc *CProtoc::m_instance = NULL;
CProtoc::CProtoc()
{
    m_factor = NULL;
    m_importer = NULL;
    m_err_collector = NULL;
}

bool CProtoc::import_proto_files()
{
    reset_proto();

    CConfig *config = CConfig::instance();

    google::protobuf::SetLogHandler(ProtoLogHandler);

    google::protobuf::compiler::DiskSourceTree disk_source;
    disk_source.MapPath( "",config->proto_source().toStdString() );

    m_err_collector = new CProtoFileErrorCollector();
    m_importer = new google::protobuf::compiler::Importer( &disk_source,m_err_collector );

    const QList<QString> &files = config->proto_files();
    QList<QString>::const_iterator itr = files.constBegin();
    while ( itr != files.end() )
    {
        m_importer->Import( (*itr).toStdString() );
        itr ++;
    }

    m_factor = new google::protobuf::DynamicMessageFactory();
}

void CProtoc::reset_proto()
{
    if ( m_factor )
        delete m_factor;
    m_factor = NULL;

    if ( m_importer )
        delete m_importer;
    m_importer = NULL;

    if ( m_err_collector )
        delete m_err_collector;
    m_err_collector = NULL;
}

CProtoc::~CProtoc()
{
    reset_proto();
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

bool CProtoc::parse_input(const QString &msg_name, const QString &text)
{
    google::protobuf::Message *pmsg = get_msg( msg_name );
    if ( !pmsg )
    {
        m_str_err = EC_NO_MSG % msg_name ;
        return false;
    }

    bool ret = google::protobuf::TextFormat::ParseFromString( text.toStdString(),pmsg );
    if ( ret )
    {
        int len = pmsg->ByteSize();
        if ( DEFAULT_BUF_LEN < len )
        {
            m_str_err = EC_BUF_OVERFLOW;
            ret =  false;
        }
        else
        {
            m_proto_buf.len = len;
            ret = pmsg->SerializeToArray( m_proto_buf.buf,len );
        }
    }

    delete pmsg;

    return ret;
}


/**
 * @brief CProtoc::get_msg
 * @param msg_name
 * @return
 * 不能把消息放在map中重复使用，要考虑同一个消息里某个类型
 * 为数组会多次使用同个message
 */
google::protobuf::Message *CProtoc::get_msg(const QString &msg_name)
{
    const google::protobuf::Descriptor *descriptor = m_importer->pool()->FindMessageTypeByName( msg_name.toStdString() );
    if ( !descriptor )  //can't find such a message
        return NULL;

    const google::protobuf::Message *message = m_factor->GetPrototype( descriptor );
    return message->New();
}

const QString &CProtoc::get_last_err_str()
{
    return m_str_err;
}

void CProtoc::on_proto_err(const char *msg)
{
    m_str_err = QString(msg);
}

CProtoFileErrorCollector *CProtoc::get_error_collector()
{
    return m_err_collector;
}

QString CProtoc::msg_example_str(google::protobuf::Message *msg, int indent)
{
    if ( !msg )
        return "";

    static QString str_indent = "    ";
    QString field_indent = str_indent.repeated(indent);
    QString bracket_indent = str_indent.repeated( indent - 1 );  //<= 0 return a ""
    QString str;

    const google::protobuf::Reflection *reflection = msg->GetReflection();
    const google::protobuf::Descriptor *descriptor = msg->GetDescriptor();
    int field_count = descriptor->field_count();
    for ( int i = 0;i < field_count;i ++ )
    {
        const google::protobuf::FieldDescriptor *field = descriptor->field(i);
        str = str % field_indent % QString("%1:\n").arg(field->name().c_str());

        if ( google::protobuf::FieldDescriptor::TYPE_MESSAGE == field->type())  //嵌套
        {
            //这里只是做样本字符串，也可以用get_msg来获取嵌套message
            google::protobuf::Message *_msg = NULL;
            if ( field->is_repeated() )
                _msg = reflection->AddMessage( msg,field );
            else
                _msg = reflection->MutableMessage( msg,field );

            str = str % msg_example_str( _msg,indent + 1 );
        }
    }

    if ( indent )
        str = bracket_indent % "{\n" % str % bracket_indent % "}\n";

    return str;
}

QString CProtoc::get_msg_example_str(const QString &msg_name)
{
    google::protobuf::Message *pmsg = get_msg( msg_name );
    if ( !pmsg )
    {
        return "";
    }
    QString str =  msg_example_str( pmsg );

    delete pmsg;

    return str;
}

const PROTO_BUF &CProtoc::get_proto_buf()
{
    return m_proto_buf;
}

bool CProtoc::parse_package(const QString &msg_name, const char *buf, int len)
{
    google::protobuf::Message *pmsg = get_msg( msg_name );
    if ( !pmsg )
    {
        m_str_err = EC_NO_MSG % msg_name ;
        return false;
    }

    bool ret = pmsg->ParseFromArray( buf,len );
    //if ( ret )  //still show package info after err
    {
        m_package_str = QString(pmsg->Utf8DebugString().c_str());
    }

    delete pmsg;

    return ret;
}

const QString &CProtoc::get_package_str()
{
    return m_package_str;
}
