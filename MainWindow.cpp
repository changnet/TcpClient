#include "MainWindow.h"

#include <QStatusBar>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->resize( 640,480 );

    QStatusBar *pbar = this->statusBar();
    pbar->addWidget( &status_text );
    status_text.setText( "ready" );

    QHBoxLayout *psvlayout = new QHBoxLayout();
    m_pb_connect.setText( "connect" );
    m_pb_disconnect.setText( "disconnect" );
    psvlayout->addWidget( &m_le_ip );
    psvlayout->addWidget( &m_le_port );
    psvlayout->addWidget( &m_pb_connect );
    psvlayout->addWidget( &m_pb_disconnect );

    QHBoxLayout *pprolayout = new QHBoxLayout();
    m_pb_reload.setText( "reload" );
    pprolayout->addWidget( &m_cb_proto );
    pprolayout->addWidget( &m_le_proname );
    pprolayout->addWidget( &m_pb_reload );

    QVBoxLayout *pleftlayout = new QVBoxLayout();
    pleftlayout->addLayout( psvlayout );
    pleftlayout->addLayout( pprolayout );
    pleftlayout->addWidget( &m_te_input );

    QGridLayout *pmain = new QGridLayout();
    pmain->setColumnStretch( 0,1 );
    pmain->setColumnStretch( 1,1 );

    pmain->addLayout( pleftlayout,0,0 );
    pmain->addWidget( &m_te_output,0,1 );

    QWidget *widget = new QWidget();
    widget->setLayout( pmain );

    this->setCentralWidget( widget );
}

MainWindow::~MainWindow()
{

}
