#ifndef CCONFIG_H
#define CCONFIG_H

#include <QString>
#include <QMap>
#include <lua.hpp>

class CConfig
{
public:
    static CConfig *instance();
    static void uninstance();

    const QString &proto_source();
    const QList<QString> &proto_files();

    bool parse_lua_config();
    int get_code( const QString &msg );
    const QString get_msg( int code );
    const QMap<int,QString> &get_code_msg_list();
    const QString get_client_msg( int code );
    const QMap<int,QString> &get_client_msg_list();

    const QString &get_last_err();
private:
    CConfig();
    ~CConfig();

    QString m_proto_source;
    QList<QString> m_proto_files;
    QMap<int,QString> m_code_to_msg;
    QMap<int,QString> m_client_msg;

    static CConfig *m_config;
    QString m_last_err;

    bool load_lua_cmd();
    bool load_lua_file(const QString &file,lua_State *L);
    bool get_proto_files(lua_State *L, const char *func );
    bool get_msgs( lua_State *L,const char *func );
};

#endif // CCONFIG_H
