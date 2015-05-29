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

class CProtoc
{
public:
    static CProtoc *instance();
    static void uninstance();

    bool parse_input(const QString &msg_name, const QString &json );
    const QString &get_last_err_str();
private:
    CProtoc();
    ~CProtoc();

    QMap<QString,google::protobuf::Message *> m_msg_list;
    google::protobuf::compiler::Importer *m_importer;
    google::protobuf::DynamicMessageFactory *m_factor;

    QString m_str_err;
    static CProtoc *m_instance;

    google::protobuf::Message *get_msg(const QString &msg_name);
    bool json_object_to_pb( const QJsonObject &jo,google::protobuf::Message *pmsg );
    bool json_to_pb(const QByteArray &json, google::protobuf::Message *pmsg );
};

#endif // CPROTOC_H
