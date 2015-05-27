#ifndef CPROTOC_H
#define CPROTOC_H

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

class CProtoc
{
public:
    static CProtoc *instance();
    static void uninstance();
private:
    CProtoc();
    ~CProtoc();

    google::protobuf::compiler::Importer *m_importer;

    static CProtoc *m_instance;
};

#endif // CPROTOC_H
