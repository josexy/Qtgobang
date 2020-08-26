#include "chessbean.h"

ChessBean::ChessBean()
{
    this->orderNumber=0;
    this->pos=QPoint(-1,-1);
    this->color=EMPTY;
    this->offense=0;
    this->defense=0;
    this->evaluScore=0;
}

ChessBean::ChessBean(int color, int orderNumber, const QPoint &pos)
{
    this->offense=0;
    this->defense=0;
    this->evaluScore=0;
    this->orderNumber=orderNumber;
    this->pos=pos;
    this->color=color;
}

int ChessBean::getColor() const
{
    return color;
}

void ChessBean::setColor(int c)
{
    this->color=c;
}

int ChessBean::getOrderNumber() const
{
    return orderNumber;
}

void ChessBean::setOrderNumber(int value)
{
    orderNumber = value;
}

const QPoint &ChessBean::getPos() const
{
    return pos;
}

void ChessBean::setPos(const QPoint &value)
{
    pos = value;
}

int ChessBean::getOffense() const
{
    return offense;
}

void ChessBean::setOffense(int value)
{
    offense = value;
}

int ChessBean::getDefense() const
{
    return defense;
}

void ChessBean::setDefense(int value)
{
    defense = value;
}

int ChessBean::getEvaluScore() const
{
    return evaluScore;
}

void ChessBean::setEvaluScore(int value)
{
    evaluScore = value;
}

void ChessBean::copyFrom(const ChessBean &cb)
{
    this->offense=cb.offense;
    this->defense=cb.defense;
    this->evaluScore=cb.evaluScore;
}


QString ChessBean::toString()const
{
    QString s=QString("%1 Point(%2,%3) offense:%4 defense:%5 score:%6")
            .arg((color==BLACK?"黑子":"白子"))
            .arg(pos.x()).arg(pos.y())
            .arg(offense).arg(defense).arg(evaluScore);
    return s;
}
QDebug operator<<(QDebug out, const ChessBean &cb)
{
    out<<cb.toString()<<Qt::endl;
    return out;
}

QDebug operator<<(QDebug out, ChessBean *cb)
{
    out<<cb->toString()<<Qt::endl;
    return out;
}
