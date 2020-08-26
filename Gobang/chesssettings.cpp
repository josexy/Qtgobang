#include "chesssettings.h"
#include <QHostInfo>
#include <QMessageBox>
#include <QNetworkInterface>

ChessSettings::ChessSettings(int m, QWidget *parent,ChessPanel *cp)
    :QWidget(parent),safeExit(true),mode(m),chessPanel(cp),initColor(true)
{
    ui=new Ui::Form;
    ui->setupUi(this);

    srand(time(nullptr));

    resize(WINDOW_WIDTH-PANEL_SIZE,WINDOW_HEIGHT);

    tcpServer=nullptr;
    tcpClient=nullptr;
    if(mode==2){
        ui->btnRegretChess->setVisible(false);
        ui->groupBox_3->setVisible(false);
        ui->btnNewGame->setVisible(false);
        ui->btnRestartGame->setEnabled(false);
        ui->btnQuitConn->setEnabled(false);
    }else if(mode==1){
        ui->groupBox->setVisible(false);
    }
    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&ChessSettings::timeOut);
    ui->lcdNumber->display("00:00:00");
    ui->lcdNumber->setFixedSize(rect().width(),80);

    for (auto && addr : QNetworkInterface::allAddresses()){
        if(addr.toIPv4Address()||addr==QHostAddress::LocalHost){
            ui->txtIP->addItem(addr.toString());
        }
    }
    ui->txtIP->setCurrentIndex(0);
}

ChessSettings::~ChessSettings()
{
    delete ui;
}
// 计时
void ChessSettings::timeOut()
{
    int sec=timeBase.secsTo(QTime::currentTime());
    QTime showTime(0,0,0);
    showTime=showTime.addSecs(sec);
    QString str=showTime.toString("hh:mm:ss");
    ui->lcdNumber->display(str);
    if(chessPanel->isGameOver){
        timer->stop();
    }
}

// 悔棋
void ChessSettings::on_btnRegretChess_clicked()
{
    chessPanel->regretChess();
}
// 新游戏
void ChessSettings::on_btnNewGame_clicked()
{
    chessPanel->personFirst=ui->personFirst->isChecked();
    chessPanel->newGame();
    chessPanel->update();
    if(mode==1){
        QMessageBox::information(this,"消息","新游戏已经开始!");
        timeBase=QTime::currentTime();
        timer->start(1000);
    }
}
// 是否显示序号
void ChessSettings::on_cbShowOrderNum_stateChanged(int )
{
    chessPanel->isShowOrderNum=ui->cbShowOrderNum->isChecked();
    chessPanel->update();
}
// 是否开启音效
void ChessSettings::on_cbSound_stateChanged(int )
{
    chessPanel->isEnableSound=ui->cbSound->isChecked();
}
// 读取数据
void ChessSettings::readData()
{
    packet_data pdata;
    tcpClient->read((char*)&pdata,sizeof(packet_data));

    if(initColor){

        chessPanel->currentPlayer=pdata.color;
        ui->txtMsg->append(QString("当前为[%1]").arg((chessPanel->currentPlayer==BLACK?"黑子":"白子")));
        if(chessPanel->currentPlayer==BLACK){
            ui->txtMsg->append("黑子先行");
            chessPanel->pausePlayer=false;
        }
        initColor=false;
        return;
    }

    if(!pdata.restart){
        if(pdata.type==CHESS_POINT){
            chessPanel->robotOperation(pdata.x,pdata.y,pdata.color);
        }else if(pdata.type==CHESS_MSG){
            char buffer[31]{0};
            memcpy(buffer,pdata.data,pdata.len);
            ui->txtMsg->append(QString(">>> [%1]: %2").arg(pdata.color==BLACK?"黑方":"白方").arg(buffer));
        }
    }else{ // 重新开始
        chessPanel->isGameOver=true;

        ui->txtMsg->append("游戏已经重新开始");

        timeBase=QTime::currentTime();
        ui->lcdNumber->display("00:00:00");

        chessPanel->newGame();
        chessPanel->tcpClient=tcpClient;
        chessPanel->currentPlayer=rand()%2+1;
        ui->txtMsg->append(QString("当前为[%1]").arg((pdata.color==BLACK?"黑子":"白子")));

        if(chessPanel->currentPlayer==BLACK){
            ui->txtMsg->append("黑子先行");
            chessPanel->pausePlayer=false;
        }
        packet_data pd;
        pd.color=3-chessPanel->currentPlayer;
        tcpClient->write((char*)&pd,sizeof(packet_data));
        initColor=false;

        ui->btnRestartGame->setEnabled(true);
        chessPanel->update();
    }
}
// 断开连接
void ChessSettings::disConnection()
{
    if(safeExit){
        ui->txtMsg->append("断开连接");
        initColor=true;
        chessPanel->newGame();
        chessPanel->update();

        ui->radioLAN->setEnabled(true);
        ui->radioWAN->setEnabled(true);
        ui->btnServerListen->setEnabled(true);
        ui->btnClientConn->setEnabled(true);
        ui->btnRestartGame->setEnabled(false);
        ui->btnQuitConn->setEnabled(false);
        ui->lcdNumber->display("00:00:00");
        timer->stop();
    }
}

