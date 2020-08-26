#include "abeval.h"

int POSITION_SCORE[LINE_NUMBER][LINE_NUMBER] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
    { 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0 },
    { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0 },
    { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 },
    { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 },
    { 0, 1, 2, 3, 4, 5, 6, 6, 6, 5, 4, 3, 2, 1, 0 },
    { 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0 },
    { 0, 1, 2, 3, 4, 5, 6, 6, 6, 5, 4, 3, 2, 1, 0 },
    { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0 },
    { 0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0 },
    { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0 },
    { 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0 },
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};


ABEval::ABEval(vector<vector<ChessBean*>>& chessBox,int depth, int player)
{
    this->depth=depth;
    this->currentPlayer=player;
    for(int x = 0; x < LINE_NUMBER ;x++){
        vector<ChessBean*>t;
        for(int y = 0; y < LINE_NUMBER ;y++){
            t.push_back(new ChessBean(*chessBox[x][y]));
        }
        tp.push_back(t);
    }
}

ABEval::~ABEval()
{
    for (int x=0; x<LINE_NUMBER; x++){
        for (int y=0; y<LINE_NUMBER; y++){
            if (tp[x][y]){
                delete tp[x][y];
                tp[x][y]=nullptr;
            }
        }
    }
}

bool ABEval::checkBound(const QPoint &p)
{
    if(p.x()>=0&&p.y()>=0&&p.x()<LINE_NUMBER&&p.y()<LINE_NUMBER){
        return true;
    }
    return false;
}
bool ABEval::checkBound(int x,int y)
{
    if(x>=0&&y>=0&&x<LINE_NUMBER&&y<LINE_NUMBER){
        return true;
    }
    return false;
}
QString ABEval::chessPlayerString(int direction,int x,int y, int currentPlayer, cb_array &chessBox)
{
    // 以当前坐标为中心，向8个方向遍历
    // 偏移(x,y)，左:右，上:下，左上:右下，左下:右上
    int offset[][2]={
        {-1,0},{1,0},
        {0,-1},{0,1},
        {-1,-1},{1,1},
        {-1,1},{1,-1}
    };
    int i,j;
    switch(direction){
    case HORIZONTAL:i=0,j=1;break;
    case VERTICAL:i=2,j=3;break;
    case LEFT_OBLIQUE:i=4,j=5;break;
    case RIGHT_OBLIQUE:i=6,j=7;break;
    };
    QString text,left,right;
    int x1,x2,y1,y2;
    x1=x2=x,y1=y2=y;
    char buf[5]{0};
    for (int k=0; k<5 ;k++){
        x1+=offset[i][0];
        y1+=offset[i][1];
        if(checkBound(x1,y1)){
            memset(buf,0,5);
            sprintf(buf,"%d",chessBox[x1][y1]->getColor());
            left.insert(0,buf);
        }
        x2+=offset[j][0];
        y2+=offset[j][1];
        if(checkBound(x2,y2)){
            memset(buf,0,5);
            sprintf(buf,"%d",chessBox[x2][y2]->getColor());
            right.append(buf);
        }
    }
    memset(buf,0,5);
    sprintf(buf,"%d",currentPlayer);
    text=left+buf+right;
    return text;
}

std::pair<int, int> ABEval::getLevel(int direction, int x, int y,int currentPlayer, cb_array &chessBox)
{
    QString text=chessPlayerString(direction,x,y,currentPlayer,chessBox);
    for (int i = 0; i < PATTERN_SIZE-1; ++i) {
        auto pt=patternTexts[i];
        QRegExp regex(pt.patterns[currentPlayer-1]);
        //  QRegularExpression regex(pt.patterns[currentPlayer-1]);
        //  regex_t rgx;
        //  regcomp(&rgx,pt.patterns[currentPlayer-1].toStdString().c_str(),0);
        //  boost::regex rgx(pt.patterns[currentPlayer-1].toStdString());

        QString rText=text;
        std::reverse(rText.begin(),rText.end());

        if(regex.indexIn(text)!=-1||regex.indexIn(rText)!=-1){
        //  if(boost::regex_search(text.toStdString(),rgx)||boost::regex_search(rText.toStdString(),rgx)){
        //  if((0==regexec(&rgx,text.toStdString().c_str(),0,0,0))||(0==regexec(&rgx,rText.toStdString().c_str(),0,0,0))){
        //  if(regex.match(text).hasMatch()||regex.match(rText).hasMatch()){
        //  regfree(&rgx);
            return std::make_pair(pt.index,pt.scroe);
        }
        //  regfree(&rgx);
    }
    return std::make_pair(NIL.index,NIL.scroe);
}

