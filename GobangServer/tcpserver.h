#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>


struct packet_data{
    bool restart;

    short type;
    short color;

    short x;
    short y;

    short len;
    char data[30];
};

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);

    void startServer(int port);

    ~TcpServer();

    QString ClientInfo(QTcpSocket *client);
public slots:
    void newConnection();
    void disConnection();
    void readData();
private:
    QTcpServer *tcpServer;
    QList<QTcpSocket *> tcpClients;

    bool init;
    packet_data init_pdata;
    packet_data pdata;

};

#endif // TCPSERVER_H
