#ifndef CPROTOFILEERRORCOLLECTOR_H
#define CPROTOFILEERRORCOLLECTOR_H

#include "color.h"

#include <QList>
#include <QString>
#include <google/protobuf/compiler/importer.h>

class CProtoFileErrorCollector :
        public google::protobuf::compiler::MultiFileErrorCollector
{
public:
    CProtoFileErrorCollector();

    void AddError(const std::string& filename, int line, int column,
                            const std::string& message);
    const QList<QString> &get_error_list();
    void clear();
private:
    QList<QString> m_err_list;
};

#endif // CPROTOFILEERRORCOLLECTOR_H
