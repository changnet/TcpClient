#ifndef CBUF_H
#define CBUF_H

#include <QByteArray>

#define HEAD_LEN    4  //协议头4字节

class CBuf : public QByteArray
{
public:
    CBuf();
    ~CBuf();

    void reset();
    int expect_len();
    bool parse();
private:
    int m_buf_len;

    bool parse_head();
    bool parse_data();
};

#endif // CBUF_H
