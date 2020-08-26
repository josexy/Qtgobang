#ifndef CHESSPANEL_H
#define CHESSPANEL_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QTextEdit>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtMultimedia/QSound>
#include "gobangConst.h"
#include "patternText.h"
#include "chessbean.h"
#include "abeval.h"

#include <queue>
#include <vector>

using std::vector;
using std::priority_queue;

class ChessPanel : public QWidget
{
    Q_OBJECT
public:
    friend class ChessSettings;
    ChessPanel(int gamemode,QWidget*parent=nullptr);
    ~ChessPanel();
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void drawLines(QPainter &p);
    void drawNumber(QPainter &p);
    void drawTrips(QPainter &p);
    void drawStar(QPainter &p);
    bool checkBound(const QPoint &p);
    void drawChess(QPainter &p);
    void drawOrderNumber(QPainter &p);
    void drawBackgroundImage(QPainter &p);
    void drawWinChessBorder(QPainter &p);

    const QPoint realPos(const QPoint&);
    void newGame();
    void regretChess();

    bool checkWin(int x,int y, int currentPlayer);
    int check(int x, int y, int *offset, int currentPlayer);
    int isOblique();
    QString chessPlayerString(int direction, int x, int y, int currentPlayer);
    bool checkBound(int x, int y);
    void robotOperation(int x, int y,int currentPlayer);
private:
    // 当前防止棋子的玩家/黑白双方
    int currentPlayer;
    // 当前棋盘上存在的棋子
    int count;
    // 是否显示顺序
    bool isShowOrderNum;
    // 是否开启音效
    bool isEnableSound;
    // 游戏是否结束
    bool isGameOver;
    // 游戏是否开始
    bool isNewGame;
    // 人类先手还是机器先手
    bool personFirst;
    //
    bool isServer;
    bool pausePlayer;
    // 游戏模式 1人机 ,2联网对战
    int gameMode;

    ChessBean curForchessBean;
    // 当前棋盘中的交叉点坐标
    QPoint currentPos;
    // 棋盘
    vector<vector<ChessBean*>> chessBox;

    priority_queue<ChessBean*,vector<ChessBean*>,OrderNumberCompare>qMaxOrderN;
    // 获胜时五个棋子的坐标
    vector<QPoint> qMinWinPos;

    QTcpSocket *tcpClient;
    packet_data pd;

    QPixmap imgChessBlack;
    QPixmap imgChessWhite;
    // 音效
    QSound bellChessHit;
    QSound bellWin;
    QSound bellFail;
};


#endif // CHESSPANEL_H
