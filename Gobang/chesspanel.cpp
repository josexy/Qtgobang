#include "chesspanel.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>

ChessPanel::ChessPanel(int gamemode,QWidget*parent)
    :QWidget(parent),isGameOver(false),isNewGame(false),gameMode(gamemode),tcpClient(nullptr),
      bellChessHit(":/my/res/chessHit.wav"),
      bellWin(":/my/res/win.wav"),
      bellFail(":/my/res/fail.wav")
{
    setGeometry(0,0,PANEL_SIZE,PANEL_SIZE);
    setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
    currentPos.setX(-1);
    currentPos.setY(-1);

    setMouseTracking(true);

    imgChessBlack.load(":/my/res/black.png","PNG");
    imgChessWhite.load(":/my/res/white.png","PNG");

    // 初始化棋盘
    for (int x = 0; x < LINE_NUMBER; ++x) {
        vector<ChessBean*>t;
        for (int y = 0; y < LINE_NUMBER; ++y) {
            t.push_back(new ChessBean(EMPTY,0,QPoint(x,y)));
        }
        chessBox.push_back(t);
    }
    isEnableSound=true;
    isShowOrderNum=false;
    personFirst=true;
    currentPlayer=BLACK;
    count=0;
}

ChessPanel::~ChessPanel()
{
    for (int x=0; x<LINE_NUMBER; x++){
        for (int y=0; y<LINE_NUMBER; y++){
            if (chessBox[x][y]){
                delete chessBox[x][y];
                chessBox[x][y]=nullptr;
            }
        }
    }
}

/**
 * @brief 悔棋
 */
void ChessPanel::regretChess()
{
    if(isGameOver){
        QMessageBox::warning(this,"警告","当前游戏已经结束!");
        return;
    }
    if(qMaxOrderN.empty()){
        QMessageBox::warning(this,"警告","已经无棋可悔!");
        return;
    }
    for (int i = 0; i < 2; ++i) {
        ChessBean* cb=qMaxOrderN.top();
        int x=cb->getPos().x();
        int y=cb->getPos().y();
        chessBox[x][y]->setColor(EMPTY);
        chessBox[x][y]->setOrderNumber(0);
        count--;
        qMaxOrderN.pop();
    }
    update();
}

/**
 * @brief 新游戏
 */
void ChessPanel::newGame()
{
    tcpClient=nullptr;
    currentPlayer=BLACK;
    count=0;
    isNewGame=true;
    isGameOver=false;
    if(gameMode==1){
        pausePlayer=false;
    }else{
        pausePlayer=true;
    }
    qMinWinPos.clear();
    while(!qMaxOrderN.empty()){
        qMaxOrderN.pop();
    }
    for (int y = 0; y < LINE_NUMBER; ++y) {
        for (int x = 0; x < LINE_NUMBER; ++x) {
            if(chessBox[x][y]){
                chessBox[x][y]->setColor(EMPTY);
                chessBox[x][y]->setOrderNumber(0);
            }
        }
    }
    // 机器先手
    if(gameMode ==1 && !personFirst){
        count++;
        chessBox[LINE_NUMBER/2][LINE_NUMBER/2]->setColor(BLACK);
        chessBox[LINE_NUMBER/2][LINE_NUMBER/2]->setOrderNumber(count);
        currentPlayer=3-currentPlayer;
    }
}

/**
 * @brief 绘制棋盘线条
 * @param p
 */
void ChessPanel::drawLines(QPainter& p)
{
    p.setPen(QPen(QBrush(Qt::black),2));
    for (int i = 0; i < LINE_NUMBER; ++i) {
        p.drawLine(OFFSET,OFFSET+i*LINE_SPACING,OFFSET+LINE_LEN,OFFSET+i*LINE_SPACING);
        p.drawLine(OFFSET+i*LINE_SPACING,OFFSET,OFFSET+i*LINE_SPACING,OFFSET+LINE_LEN);
    }
}
/**
 * @brief 绘制棋盘周围的数字和字母
 * @param p
 */
void ChessPanel::drawNumber(QPainter&p)
{
    p.setPen(Qt::white) ;
    QFontMetrics f=p.fontMetrics();
    for (int i = 0; i < LINE_NUMBER; ++i) {
        QString s=QString::number(LINE_NUMBER-i);
        int w=f.horizontalAdvance(s);
        int h=f.height();
        // 绘制数字
        p.drawText(10,OFFSET+i*LINE_SPACING+h/2,s);
        s=QString(char('A'+i));
        w=f.horizontalAdvance(s);
        // 绘制字母
        p.drawText(OFFSET+i*LINE_SPACING-w/2,OFFSET+LINE_LEN+2*h,s);
    }
}
/**
 * @brief 绘制天元和四星
 * @param p
 */
