#include "initdlg.h"
#include "ui_initdlg.h"

#include <QPainter>
#include <QScreen>
#include <QDebug>
#include "mainwindow.h"

InitDlg::InitDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InitDlg),w(nullptr)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    auto x=QApplication::screens();
    if(x.size()){
        QScreen *screen=x.first();
        QRect r=screen->availableGeometry();
        move((r.width()-rect().width())/2,(r.height()-rect().height())/2);
    }
}

InitDlg::~InitDlg()
{
    delete ui;
}

void InitDlg::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.drawImage(rect(),QImage(":/my/res/background.jpg"));
    QWidget::paintEvent(event);
}


void InitDlg::on_btnExit_clicked()
{
    qApp->quit();
}

void InitDlg::on_btn1_clicked()
{
    w=new MainWindow(1);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
    close();
}

void InitDlg::on_btn2_clicked()
{
    w=new MainWindow(2);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
    close();
}
