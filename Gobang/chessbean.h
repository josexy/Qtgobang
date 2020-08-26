#ifndef CHESSBEAN_H
#define CHESSBEAN_H

#include <QPoint>
#include "gobangConst.h"
#include <QDebug>

class ChessBean
{

public:
    ChessBean();
    ChessBean(int color,int orderNumber,const QPoint&pos);

    int getColor()const;
    void setColor(int color);

    int getOrderNumber() const;
    void setOrderNumber(int value);

    const QPoint& getPos() const;
    void setPos(const QPoint &value);

    int getOffense() const;
    void setOffense(int value);

    int getDefense() const;
    void setDefense(int value);

    int getEvaluScore() const;
    void setEvaluScore(int value);

    void copyFrom(const ChessBean&);
    friend QDebug operator<<(QDebug out,const ChessBean&cb);
    friend QDebug operator<<(QDebug out,ChessBean*cb);

    QString toString() const;
private:
    // 棋子坐标[0->14,0->14]
    QPoint pos;
    // 下棋顺序
    int orderNumber;
    // 棋子颜色,黑白
    int color;
    // 进攻得分
    int offense;
    // 防守得分
    int defense;
    // 总估值得分
    int evaluScore;
};

#endif // CHESSBEAN_H
