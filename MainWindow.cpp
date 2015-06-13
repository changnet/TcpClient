#include "MainWindow.h"
#include "CProtoc.h"
#include "CConfig.h"

#include <QStatusBar>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractItemView>
#include <QString>
#include <QFont>
#include <QSettings>
#include <QCloseEvent>
#include <QDateTime>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->resize( 960,480 );

    ///////////////////////////////////////////////服务器信息栏///////////////////////////////////////////

    QGridLayout *psvlayout = new QGridLayout();
    m_pb_connect.setText( "connect" );
    m_pb_disconnect.setText( "disconnect" );

    QLabel *label_ip = new QLabel( "IP:" );
    QLabel *label_port = new QLabel( "PORT:" );

    psvlayout->setColumnStretch( 1,4 );
    psvlayout->setColumnStretch( 3,1 );

    psvlayout->addWidget( label_ip,0,0 );
    psvlayout->addWidget( &m_le_ip,0,1 );
    psvlayout->addWidget( label_port,0,2 );
    psvlayout->addWidget( &m_le_port,0,3 );
    psvlayout->addWidget( &m_pb_connect,0,4 );
    psvlayout->addWidget( &m_pb_disconnect,0,5 );

    ///////////////////////////////////////////////协议栏////////////////////////////////////////////////

    QGridLayout *pprolayout = new QGridLayout();
    m_pb_send.setText( "send" );
    QLabel *label_code = new QLabel( "CODE:" );
    QLabel *label_msg = new QLabel( "MSG:" );
    m_cb_code.view()->setMinimumHeight( 60 );
    m_cb_msg.view()->setMinimumHeight( 60 );

    pprolayout->setColumnStretch(1,1);
    pprolayout->setColumnStretch(3,3);

    pprolayout->addWidget( label_code,0,0);
    pprolayout->addWidget( &m_cb_code,0,1 );
    pprolayout->addWidget( label_msg,0,2 );
    pprolayout->addWidget( &m_cb_msg,0,3 );
    pprolayout->addWidget( &m_pb_send,0,4 );

    ////////////////////////////////////////////////控制栏///////////////////////////////////////////////
    QGridLayout *pctrllayout = new QGridLayout();

    pctrllayout->setColumnStretch(0,1);
    pctrllayout->setColumnStretch(1,1);
    pctrllayout->setColumnStretch(2,1);
    pctrllayout->setColumnStretch(3,1);
    pctrllayout->setColumnStretch(4,3);

    m_pb_sk1.setText( "SK-1" );
    m_pb_sk2.setText( "SK-2" );
    m_pb_sk3.setText( "SK-3" );
    m_pb_sk4.setText( "SK-4" );
    m_cb_history.view()->setMinimumHeight( 60 );

    pctrllayout->addWidget( &m_pb_sk1,0,0 );
    pctrllayout->addWidget( &m_pb_sk2,0,1 );
    pctrllayout->addWidget( &m_pb_sk3,0,2 );
    pctrllayout->addWidget( &m_pb_sk4,0,3 );
    pctrllayout->addWidget( &m_cb_history,0,4 );

    ///////////////////////////////////////////////统一左栏///////////////////////////////////////////////////

    QVBoxLayout *pleftlayout = new QVBoxLayout();
    pleftlayout->addLayout( psvlayout );
    pleftlayout->addLayout( pprolayout );
    pleftlayout->addLayout( pctrllayout );
    pleftlayout->addWidget( &m_te_input );

    ///////////////////////////////////////////////左右整合///////////////////////////////////////////////////

    QGridLayout *pmain = new QGridLayout();
    pmain->setColumnStretch( 0,1 );
    pmain->setColumnStretch( 1,1 );

    pmain->addLayout( pleftlayout,0,0 );
    pmain->addWidget( &m_te_output,0,1 );

    QWidget *widget = new QWidget();
    widget->setLayout( pmain );

    this->setCentralWidget( widget );

    ////////////////////////////////////////////////初始化/////////////////////////////////////////////////////

    connect( &m_pb_connect,SIGNAL(clicked()),this,SLOT(on_connect()) );
    connect( &m_pb_disconnect,SIGNAL(clicked()),this,SLOT(on_disconnect()) );
    connect( &m_pb_send,SIGNAL(clicked()),this,SLOT(on_send()) );

    CNet *connector = m_player.get_connector();
    connect( connector,SIGNAL(sig_msg(QString,Color,int)),this,SLOT(on_status(QString,Color,int)) );
    connect( &m_player,SIGNAL(sig_msg(QString,Color,int)),this,SLOT(on_status(QString,Color,int)) );
    connect( &m_player,SIGNAL(sig_package(QString,int,int,QString)),this,SLOT(on_package(QString,int,int,QString)) );

    m_te_output.setReadOnly( true );
    m_te_output.setLineWrapMode(QTextEdit::NoWrap);
    m_cb_code.setEditable( true );
    m_cb_msg.setEditable( true );

    //右键菜单
    m_output_clear = new QAction("clear",this);
    m_te_output.setContextMenuPolicy(Qt::CustomContextMenu);

    m_output_menu = m_te_output.createStandardContextMenu();
    m_output_menu->addAction( m_output_clear );

    connect( m_output_clear,SIGNAL(triggered()),this,SLOT(clear_output()) );

    connect( &m_te_output,SIGNAL(customContextMenuRequested(const QPoint&)),
                this,SLOT(showContextMenu(const QPoint&)));

    //菜单栏
    QMenuBar *menu_bar = menuBar();
    m_history_clear = new QAction( "clear history",menu_bar );
    m_config_reset  = new QAction( "reset config",menu_bar );
    menu_bar->addAction( m_history_clear );
    menu_bar->addAction( m_config_reset );
    connect( m_history_clear,SIGNAL(triggered()),this,SLOT(clear_history()) );
    connect( m_config_reset,SIGNAL(triggered()),this,SLOT(reset_config()) );

    connect( &m_pb_sk1,SIGNAL(pressed()),this,SLOT(sk1_pressed()) );
    connect( &m_pb_sk1,SIGNAL(released()),this,SLOT(sk1_released()) );
    connect( &m_pb_sk2,SIGNAL(pressed()),this,SLOT(sk2_pressed()) );
    connect( &m_pb_sk2,SIGNAL(released()),this,SLOT(sk2_released()) );
    connect( &m_pb_sk3,SIGNAL(pressed()),this,SLOT(sk3_pressed()) );
    connect( &m_pb_sk3,SIGNAL(released()),this,SLOT(sk3_released()) );
    connect( &m_pb_sk4,SIGNAL(pressed()),this,SLOT(sk4_pressed()) );
    connect( &m_pb_sk4,SIGNAL(released()),this,SLOT(sk4_released()) );

    connect( &m_cb_history,SIGNAL(currentIndexChanged(int)),this,SLOT(on_history_index_change(int)) );

    //////////////////////////////////////////////初始化其他组件/////////////////////////////////////////////////
    on_parse_lua_config(false); //先配置后初始化protobuf
    on_import_proto_files();

    read_setting();

    set_status( "ready",CL_GREEN,0 );
}

