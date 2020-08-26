#ifndef PATTERNTEXT_H
#define PATTERNTEXT_H

#include <QString>
#include <QVector>

struct PatternText{
    int index;
    int scroe;
    QVector<QString> patterns;
    PatternText(){}
    PatternText(int index,int score,const QVector<QString>&patterns){
        this->index=index;
        this->scroe=score;
        this->patterns=patterns;
    }
};

constexpr int PATTERN_SIZE=11;

// 1:黑棋, 2:白棋
const PatternText CON_5(0,100000,{"11111","22222"});     // 长连
const PatternText ALIVE_4(1,10000,{"011110", "022220"}); // 活四
const PatternText GO_4(2,500,{"011112|0101110|0110110", "022221|0202220|0220220"});   // 冲四
const PatternText DEAD_4(3,-5,{"211112", "122221"}); // 死四
const PatternText ALIVE_3(4,200,{"01110|010110", "02220|020220"}); // 活三
const PatternText SLEEP_3(5,50,{"001112|010112|011012|10011|10101|2011102",
                                "002221|020221|022021|20022|20202|1022201" }); // 眠三
const PatternText DEAD_3(6,-5,{ "21112", "12221"});  // 死三
const PatternText ALIVE_2(7,5,{"00110|01010|010010", "00220|02020|020020"}); // 活二
const PatternText SLEEP_2(8,3,{"000112|001012|010012|10001|2010102|2011002",
                               "000221|002021|020021|20002|1020201|1022001"}); // 眠二
const PatternText DEAD_2(9,-5,{"2112", "1221"});  // 死二
const PatternText NIL(10,0,{"",""});     // 空

const PatternText patternTexts[PATTERN_SIZE]={
    CON_5,ALIVE_4,GO_4,DEAD_4,ALIVE_3,SLEEP_3,DEAD_3,ALIVE_2,SLEEP_2,DEAD_2,NIL
};

#endif // PATTERNTEXT_H
