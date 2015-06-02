#include "CConfig.h"
#include <QFile>
#include <QDebug>

CConfig *CConfig::m_config = NULL;

CConfig::CConfig()
{
    //TODO:test data
    m_proto_source = ".";
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

    load_lua_cmd();

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

/**
 * @brief CConfig::load_lua_cmd
 * @return
 * 简单取巧解析出命令字
 */
bool CConfig::load_lua_cmd()
{
    lua_State *L = lua_open();
    luaopen_base(L);

    //dofile错误返回非零
    /*if ( luaL_dofile( L,"external_cmd.lua" ) ||
         luaL_dofile( L,"pb_files_find.lua" ) )
    {
        const char *err = lua_tostring(L,-1);
        qDebug() << err;
        lua_close(L);
        return false;
    }

    lua_getglobal(L,"CMD_TO_PBFILED");
*/
    //手动补齐依赖
    const char *pb_path = "PB_PATH = \"\"";
    if ( luaL_loadbuffer(L,pb_path,strlen(pb_path),"PB_PATH") ||
         lua_pcall(L, 0, 0, 0) )
    {
        return false;
    }

    if ( !load_lua_file( "external_cmd.lua",L )
         || !load_lua_file( "internal_cmd.lua",L )
         || !load_lua_file( "pb_files_find.lua",L ) )
    {
        const char *err = lua_tostring(L,-1);
        qDebug() << err;
    }

    get_proto_files( L,"get_pb_files" );
    get_msgs( L,"get_pb_filed" );

    lua_close(L);

    return true;
}

bool CConfig::load_lua_file(const QString &file , lua_State *L)
{
    QFile file_obj( file );
    if ( !file_obj.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        return false;
    }

    QString str_file;
    while ( !file_obj.atEnd() )
    {
        QString line = file_obj.readLine();
        if ( line.startsWith("require") ) //去除require依赖
            continue;

        str_file.append(line);
    }

    QByteArray byte_file = str_file.toLatin1();
    QByteArray byte_name = file.toLatin1();

    if ( luaL_loadbuffer(L,byte_file.data(),byte_file.size(),byte_name.data()) ||
         lua_pcall(L, 0, 0, 0) )
    {
        return false;
    }

    return true;
}

/*
lua_next() 这个函数的工作过程是：
1) 先从栈顶弹出一个 key
2) 从栈指定位置的 table 里取下一对 key-value，先将 key 入栈再将 value 入栈
3) 如果第 2 步成功则返回非 0 值，否则返回 0，并且不向栈中压入任何值

第 2 步中从 table 里取出所谓“下一对 key-value”是相对于第 1 步中弹出的 key 的。table 里第一对 key-value 的前面没有数据，
所以先用 lua_pushnil() 压入一个 nil 充当初始 key。

注意开始的时候先用 lua_gettop() 取了一下 table 在栈中的正索引（前面说过了，在进行这个 lua_next() 过程之前先将 table 入栈，
所以栈大小就是 table 的正索引），后面的 lua_next() 过程中不断的有元素出入栈，所以使用正索引来定位 table 比较方便。

到了 table 中已经没有 key-value 对时，lua_next() 先弹出最后一个 key，然后发现已经没有数据了会返回 0，while 循环结束。
所以这个 lua_next() 过程结束以后 table 就又位于栈顶了。
*/
bool CConfig::get_proto_files(lua_State *L, const char *func)
{
    m_proto_files.clear();

    lua_getglobal( L,func );
    lua_pcall( L,0,1,0 );

    int index = lua_gettop(L);                    //此时栈顶是table
    lua_pushnil(L);                               //压入初始key
    while(lua_next(L, index))                     //
    {
        //qDebug() << lua_tostring(L, -1);          // value在-1
        //qDebug() << lua_tonumber(L, -2);          // key在-2

        QString pb_file(lua_tostring(L, -1));
        pb_file.replace( pb_file.size() - 2,2,"proto" );
        m_proto_files.push_back(pb_file);

        lua_pop(L, 1);                            // 将value从栈里面弹出
    }
    lua_pop(L, 1);

    return true;
}

bool CConfig::get_msgs(lua_State *L, const char *func)
{
    m_code_to_msg.clear();

    lua_getglobal( L,func );
    lua_pcall( L,0,1,0 );

    int index = lua_gettop(L);                    //此时栈顶是table
    lua_pushnil(L);                               //压入初始key
    while(lua_next(L, index))                     //
    {
        //qDebug() << lua_tostring(L, -1);          // value在-1
        //qDebug() << lua_tonumber(L, -2);          // key在-2

        QString msg_name(lua_tostring(L, -1));
        int code = lua_tonumber( L,-2 );
        m_code_to_msg[code] = msg_name;

        lua_pop(L, 1);                            // 将Item从栈里面弹出
    }
    lua_pop(L, 1);

    return true;
}
