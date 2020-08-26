#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{
    tcpServer=new QTcpServer(this);
    init=true;
}

void TcpServer::startServer(int port)
{
    tcpServer->setMaxPendingConnections(2);
    tcpServer->listen(QHostAddress::Any,port);
    qDebug()<<"Server listening...";
    connect(tcpServer,&QTcpServer::newConnection,this,&TcpServer::newConnection);
}

TcpServer::~TcpServer()
{
}

void TcpServer::newConnection()
{
    QTcpSocket* tcpClient = tcpServer->nextPendingConnection();
    tcpClients.push_back(tcpClient);

    qDebug()<<ClientInfo(tcpClient)<<" connected!";

    connect(tcpClient,&QTcpSocket::readyRead,this,&TcpServer::readData);
    connect(tcpClient,&QTcpSocket::disconnected,this,&TcpServer::disConnection);

    if(tcpClients.size()==2){
        tcpClients[1]->write((char*)&init_pdata,sizeof(init_pdata));
    }
}

QString TcpServer::ClientInfo(QTcpSocket* tcpClient)
{
    QString str=QString("%1:%2")
            .arg(tcpClient->peerAddress().toString())
            .arg(tcpClient->peerPort());
    return str;
}

void TcpServer::disConnection()
{
    for(int i=0; i<tcpClients.size(); ++i){
        if(tcpClients[i]){
            qDebug()<<ClientInfo(tcpClients[i])<<" disconnected!.";
            tcpClients[i]->disconnectFromHost();
        }
    }
    tcpClients.clear();
    init=true;
}
// bad
void TcpServer::readData()
{
    if(tcpClients[0]->bytesAvailable()){
        qDebug()<<"read from "<<ClientInfo(tcpClients[0]);
        tcpClients[0]->read((char*)&pdata,sizeof(packet_data));
        if(init){
            init_pdata=pdata;
        }
        if(tcpClients.size()==2){
            qDebug()<<"write to "<<ClientInfo(tcpClients[1]);
            tcpClients[1]->write((char*)&pdata,sizeof(pdata));
        }
    }else if(tcpClients.size()==2 && tcpClients[1]->bytesAvailable()){
        qDebug()<<"read from "<<ClientInfo(tcpClients[1]);
        tcpClients[1]->read((char*)&pdata,sizeof(packet_data));
        if(init){
            init=false;
        }
        else{
            qDebug()<<"write to "<<ClientInfo(tcpClients[0]);
            tcpClients[0]->write((char*)&pdata,sizeof(pdata));
        }
    }
}










