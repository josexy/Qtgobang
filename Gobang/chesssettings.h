#ifndef CHESSSETTINGS_H
#define CHESSSETTINGS_H

#include <QMouseEvent>
#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QTextEdit>
#include <QTcpServer>
#include <QTcpSocket>
#include "ui_form.h"
#include "chesspanel.h"

class ChessSettings: public QWidget
{
    Q_OBJECT
public:

    ChessSettings(int mode,QWidget *parent,ChessPanel *chessPanel=nullptr);
    ~ChessSettings();
    bool safeExit;

public slots:
    void readData();
    void disConnection();
    void timeOut();
private slots:
    void on_btnRegretChess_clicked();

    void on_btnNewGame_clicked();

    void on_cbShowOrderNum_stateChanged(int arg1);

    void on_btnServerListen_clicked();

    void on_btnClientConn_clicked();

    void on_cbSound_stateChanged(int arg1);

    void on_btnQuitConn_clicked();

    void on_btnRestartGame_clicked();

    void on_btnSendMsg_clicked();

    void on_radioWAN_toggled(bool checked);

private:
    int mode;
    Ui::Form *ui;
    ChessPanel *chessPanel;
    bool initColor;
    bool isServer;
    QTcpServer *tcpServer;
    QTcpSocket *tcpClient;
    QTimer *timer;
    QTime timeBase;
};

#endif // CHESSSETTINGS_H
