#include "CProtoc.h"
#include "CConfig.h"

CProtoc *CProtoc::m_instance = NULL;
CProtoc::CProtoc()
{

    google::protobuf::compiler::DiskSourceTree disk_source;
    disk_source.MapPath( "",CConfig::instance()->proto_source() );

    m_importer = new google::protobuf::compiler::Importer( &disk_source,NULL );
}

CProtoc::~CProtoc()
{

}

CProtoc *CProtoc::instance()
{
    if (!m_instance)
        m_instance = new CProtoc();

    return m_instance;
}

void CProtoc::uninstance()
{
    if ( m_instance )
        delete m_instance;
    m_instance = NULL;
}

