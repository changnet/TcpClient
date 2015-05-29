#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>

#include "CPlayer.h"
#include "color.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QLineEdit m_le_ip;
    QLineEdit m_le_port;
    QPushButton m_pb_connect;
    QPushButton m_pb_disconnect;

    QComboBox m_cb_proto;
    QLineEdit m_le_proname;
    QPushButton m_pb_reload;

    QTextEdit m_te_input;
    QTextEdit m_te_output;

    QPushButton m_pb_send;

    CPlayer m_player;
public slots:
    void on_send();
    void on_connect();
    void on_disconnect();
    void on_status( const QString &st,Color color = CL_BLACK,int timeout = 5000 );

private:
    void set_status(const QString &st, Color color = CL_BLACK , int timeout = 5000 );
};

#endif // MAINWINDOW_H
