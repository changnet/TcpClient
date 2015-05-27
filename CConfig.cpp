#include "CConfig.h"

CConfig *CConfig::m_config = NULL;

CConfig::CConfig()
{

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

const std::string &CConfig::proto_source()
{
    return ".";
}
