#include "CProtoFileErrorCollector.h"

CProtoFileErrorCollector::CProtoFileErrorCollector()
{
}

void CProtoFileErrorCollector::AddError(
        const std::string &filename, int line, int column, const std::string &message)
{
    QString str_err = QString("%1:%2:%3:%4")
            .arg(filename.c_str())
            .arg(line)
            .arg(column)
            .arg(message.c_str());

    m_err_list.push_back( str_err );
}

const QList<QString> &CProtoFileErrorCollector::get_error_list()
{
    return m_err_list;
}


void CProtoFileErrorCollector::clear()
{
    m_err_list.clear();
}