void ChessPanel::drawStar(QPainter&p)
{
    p.setPen(Qt::PenStyle::NoPen);
    p.setBrush(QBrush(Qt::black));
    // 四星和天元出现的坐标
    int offset[][2]={{3,3},{11,3},{3,11},{11,11},{7,7}};
    for (int i = 0; i < 5; ++i) {
        QRect r(OFFSET+offset[i][0]*LINE_SPACING-STAR_SIZE/2,
                OFFSET+offset[i][1]*LINE_SPACING-STAR_SIZE/2,
                STAR_SIZE,STAR_SIZE);
        // 绘制圆形，左上角坐标和圆形宽高，圆形在一个矩形中...
        p.drawEllipse(r);
    }
}

/**
 * @brief 绘制显示顺序
 * @param p
 */
void ChessPanel::drawOrderNumber(QPainter&p)
{
    if(!isShowOrderNum)
        return;
    p.setPen(QPen(Qt::red,16));
    for (int y = 0; y < LINE_NUMBER; ++y) {
        for (int x = 0; x < LINE_NUMBER; ++x) {
            if(chessBox[x][y]->getColor()!=EMPTY){
                ChessBean *cb= chessBox[x][y];
                int x=realPos(cb->getPos()).x();
                int y=realPos(cb->getPos()).y();
                QString s=QString::number(cb->getOrderNumber());
                QFontMetrics fm=p.fontMetrics();
                int w=fm.horizontalAdvance(s);
                int h=fm.height();
                x-=w/2;
                y+=h/4;
                p.drawText(x,y,s);
            }
        }
    }
}

/**
 * @brief 绘制提示框
 * @param p
 */
void ChessPanel::drawTrips(QPainter&p)
{
    if(!checkBound(currentPos))
        return;

    p.setPen(QPen(QBrush(Qt::red),2));

    int x=OFFSET+LINE_SPACING*currentPos.x();
    int y=OFFSET+LINE_SPACING*currentPos.y();
    int centralPos[][2]={
        {x-CELL_SIZE/2,y-CELL_SIZE/2},
        {x+CELL_SIZE/2,y-CELL_SIZE/2},
        {x+CELL_SIZE/2,y+CELL_SIZE/2},
        {x-CELL_SIZE/2,y+CELL_SIZE/2},
    };
    int q=CELL_SIZE/4;
    int offset[][4]={
        {q,0,0,q},
        {-q,0,0,q},
        {-q,0,0,-q},
        {q,0,0,-q}
    };
    // 绘制当前交叉点四周的提示框，上右下左，先横后竖
    for (int i = 0; i < 4; ++i) {
        QPoint p1(centralPos[i][0],centralPos[i][1]);
        for (int j= 0; j < 4; j+=2) {
            QPoint p2(p1.x()+offset[i][j],p1.y()+offset[i][j+1]);
            p.drawLine(p1,p2);
        }
    }

}
/**
 * @brief 绘制棋子
 * @param p
 */
void ChessPanel::drawChess(QPainter&p)
{
    p.setPen(Qt::PenStyle::NoPen);
    // y
    for (int i = 0; i < LINE_NUMBER; ++i) {
        // x
        for (int j = 0; j < LINE_NUMBER; ++j) {
            // 该位置有黑白棋子
            if (chessBox[j][i]->getColor()!=EMPTY){
                QPoint pos=realPos(QPoint(j,i));
                int x=pos.x()-CHESS_SIZE/2;
                int y=pos.y()-CHESS_SIZE/2;
                int color=BLACK;
                if (chessBox[j][i]->getColor()==BLACK){
                    color=BLACK;
                    // p.setBrush(Qt::black);
                }else{
                    color=WHITE;
                    // p.setBrush(Qt::white);
                }
                // 绘制不同颜色的棋子
                if(color==BLACK)
                    p.drawPixmap(QRect(x,y,CHESS_SIZE,CHESS_SIZE),imgChessBlack,imgChessBlack.rect());
                else
                    p.drawPixmap(QRect(x,y,CHESS_SIZE,CHESS_SIZE),imgChessWhite,imgChessWhite.rect());
                // p.drawEllipse(x,y,CHESS_SIZE,CHESS_SIZE);
            }
        }
    }
    if(!isShowOrderNum){
        // 最后添加的棋子
        if(qMaxOrderN.empty())
            return;
        int Size=10;
        QPoint pos=qMaxOrderN.top()->getPos();
        pos=realPos(pos);
        // 绘制标记矩形
        p.fillRect(pos.x()-Size/2,pos.y()-Size/2,Size,Size,Qt::red);
    }
}

