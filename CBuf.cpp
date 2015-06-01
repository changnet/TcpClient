#include "CBuf.h"

#include <QDataStream>
#include <QDebug>

CBuf::CBuf()
{

}

CBuf::~CBuf()
{

}

bool CBuf::parse()
{
    //未收完足够数据
    if ( size() < expect_len() )
        return true;

    return m_buf_len > 0 ? parse_data() : parse_head();
}

/**
 * @brief CBuf::expect_len
 * @return
 * get the reset length of a package
 */
int CBuf::expect_len()
{
    int len = m_buf_len > 0 ? m_buf_len : HEAD_LEN;

    return len - size();
}

bool CBuf::parse_head()
{
    m_buf_len = *(reinterpret_cast<qint32 *>(data()));
    this->clear();

    return true;
}

bool CBuf::parse_data()
{
    m_is_done = true;
    return true;
}

void CBuf::reset()
{
    m_is_done = false;
    m_buf_len = 0;
    this->clear();
}

bool CBuf::is_parse_done()
{
    return m_is_done;
}