MainWindow::~MainWindow()
{
    CProtoc::uninstance();
    CConfig::uninstance();
}

void MainWindow::on_connect()
{
    const QString &ip   = m_le_ip.text();
    const QString &port = m_le_port.text();

    if ( ip.isEmpty() || port.isEmpty() )
    {
        set_status( "ip or port empty",CL_RED );
        return;
    }

    CNet *connector = m_player.get_connector();
    connector->connect_host( ip,port.toInt() );
}

void MainWindow::on_disconnect()
{
    CNet *connector = m_player.get_connector();
    connector->disconnect_host();
}

void MainWindow::on_status(const QString &st, Color color,int timeout)
{
    set_status( st,color,timeout );
}

void MainWindow::set_status(const QString &st, Color color,int timeout)
{
    QString cl;
    switch ( color )
    {
    case CL_RED   : cl = "color:red";break;
    case CL_GREEN : cl = "color:green";break;
    case CL_BLACK : cl = "color:black";break;
    default       : cl = "color:black";break;
    }

    statusBar()->setStyleSheet( cl );
    statusBar()->showMessage( st,timeout );
}

void MainWindow::on_send()
{
    /* some package maybe empty */
    const QString &str_send = m_te_input.toPlainText();

    const QString &str_code = m_cb_code.currentText();
    const QString &str_msg_name = m_cb_msg.currentText();
    if ( str_code.isEmpty() )
    {
        set_status( "no protocol code is specified",CL_RED );
        return;
    }

    if ( str_msg_name.isEmpty() )
    {
        set_status( "no message name is specified",CL_RED );
        return;
    }

    int code = str_code.toInt();
    bool ret = m_player.send_package( code,str_msg_name,str_send );
    if ( ret )
        set_status( "send ...",CL_GREEN );

    add_history( code,str_msg_name,str_send );
}