// 发送文本数据
void ChessSettings::on_btnSendMsg_clicked()
{
    if(!tcpClient)return;

    std::string text=ui->txtSendMsg->text().trimmed().toStdString();
    short text_size=text.size();
    if(text_size<=0) return;

    if(text_size>30){
        QMessageBox::information(this,"消息","发送文本内容过长!");
        return;
    }

    packet_data pd;
    pd.restart=false;
    pd.type=CHESS_MSG;
    pd.color=chessPanel->currentPlayer;
    pd.len=text_size;
    memcpy(pd.data,text.c_str(),text_size);
    if(-1==tcpClient->write((char*)&pd,sizeof(packet_data))){
        ui->txtMsg->append("Error: "+tcpClient->errorString());
        return;
    }

    ui->txtMsg->append(QString(">>> [%1]: %2")
                       .arg(chessPanel->currentPlayer==BLACK?"黑方":"白方")
                       .arg(text.c_str()));
}

// 服务器监听端口
void ChessSettings::on_btnServerListen_clicked()
{
    tcpServer=new QTcpServer(this);
    if(!tcpServer)return;

    tcpServer->setMaxPendingConnections(1);

    if(!tcpServer->listen(QHostAddress(ui->txtIP->currentText()),ui->txtPort->text().toInt())){
        QMessageBox::critical(this,"错误","服务端监听失败: "+tcpServer->errorString());
        delete tcpServer;
        tcpServer=nullptr;
        return ;
    }
    isServer=true;

    ui->txtMsg->append(QString("服务端监听成功"));

    ui->btnServerListen->setEnabled(false);
    ui->btnClientConn->setEnabled(false);
    ui->btnQuitConn->setEnabled(true);
    ui->radioLAN->setEnabled(false);
    ui->radioWAN->setEnabled(false);

    connect(tcpServer,&QTcpServer::newConnection,this,[&](){
        tcpClient=tcpServer->nextPendingConnection();

        chessPanel->newGame();
        chessPanel->currentPlayer=rand()%2+1;
        chessPanel->update();

        chessPanel->tcpClient=tcpClient;

        packet_data pd;
        pd.color=3-chessPanel->currentPlayer;
        tcpClient->write((char*)&pd,sizeof(packet_data));
        initColor=false;

        // 计时开始 /s
        timeBase=QTime::currentTime();
        timer->start(1000);

        ui->btnRestartGame->setEnabled(true);

        QString connText=QString("客户端连接成功");
        ui->txtMsg->append(connText);
        ui->txtMsg->append(QString("当前为[%1]").arg((chessPanel->currentPlayer==BLACK?"黑子":"白子")));

        if(chessPanel->currentPlayer==BLACK){
            ui->txtMsg->append("黑子先行");
            chessPanel->pausePlayer=false;
        }

        connect(tcpClient,&QTcpSocket::readyRead,this,&ChessSettings::readData);

        connect(tcpClient,&QTcpSocket::disconnected,this,&ChessSettings::disConnection);
    });
}
// 客户端连接
void ChessSettings::on_btnClientConn_clicked()
{
    tcpClient=new QTcpSocket(this);
    if(!tcpClient)return;
    tcpClient->connectToHost(ui->txtIP->currentText(),ui->txtPort->text().toInt());
    // 超时连接
    if(!tcpClient->waitForConnected(5000)){
        QMessageBox::critical(this,"错误","客户端连接失败: "+tcpClient->errorString());
        delete tcpClient;
        tcpClient=nullptr;
        return;
    }
    chessPanel->newGame();
    chessPanel->update();
    chessPanel->tcpClient=tcpClient;

    // 计时开始 /s
    timeBase=QTime::currentTime();
    timer->start(1000);

    QString connText=QString("客户端连接成功");
    ui->txtMsg->append(connText);

    connect(tcpClient,&QTcpSocket::readyRead,this,&ChessSettings::readData);

    connect(tcpClient,&QTcpSocket::disconnected,this,&ChessSettings::disConnection);

    //
    if(ui->radioWAN->isChecked()){
        chessPanel->currentPlayer=rand()%2+1;
        initColor=false;

        packet_data pd;
        pd.color=3-chessPanel->currentPlayer;
        tcpClient->write((char*)&pd,sizeof(packet_data));

        if(chessPanel->currentPlayer==BLACK){
            chessPanel->pausePlayer=false;
        }
    }

    isServer=false;
    ui->btnServerListen->setEnabled(false);
    ui->btnClientConn->setEnabled(false);
    ui->btnRestartGame->setEnabled(true);
    ui->btnQuitConn->setEnabled(true);
    ui->radioLAN->setEnabled(false);
    ui->radioWAN->setEnabled(false);
}

