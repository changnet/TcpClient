#include "MainWindow.h"

#include <QStatusBar>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>

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

    QHBoxLayout *pprolayout = new QHBoxLayout();
    m_pb_reload.setText( "reload msg" );
    QLabel *label_code = new QLabel( "CODE:" );
    QLabel *label_msg = new QLabel( "MSG:" );
    m_cb_proto.setEditable( true );

    pprolayout->addWidget( label_code);
    pprolayout->addWidget( &m_cb_proto );
    pprolayout->addWidget( label_msg );
    pprolayout->addWidget( &m_le_proname );
    pprolayout->addWidget( &m_pb_reload );

    ////////////////////////////////////////////////控制栏///////////////////////////////////////////////
    QHBoxLayout *pctrllayout = new QHBoxLayout();
    m_pb_send.setText( "send" );
    pctrllayout->addWidget( &m_pb_send );

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

    set_status( "ready",CL_GREEN,0 );
}

MainWindow::~MainWindow()
{

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
    const QString &str_send = m_te_input.toPlainText();
    if ( str_send.isEmpty() )
    {
        set_status( "nothing to be send",CL_RED );
        return;
    }

    const QString &str_code = m_cb_proto.currentText();
    const QString &str_msg_name = m_le_proname.text();
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