void MainWindow::on_package(const QString msg_name,int code,int err,const QString str)
{
    QString str_info = QString("MSG:%1 CODE:%2 ERR:%3").arg(msg_name).arg(code).arg(err);

    QString tmp = ">>>>>>>>>>";
    tmp.append(str_info);
    tmp.append("<<<<<<<<<<");

    m_te_output.append(tmp);
    m_te_output.append(str);
}

void MainWindow::on_import_proto_files()
{
    CProtoc *protoc = CProtoc::instance();
    protoc->import_proto_files();

    CProtoFileErrorCollector *collector = protoc->get_error_collector();
    const QList<QString> &err_list = collector->get_error_list();

    QList<QString>::const_iterator itr = err_list.constBegin();

    m_te_output.setTextColor( QColor("red") );
    while ( itr != err_list.constEnd() )
    {
        m_te_output.append( *itr );
        m_te_output.append( "\n" );

        itr ++;
    }
    m_te_output.setTextColor( QColor("black") );
}

void MainWindow::on_parse_lua_config( bool is_connect )
{
    if ( is_connect )
    {
        disconnect( &m_cb_code,SIGNAL(currentIndexChanged(int)),this,SLOT(on_code_index_change(int)) );
        disconnect( &m_cb_msg,SIGNAL(currentIndexChanged(int)),this,SLOT(on_msg_index_change(int)) );
    }

    m_cb_code.clear();
    m_cb_msg.clear();

    CConfig *config = CConfig::instance();

    if ( !config->parse_lua_config() )
    {
        set_status( config->get_last_err(),CL_RED,6000 );
        return;
    }

    const QMap<int,QString> &code_msg_list = config->get_code_msg_list();
    QMap<int,QString>::const_iterator itr = code_msg_list.constBegin();
    while( itr != code_msg_list.constEnd() )
    {
        m_cb_code.addItem( QString("%1").arg(itr.key()) );
        m_cb_msg.addItem( itr.value() );

        itr ++;
    }

    m_cb_code.setCurrentText( "" );
    m_cb_msg.setCurrentText( "" );
    //虽然设置为空，但这时currentIndex都为0，即第一个

    connect( &m_cb_code,SIGNAL(currentTextChanged(const QString &)),this,SLOT(on_code_index_change(const QString &)) );
    connect( &m_cb_msg,SIGNAL(currentTextChanged(const QString &)),this,SLOT(on_msg_index_change(const QString &)) );
}

void MainWindow::on_code_index_change(const QString &text)
{
    Q_UNUSED(text);

    //插入数据时，会触发，这时另一个不一定有相同的索引
    int index = m_cb_code.currentIndex();
    if ( m_cb_msg.count() - 1 < index )
        return;

    m_cb_msg.setCurrentIndex(index); //on_parse_lua_config中是对应位置的，index应该也是对应的
}

void MainWindow::on_msg_index_change(const QString &text)
{
    int index = m_cb_msg.currentIndex();
    if ( m_cb_code.count() - 1 < index )
        return;

    m_cb_code.setCurrentIndex(index);

    m_te_input.setText( CProtoc::instance()->get_msg_example_str(text) );
}