// 重新开始游戏
void ChessSettings::on_btnRestartGame_clicked()
{
    if(!tcpClient)return;
    packet_data pdata;
    pdata.restart=true;
    if(-1==tcpClient->write((char*)&pdata,sizeof(packet_data))){
        ui->txtMsg->append("Error: "+tcpClient->errorString());
        return;
    }
    ui->txtMsg->append("游戏已经重新开始");
    ui->btnRestartGame->setEnabled(true);

    timeBase=QTime::currentTime();
    timer->start(1000);
    ui->lcdNumber->display("00:00:00");

    chessPanel->newGame();
    chessPanel->tcpClient=tcpClient;
    initColor=true;
    chessPanel->update();
}

// 断开连接
void ChessSettings::on_btnQuitConn_clicked()
{
    if(tcpServer && isServer){
        if(tcpClient) {
            delete tcpClient;
            tcpClient=nullptr;
        }
        tcpServer->close();
        delete tcpServer;
        tcpServer=nullptr;
    }else if(tcpClient && !isServer){
        tcpClient->disconnectFromHost();
        delete tcpClient;
        tcpClient=nullptr;
    }
    initColor=true;
    chessPanel->newGame();
    chessPanel->update();

    ui->radioLAN->setEnabled(true);
    ui->radioWAN->setEnabled(true);
    ui->btnServerListen->setEnabled(true);
    ui->btnClientConn->setEnabled(true);
    ui->btnRestartGame->setEnabled(false);
    ui->btnQuitConn->setEnabled(false);
    ui->lcdNumber->display("00:00:00");
    timer->stop();
}

void ChessSettings::on_radioWAN_toggled(bool checked)
{
    if(!checked){
        ui->btnServerListen->setEnabled(true);
        for (auto && addr : QNetworkInterface::allAddresses())
            if(addr.toIPv4Address()||addr==QHostAddress::LocalHost)
                ui->txtIP->addItem(addr.toString());
    }else{
        ui->btnServerListen->setEnabled(false);
        ui->txtIP->clear();
    }
}