/**
 * @brief // 五子连珠是否是斜的 0 垂直，1 左斜，-1 右斜
 * @return
 */
int ChessPanel::isOblique(){
    QPoint from =qMinWinPos.front();
    QPoint to =qMinWinPos.back();
    int detX=to.x()-from.x();
    int detY=to.y()-from.y();
    if (detX==0||detY==0)
        return 0;
    if ((detX>0&&detY>0)||(detX<0&&detY<0))
        return 1;
    else
        return -1;
}
/**
 * @brief 绘制胜利后的五子连珠边框
 * @param p
 */
void ChessPanel::drawWinChessBorder(QPainter& p)
{
    if (qMinWinPos.empty())
         return;
    if (qMinWinPos.size()<5)
         return;
    // 排序 从小到大
    std::sort(qMinWinPos.begin(),qMinWinPos.end(),PosCompare());

    // 把多余的棋子去除，保证最后只有5个棋子
    if (qMinWinPos.size() > 5) {
        qMinWinPos.erase(qMinWinPos.begin()+5,qMinWinPos.end());
    }

    QPoint from=qMinWinPos.front();
    QPoint to=qMinWinPos.back();
    p.setBrush(QBrush(Qt::NoBrush));
    p.setPen(QPen(Qt::green,2));

    int cellSize=CELL_SIZE;
    int offset=OFFSET;
    int Y=isOblique();
    if (Y==0){
        int x=from.x()*cellSize+offset-cellSize/2;
        int y=from.y()*cellSize+offset-cellSize/2;
        int w=cellSize*(to.x()-from.x()+1);
        int h=cellSize*(to.y()-from.y()+1);
        // 绘制矩形
        p.drawRect(x,y,w,h);
    }else{
        int x[4];
        int y[4];
        x[0]=from.x()*cellSize-cellSize/2+offset;
        x[1]=from.x()*cellSize+offset;
        x[2]=to.x()*cellSize+cellSize/2+offset;
        x[3]=to.x()*cellSize+offset;

        y[0]=from.y()*cellSize+offset;
        y[1]=from.y()*cellSize-Y*cellSize/2+offset;
        y[2]=to.y()*cellSize+offset;
        y[3]=to.y()*cellSize+Y*cellSize/2+offset;
        QPoint points[4]={
            QPoint(x[0],y[0]),
            QPoint(x[1],y[1]),
            QPoint(x[2],y[2]),
            QPoint(x[3],y[3])
        };
        // 绘制多边形
        p.drawPolygon(points,4);
    }

}

/**
 * @brief 绘制窗口背景
 * @param p
 */
void ChessPanel::drawBackgroundImage(QPainter &p){
    QPixmap img;
    img.load(":/my/res/background.jpg","JPG");
    p.drawPixmap(QRect(0,0,geometry().width(),geometry().height()),img,img.rect());
}

void ChessPanel::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);

    drawBackgroundImage(painter);

    drawLines(painter);
    drawNumber(painter);
    drawStar(painter);
    drawTrips(painter);
    drawChess(painter);
    drawOrderNumber(painter);
    drawWinChessBorder(painter);

    QWidget::paintEvent(e);
}
/**
 * @brief 判断当前交叉点是否在棋盘区域内
 * @param p
 * @return
 */
bool ChessPanel::checkBound(const QPoint &p)
{
    if(p.x()>=0&&p.y()>=0&&p.x()<LINE_NUMBER&&p.y()<LINE_NUMBER){
        return true;
    }
    return false;
}
bool ChessPanel::checkBound(int x,int y)
{
    if(x>=0&&y>=0&&x<LINE_NUMBER&&y<LINE_NUMBER){
        return true;
    }
    return false;
}

/**
 * @brief 从当前交叉点坐标[0->14,0->14]计算出实际的坐标
 * @return
 */
const QPoint ChessPanel::realPos(const QPoint&p)
{
    int x=OFFSET+p.x()*LINE_SPACING;
    int y=OFFSET+p.y()*LINE_SPACING;
    return QPoint(x,y);
}

/**
 * @brief ChessPanel::check
 * @param x
 * @param y
 * @param offset
 * @param currentPlayer
 * @return
 */
