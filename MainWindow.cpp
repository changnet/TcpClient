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
    //QHBoxLayout *pctrllayout = new QHBoxLayout();

    //pctrllayout->addWidget( &m_pb_send );

    ///////////////////////////////////////////////统一左栏///////////////////////////////////////////////////

    QVBoxLayout *pleftlayout = new QVBoxLayout();
    pleftlayout->addLayout( psvlayout );
    pleftlayout->addLayout( pprolayout );
    //pleftlayout->addLayout( pctrllayout );
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

    //菜单
    m_output_clear = new QAction("clear",this);
    m_te_output.setContextMenuPolicy(Qt::CustomContextMenu);

    m_output_menu = m_te_output.createStandardContextMenu();
    m_output_menu->addAction( m_output_clear );

    connect( m_output_clear,SIGNAL(triggered()),this,SLOT(clear_output()) );

    connect( &m_te_output,SIGNAL(customContextMenuRequested(const QPoint&)),
                this,SLOT(showContextMenu(const QPoint&)));

    set_status( "ready",CL_GREEN,0 );

    //////////////////////////////////////////////初始化其他组件/////////////////////////////////////////////////
    on_parse_lua_config(false); //先配置后初始化protobuf
    on_import_proto_files();

    read_setting();
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

    m_player.send_package( str_code.toInt(),str_msg_name,str_send );
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

    config->parse_lua_config();

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
