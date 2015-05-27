#include "CBuf.h"

CBuf::CBuf()
{

}

CBuf::~CBuf()
{

}

bool CBuf::parse()
{
    return true;
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
    return true;
}

bool CBuf::parse_data()
{
    return true;
}

void CBuf::reset()
{
    m_buf_len = 0;
    this->clear();
}

