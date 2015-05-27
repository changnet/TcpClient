#ifndef CCONFIG_H
#define CCONFIG_H

#include <string>

class CConfig
{
public:
    static CConfig *instance();
    static void uninstance();

    const std::string &proto_source();
private:
    CConfig();
    ~CConfig();

    static CConfig *m_config;
};

#endif // CCONFIG_H
