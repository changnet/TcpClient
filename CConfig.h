#ifndef CCONFIG_H
#define CCONFIG_H

#include <QString>
#include <QMap>

class CConfig
{
public:
    static CConfig *instance();
    static void uninstance();

    const QString &proto_source();
    const QList<QString> &proto_files();

    bool parse_lua_config();
    int get_code( const QString &msg );
    const QString &get_msg( int code );
    const QMap<int,QString> &get_code_msg_list();
private:
    CConfig();
    ~CConfig();

    QString m_proto_source;
    QList<QString> m_proto_files;
    QMap<int,QString> m_code_to_msg;

    static CConfig *m_config;

    bool load_lua_cmd();
};

#endif // CCONFIG_H
