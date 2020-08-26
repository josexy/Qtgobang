#include <QCoreApplication>
#include "tcpserver.h"
#include <signal.h>

TcpServer *tcpServer=nullptr;

void handler(int){
    qDebug()<<"Exit tcp server...";
    if(tcpServer){
        delete tcpServer;
        tcpServer=nullptr;
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug()<<"Start tcp server...";
    signal(SIGINT,handler);

    tcpServer=new TcpServer;
    tcpServer->startServer(6666);


    return a.exec();
}