void MainWindow::read_setting()
{
    QSettings rsetting( "setting.ini",QSettings::IniFormat );
    QString ip = rsetting.value( "IP","192.168.0.23" ).toString();
    QString port = rsetting.value( "PORT","4113" ).toString();

    m_le_ip.setText( ip );
    m_le_port.setText( port );

    //尝试读取4个快捷键
    for ( int i = 1;i < 5;i ++ ) //one base
    {
        int code = rsetting.value( QString("SK/%1/CODE").arg(i),0 ).toInt();
        if ( !code )
            continue;

        QString msg = rsetting.value( QString("SK/%1/MSG").arg(i),"" ).toString();
        QString content = rsetting.value( QString("SK/%1/CONTENT").arg(i),"" ).toString();

        sk_update( code,msg,content,i );
    }

    int cnt = 0;
    struct SKey sk;
    m_history.clear();

    while ( true )
    {
        sk.code = rsetting.value( QString("HISTORY/%1/CODE").arg(cnt),0 ).toInt();
        if ( !code )
            break;

        sk.msg = rsetting.value( QString("HISTORY/%1/MSG").arg(cnt),"" ).toString();
        sk.content = rsetting.value( QString("HISTORY/%1/CONTENT").arg(cnt),"" ).toString();

        m_history.pop_back( sk );
        cnt ++;
    }
    update_history();
}

