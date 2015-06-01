#include "CConfig.h"

CConfig *CConfig::m_config = NULL;

CConfig::CConfig()
{
    //TODO:test data
    m_proto_source = ".";

    m_proto_files.push_back( "Login.proto" );
}

CConfig::~CConfig()
{
}

CConfig *CConfig::instance()
{
    if ( !m_config )
        m_config = new CConfig();

    return m_config;
}

void CConfig::uninstance()
{
    if ( m_config )
        delete m_config;

    m_config = NULL;
}

const QString &CConfig::proto_source()
{
    return m_proto_source;
}

/**
 * @brief CConfig::proto_files
 * @return
 * get all proto files need to be import
 */
const QList<QString> &CConfig::proto_files()
{
    return m_proto_files;
}

bool CConfig::parse_lua_config()
{
    m_code_to_msg.clear();

    m_code_to_msg[20001] = "CLogin";
    m_code_to_msg[40001] = "SLogin";
    m_code_to_msg[20002] = "CCreateRole";
    m_code_to_msg[25001] = "CStartGame";

    return true;
}

int CConfig::get_code(const QString &msg)
{
    //不要求效率，直接遍历查找。不需要映射msg_to_code
    QMap<int,QString>::iterator itr = m_code_to_msg.begin();
    while ( itr != m_code_to_msg.constEnd() )
    {
        if ( 0 == msg.compare( itr.value() ) )
            return itr.key();

        itr ++;
    }

    return 0;
}

const QString &CConfig::get_msg(int code)
{
    if ( m_code_to_msg.contains( code ) )
        return m_code_to_msg[code];

    return "";
}

const QMap<int,QString> &CConfig::get_code_msg_list()
{
    return m_code_to_msg;
}
