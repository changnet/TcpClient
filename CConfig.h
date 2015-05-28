#ifndef CCONFIG_H
#define CCONFIG_H

#include <string>
#include <QMap>

class CConfig
{
public:
    static CConfig *instance();
    static void uninstance();

    const std::string &proto_source();
    const QList<std::string> &proto_files();
private:
    CConfig();
    ~CConfig();

    QList<std::string> m_proto_files;
    QMap<int,std::string> m_code_to_msg;

    static CConfig *m_config;
};

#endif // CCONFIG_H