void MainWindow::write_setting()
{
    QSettings wsetting( "setting.ini",QSettings::IniFormat );

    QString ip = m_le_ip.text();
    QString port = m_le_port.text();

    if ( !ip.isEmpty() )
        wsetting.setValue( "IP",ip );

    if ( !port.isEmpty() )
        wsetting.setValue( "PORT",port );

    //4个快捷键

    QMap<int,SKey>::const_iterator itr = m_sk.constBegin();
    while( itr != m_sk.constEnd() )
    {
        int key  = itr.key();
        wsetting.setValue( QString("SK/%1/CODE").arg(key),itr->code );
        wsetting.setValue( QString("SK/%1/MSG").arg(key),itr->msg );
        wsetting.setValue( QString("SK/%1/CONTENT").arg(key),itr->content );

        itr ++;
    }

    for ( int i = 0;i < m_history.length();i ++ )
    {
        struct SKey &sk = m_history.at( i );
        wsetting.setValue( QString("HISTORY/%1/CODE").arg(i),sk.code );
        wsetting.setValue( QString("HISTORY/%1/MSG").arg(i),sk.msg );
        wsetting.setValue( QString("HISTORY/%1/CONTENT").arg(i),sk.content );
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    write_setting();
    event->accept();
}

void MainWindow::clear_output()
{
    m_te_output.clear();
}

void MainWindow::showContextMenu(const QPoint &pt)
{
    m_output_menu->exec(m_te_output.mapToGlobal(pt));
}

void MainWindow::sk1_pressed()
{
    sk_pressed( 1 );
}

void MainWindow::sk1_released()
{
    sk_released( 1 );
}

void MainWindow::sk2_pressed()
{
    sk_pressed( 2 );
}

void MainWindow::sk2_released()
{
    sk_released( 2 );
}

void MainWindow::sk3_pressed()
{
    sk_pressed( 3 );
}

void MainWindow::sk3_released()
{
    sk_released( 3 );
}

void MainWindow::sk4_pressed()
{
    sk_pressed( 4 );
}

void MainWindow::sk4_released()
{
    sk_released( 4 );
}


void MainWindow::sk_pressed(int index)
{
    m_sk_index = index;
    m_sk_utc = QDateTime::currentMSecsSinceEpoch();
}

/**
 * @brief MainWindow::sk_released
 * @param index
 * 发送(单击)
 * 录制(空白时单击)
 * 删除(长按3S以上)
 */
void MainWindow::sk_released(int index)
{
    if ( m_sk_index != index )
        return;

    qint64 m_sk_current = QDateTime::currentMSecsSinceEpoch();

    if ( m_sk_current - m_sk_utc > 3000 ) //长按3S以上
    {
        if ( m_sk.contains(index) )
        {
            m_sk.remove(index);
            sk_reset( index );
            return;
        }
    }

    //发送
    if ( m_sk.contains(index) )
    {
        sk_send( index );
        return;
    }

    //录制
    QString code = m_cb_code.currentText();
    QString msg  = m_cb_msg.currentText();
    if ( code.isEmpty() || msg.isEmpty() )
        return;

    sk_update( code.toInt(),msg,m_te_input.toPlainText(),index );
}

void MainWindow::sk_reset(int index)
{
    /* 由于为了方便，没有重写QPushButton，只能采用这种查找方式 */
    /* 将它们放到一个数组，connect这些也不好处理 */
    QPushButton *button = NULL;
    switch (index)
    {
    case 1 : button = &m_pb_sk1;break;
    case 2 : button = &m_pb_sk2;break;
    case 3 : button = &m_pb_sk3;break;
    case 4 : button = &m_pb_sk4;break;
    }

    if ( !button )
        return;

    button->setToolTip( "" );
    button->setStyleSheet( "color:black" );
}

void MainWindow::sk_update(int code, const QString &msg, const QString &content, int index)
{
    QPushButton *button = NULL;
    switch (index)
    {
    case 1 : button = &m_pb_sk1;break;
    case 2 : button = &m_pb_sk2;break;
    case 3 : button = &m_pb_sk3;break;
    case 4 : button = &m_pb_sk4;break;
    }

    if ( !button )
        return;

    SKey _sk;
    _sk.code = code;
    _sk.msg = msg;
    _sk.content = content;

    m_sk[index] = _sk;

    QString tips = QString( "CODE:%1\nMSG:%2\n%3").arg(code).arg(msg).arg(content);
    button->setStyleSheet( "color:green" );
    button->setToolTip( tips );
}

void MainWindow::sk_send(int index)
{
    if ( !m_sk.contains(index) )
        return;

    SKey &_sk = m_sk[index];
    bool ret = m_player.send_package( _sk.code,_sk.msg,_sk.content );
    if ( ret )
        set_status( "sk send ...",CL_GREEN );
}

void MainWindow::add_history(int code, const QString &msg, const QString &content)
{
    QList<struct SKey>::const_iterator itr = m_history.constBegin();
    while ( itr != m_history.constEnd() )
    {
        if ( itr->code == code && 0 == QString::compare(itr->msg,msg)
             && 0 == QString::compare(itr->content,content) )
            return;

        itr ++;
    }

    const int max_history = 10;
    while ( m_history.length() >= max_history )
    {
        m_history.pop_back();
    }

    struct SKey sk;
    sk.code = code;
    sk.msg = msg;
    sk.content = content;

    m_history.push_front( sk );
    update_history();
}

void MainWindow::update_history()
{
    //先断开，不然addItem会触发
    disconnect( &m_cb_history,SIGNAL(currentIndexChanged(int)),this,SLOT(on_history_index_change(int)) );

    m_cb_history.clear();

    QList<struct SKey>::const_iterator itr = m_history.constBegin();
    while ( itr != m_history.constEnd() )
    {
        m_cb_history.addItem( itr->msg );

        itr ++;
    }

    connect( &m_cb_history,SIGNAL(currentIndexChanged(int)),this,SLOT(on_history_index_change(int)) );
}

void MainWindow::on_history_index_change(int index)
{
    if ( index < 0 || index >= m_history.length() )
        return;

    struct SKey &sk = m_history.at(index);
    m_cb_code.setCurrentText( QString("%1").arg(sk.code) );
    m_cb_msg.setCurrentText( sk.msg );
    m_te_input.setText( sk.content );
}

void MainWindow::clear_history()
{
    m_history.clear();
    m_cb_history.clear();
}

void MainWindow::reset_config()
{
    CProtoc::uninstance();
    CConfig::uninstance();

    on_parse_lua_config(true); //先配置后初始化protobuf
    on_import_proto_files();
}
