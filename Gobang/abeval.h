#ifndef ABEval_H
#define ABEval_H

#include <QObject>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include "chessbean.h"
#include "patternText.h"
#include <vector>
#include <memory>

//#include <regex.h>
//#include <boost/regex.hpp>

using std::shared_ptr;
using std::vector;
using std::max;

struct OrderNumberCompare{
    bool operator()(ChessBean*cb1,ChessBean *cb2)const{
        return cb1->getOrderNumber()<cb2->getOrderNumber();
    }
};
struct ScoreComp{
    bool operator()(ChessBean*cb1,ChessBean *cb2){
        return cb1->getEvaluScore()>cb2->getEvaluScore();
    }
};
struct PosCompare{
    bool operator()(const QPoint&p1,const QPoint&p2)const{
        if (p1.x()==p2.x()&&p1.y()==p2.y())return true;
        if (p1.x()<p2.x()&& p1.y()<p2.y())
            return true;
        else if (p1.x()<p2.x()&&p1.y()>p2.y()){
            return true;
        }else if (p1.x()==p2.x()&&p1.y()<p2.y()){
            return true;
        }else if (p1.x()<p2.x()&&p1.y()==p2.y()){
            return true;
        }else{
            return false;
        }
    }
};

class ABEval:public QObject
{
    Q_OBJECT
public:
    typedef vector<vector<ChessBean*>> cb_array;
    ABEval()=delete;
    ABEval(cb_array & chessBox,int depth,int currentPlayer);
    ~ABEval();
    void run();

    int ab1(cb_array &tp, int depth, int currentPlayer, int alpha, int beta, bool maximizingPlayer);
    int ab2(cb_array &tp, int depth, int currentPlayer, int alpha, int beta);

    std::vector<ChessBean*> evaluation(int currentPlayer, cb_array&tmpChessBox);
    int getEvaluationValue(int x, int y, int currentPlayer, cb_array&chessBox);
    bool checkBound(const QPoint &p);
    bool checkBound(int x, int y);
    QString chessPlayerString(int direction, int x, int y, int currentPlayer, cb_array&chessBox);
    ChessBean *getChessBean() const;
    std::pair<int,int>getLevel(int direction, int x,int y,int currentPlayer, cb_array &chessBox);
private:
    ChessBean *curForchessBean;
    cb_array tp;
    int depth;
    int currentPlayer;
};

#endif // ABEval_H
