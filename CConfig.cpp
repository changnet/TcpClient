#include "CConfig.h"

CConfig *CConfig::m_config = NULL;

CConfig::CConfig()
{
    //TODO:test data
    m_proto_source = ".";
    m_proto_files.push_back( "addressbook.proto" );
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
    return m_proto_source;
}

/**
 * @brief CConfig::proto_files
 * @return
 * get all proto files need to be import
 */
const QList<std::string> &CConfig::proto_files()
{
    return m_proto_files;
}
