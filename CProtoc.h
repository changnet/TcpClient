#ifndef CPROTOC_H
#define CPROTOC_H

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

#include <QJsonObject>
#include <QByteArray>
#include <QString>
#include <QMap>

#include <CProtoFileErrorCollector.h>

#define DEFAULT_BUF_LEN 65535 //64*1024-1

struct PROTO_BUF
{
    char buf[DEFAULT_BUF_LEN];
    int len;
};

class CProtoc
{
public:
    static CProtoc *instance();
    static void uninstance();

    bool import_proto_files();
    bool parse_input(const QString &msg_name, const QString &text );
    const QString &get_last_err_str();
    void on_proto_err( const char *msg );
    QString get_msg_example_str( const QString &msg_name );

    CProtoFileErrorCollector *get_error_collector();
    const PROTO_BUF &get_proto_buf();

    bool parse_package( const QString &msg_name,const char *buf,int len );
    const QString &get_package_str();
private:
    CProtoc();
    ~CProtoc();

    google::protobuf::compiler::Importer *m_importer;
    google::protobuf::DynamicMessageFactory *m_factor;
    CProtoFileErrorCollector *m_err_collector;

    QString m_str_err;
    QString m_package_str;
    struct PROTO_BUF m_proto_buf;
    static CProtoc *m_instance;

    google::protobuf::Message *get_msg(const QString &msg_name);
    QString msg_example_str(google::protobuf::Message *msg, int indent = 0);

    void reset_proto();
};

#endif // CPROTOC_H