int ABEval::getEvaluationValue(int x,int y,int currentPlayer, cb_array &chessBox)
{
    auto horizontal=getLevel(HORIZONTAL,x,y,currentPlayer,chessBox);
    auto vertical=getLevel(VERTICAL,x,y,currentPlayer,chessBox);
    auto leftOblique=getLevel(LEFT_OBLIQUE,x,y,currentPlayer,chessBox);
    auto rightOblique=getLevel(RIGHT_OBLIQUE,x,y,currentPlayer,chessBox);

    int countSc[PATTERN_SIZE]{0};
    countSc[horizontal.first]++;
    countSc[vertical.first]++;
    countSc[leftOblique.first]++;
    countSc[rightOblique.first]++;

    int score=0;
    if(countSc[GO_4.index]>=2||(countSc[GO_4.index]>=1&&countSc[ALIVE_3.index]>=1)){// 双冲四 或者 冲四活三
        score=10000;
    }else if(countSc[ALIVE_3.index]>=2){// 双活三
        score=5000;
    }else if(countSc[ALIVE_3.index]>=1&&countSc[SLEEP_3.index]>=1){// 活三眠三
        score=1000;
    }else if(countSc[ALIVE_2.index]>=2){// 双活二
        score=100;
    }else if(countSc[ALIVE_2.index]>=1&&countSc[SLEEP_2.index]>=1){// 活二眠二
        score=10;
    }
    score=max(score,max(max(horizontal.second,vertical.second),max(leftOblique.second,rightOblique.second)));

    return score+POSITION_SCORE[x][y];
}

std::vector<ChessBean*> ABEval::evaluation(int currentPlayer, cb_array &tmpChessBox)
{
    std::vector<ChessBean*>q;
    // 遍历棋盘上每一个位置，找到一个最大值的估值
    for (int y = 0; y < LINE_NUMBER; ++y) {
        for (int x = 0; x < LINE_NUMBER; ++x) {
            // 空位置
            if(tmpChessBox[x][y]->getColor()==EMPTY){
                auto cb=tmpChessBox[x][y];
                // 攻
                int offense=getEvaluationValue(x,y,currentPlayer,tmpChessBox);
                // 守
                int defense=getEvaluationValue(x,y,3-currentPlayer,tmpChessBox);
                cb->setDefense(defense);
                cb->setOffense(offense);
                cb->setEvaluScore(defense+offense);
                q.push_back(cb);
            }
        }
    }
    std::sort(q.begin(),q.end(),ScoreComp());
    return q;
}

int ABEval::ab2(cb_array &tp,int _depth, int currentPlayer,int alpha,int beta)
{
    std::vector<ChessBean*>lstChess=evaluation(currentPlayer,tp);
    if (depth ==_depth){
        return lstChess.front()->getEvaluScore();
    }
    for (int i=0; i<4; ++i){
        ChessBean* cb=lstChess.at(i);
        int score;
        if (cb->getEvaluScore()>ALIVE_4.scroe){
            score=cb->getEvaluScore();
        }else{
            int x=cb->getPos().x();
            int y=cb->getPos().y();
            int old=tp[x][y]->getColor();
            tp[x][y]->setColor(currentPlayer);
            score=ab2(tp,_depth+1,3-currentPlayer,alpha,beta);
            tp[x][y]->setColor(old);
        }
        if (_depth %2==0){
            if (score>alpha){
                alpha=score;
                if (_depth==0){
                    curForchessBean=cb;
                }
            }
            if (alpha>=beta){
                score=alpha;
                return score;
            }
        }else{
            if (score<beta){
                beta=score;
            }
            if (alpha>=beta){
                score=beta;
                return score;
            }
        }
    }
    return _depth % 2 ==0?alpha:beta;
}

// bad
int ABEval::ab1(cb_array &tp,int depth,
                                int currentPlayer,int alpha,int beta,
                                bool maximizingPlayer)
{
    std::vector<ChessBean*>lstChess=evaluation(currentPlayer,tp);

    if (depth==0){
        curForchessBean=lstChess.front();
        return curForchessBean->getEvaluScore();
    }
    if(maximizingPlayer){
        int value=-std::numeric_limits<int>::max();
        for (int i = 0; i < 4; ++i) {
            ChessBean *chessBean=lstChess[i];
            int x=chessBean->getPos().x();
            int y=chessBean->getPos().y();
            if(chessBean->getEvaluScore()>ALIVE_4.scroe){
                curForchessBean=chessBean;
                value= curForchessBean->getEvaluScore();
                return value;
            }else{
                int old=tp[x][y]->getColor();
                tp[x][y]->setColor(currentPlayer);
                value=std::max(value,ab1(tp,depth-1,3-currentPlayer,alpha,beta,false));
                tp[x][y]->setColor(old);
            }
            alpha=std::max(alpha,value);
            if(beta<=alpha){
                break;
            }
        }
        return value;
    }else{
        int value=std::numeric_limits<int>::max();
        for (int i = 0; i < 4; ++i) {
            ChessBean * chessBean=lstChess[i];
            int x=chessBean->getPos().x();
            int y=chessBean->getPos().y();
            if(lstChess.at(i)->getEvaluScore()>ALIVE_4.scroe){
                curForchessBean=chessBean;
                value=curForchessBean->getEvaluScore();
                return value;
            }else{
                int old=tp[x][y]->getColor();
                tp[x][y]->setColor(currentPlayer);
                value=std::min(value,ab1(tp,depth-1,3-currentPlayer,alpha,beta,true));
                tp[x][y]->setColor(old);
            }
            beta=std::min(beta,value);
            if(beta<=alpha){
                break;
            }
        }
        return value;
    }
}
ChessBean* ABEval::getChessBean()const
{
    return this->curForchessBean;
}


void ABEval::run()
{
    auto x=QtConcurrent::run([&](){
        int n=std::numeric_limits<int>::max();
        ab2(tp,0,currentPlayer,-n,n);
    });
    x.waitForFinished();
}
