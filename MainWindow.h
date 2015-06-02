#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QAction>
#include <QMenu>

#include "CPlayer.h"
#include "color.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_package(const QString msg_name, int code, int err, const QString str );
    void on_send();
    void on_connect();
    void on_disconnect();
    void on_status( const QString &st,Color color = CL_BLACK,int timeout = 5000 );
    void on_import_proto_files();
    void on_parse_lua_config(bool is_connect);

    void on_code_index_change(const QString &text);
    void on_msg_index_change(const QString &text);

    void clear_output();

    void showContextMenu(const QPoint &pt);

private:
    void set_status(const QString &st, Color color = CL_BLACK , int timeout = 5000 );
    void write_setting();
    void read_setting();

    void closeEvent(QCloseEvent *event);

private:
    QLineEdit m_le_ip;
    QLineEdit m_le_port;
    QPushButton m_pb_connect;
    QPushButton m_pb_disconnect;

    QComboBox m_cb_code;
    QComboBox m_cb_msg;

    QTextEdit m_te_input;
    QTextEdit m_te_output;

    QPushButton m_pb_send;

    CPlayer m_player;

    QMenu *m_output_menu;
    QAction *m_output_clear;
};

#endif // MAINWINDOW_H
