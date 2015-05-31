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
private:
    CProtoc();
    ~CProtoc();

    google::protobuf::compiler::Importer *m_importer;
    google::protobuf::DynamicMessageFactory *m_factor;
    CProtoFileErrorCollector *m_err_collector;

    QString m_str_err;
    static CProtoc *m_instance;

    google::protobuf::Message *get_msg(const QString &msg_name);
    QString msg_example_str(google::protobuf::Message *msg, const QString &indent = "");

    void reset_proto();
};

#endif // CPROTOC_H
