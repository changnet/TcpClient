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

struct SKey  /* shortcut key */
{
    int code;
    QString msg;
    QString content;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_package(const QString msg_name, int code, int err, const QString str , int len);
    void on_send();
    void on_connect();
    void on_disconnect();
    void on_status( const QString &st,Color color = CL_BLACK,int timeout = 5000 );
    void on_proto_err_msg( const QString &st );

    void on_code_index_change(const QString &text);
    void on_msg_index_change(const QString &text);
    void on_history_index_change(int index);

    void clear_history();
    void reset_config();

    void showContextMenu(const QPoint &pt);

    void sk1_pressed();
    void sk1_released();
    void sk2_pressed();
    void sk2_released();
    void sk3_pressed();
    void sk3_released();
    void sk4_pressed();
    void sk4_released();

private:
    void set_status(const QString &st, Color color = CL_BLACK , int timeout = 5000 );
    void write_setting();
    void read_setting();

    void closeEvent(QCloseEvent *event);

    void sk_pressed( int index );
    void sk_released( int index );
    void sk_reset( int index );
    void sk_send( int index );
    void sk_update( int code,const QString &msg,const QString &content,int index);
    void add_history( int code,const QString &msg,const QString &content );
    void update_history();
    void on_import_proto_files();
    void on_parse_lua_config();
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
    QAction *m_output_copy;
    QAction *m_output_selectall;
    QAction *m_history_clear;
    QAction *m_config_reset;

    QPushButton m_pb_sk1;
    QPushButton m_pb_sk2;
    QPushButton m_pb_sk3;
    QPushButton m_pb_sk4;

    QComboBox m_cb_history;

    QMap<int,struct SKey> m_sk;
    QList<struct SKey> m_history;

    int m_sk_index;
    qint64 m_sk_utc;
    quint8 m_proto_bit;
};

#endif // MAINWINDOW_H