int ChessPanel::check(int x,int y,int offset[2],int currentPlayer)
{
    int sum=0;
    // 从当前位置开始 向外依次检查每个位置是否有相同的棋子
    for (int i = 0; i < 4; ++i) {
        x+=offset[0];
        y+=offset[1];
        if(checkBound(QPoint(x,y))){
            if(chessBox[x][y]->getColor()==currentPlayer){
                qMinWinPos.push_back(QPoint(x,y));
                sum++;
            }else{
                break;
            }
        }
    }
    return sum;
}
/**
 * @brief 判断是否五子连珠
 * @param p
 * @param currentPlayer
 * @return
 */
bool ChessPanel::checkWin(int x,int y,int currentPlayer)
{
    // 偏移(x,y)，左:右，上:下，左上:右下，左下:右上
    int offset[][2]={
        {-1,0},{1,0},
        {0,-1},{0,1},
        {-1,-1},{1,1},
        {-1,1},{1,-1}
    };
    for (int i = 0; i < 8; i+=2) {
        qMinWinPos.clear();

        if(check(x,y,offset[i],currentPlayer) + check(x,y,offset[i+1],currentPlayer)>=4){
            qMinWinPos.push_back(QPoint(x,y));
            return true;
        }
    }
    return false;
}

void ChessPanel::mouseMoveEvent(QMouseEvent *event)
{
    // 判断当前鼠标位置是否处于棋盘区域
    if(event->pos().x()>=OFFSET&&event->pos().x()<OFFSET+LINE_LEN&&
            event->pos().y()>=OFFSET&&event->pos().y()<OFFSET+LINE_LEN){
        // 鼠标移动时计算得到棋盘交叉点坐标
        int x=(event->pos().x()-OFFSET+OFFSET/2)/LINE_SPACING;
        int y=(event->pos().y()-OFFSET+OFFSET/2)/LINE_SPACING;
        currentPos.setX(x);
        currentPos.setY(y);

        update();
    }
}

void ChessPanel::mousePressEvent(QMouseEvent *event)
{
    if(pausePlayer)
        return;
    if(event->button()!=Qt::MouseButton::LeftButton)
        return;
    if(!isNewGame)
        return;
    if(isGameOver)
        return;
    if(event->pos().x()>=OFFSET&&event->pos().x()<OFFSET+LINE_LEN&&
       event->pos().y()>=OFFSET&&event->pos().y()<OFFSET+LINE_LEN){
        // 鼠标移动时计算得到棋盘交叉点坐标
        int x=(event->pos().x()-OFFSET+OFFSET/2)/LINE_SPACING;
        int y=(event->pos().y()-OFFSET+OFFSET/2)/LINE_SPACING;
        bool isWin=false;
        // 空位置放置棋子
        if(chessBox[x][y]->getColor()==EMPTY){

            // 发送数据包
            if(gameMode == 2) {
                pd.type=CHESS_POINT;
                pd.restart=false;
                pd.color=currentPlayer;
                pd.x=x; pd.y=y;

                if(tcpClient && -1==tcpClient->write((char*)&pd,sizeof(packet_data))){
                    QMessageBox::information(this,"错误",QString("数据传输过程出现错误: %1")
                                             .arg(tcpClient->errorString()));
                    return;
                }
            }
            ++count;
            isWin=checkWin(x,y,currentPlayer);
            chessBox[x][y]->setColor(currentPlayer);
            chessBox[x][y]->setOrderNumber(count);
            qMaxOrderN.push(chessBox[x][y]);

            if(gameMode == 1) currentPlayer=3-currentPlayer;

            if(isEnableSound) bellChessHit.play();
            repaint();

            if(isWin){
                isGameOver=true;
                if(isEnableSound) bellWin.play();
                QMessageBox::information(this,"消息","游戏结束，玩家获取!");
                return;
            }
            pausePlayer=true;
        }else return;

        if(gameMode==1) {
            // 电脑放置棋子
            ABEval abv(chessBox,3,currentPlayer);
            abv.run();
            ChessBean *cb=abv.getChessBean();
            x=cb->getPos().x();
            y=cb->getPos().y();
            chessBox[x][y]->copyFrom(*cb);
            robotOperation(x,y,currentPlayer);
            currentPlayer=3-currentPlayer;
        }
    }
}

void ChessPanel::robotOperation(int x,int y,int color)
{
    pausePlayer=false;
    ++count;
    chessBox[x][y]->setColor(color);
    chessBox[x][y]->setOrderNumber(count);
    qMaxOrderN.push(chessBox[x][y]);

    if(isEnableSound) bellChessHit.play();
    repaint();

    bool isWin=checkWin(x,y,color);
    if(isWin){
        isGameOver=true;
        if(isEnableSound)bellFail.play();
        QMessageBox::information(this,"消息","游戏结束，玩家败北!");
        return;
    }
}
