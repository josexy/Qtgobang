#ifndef INITDLG_H
#define INITDLG_H

#include <QWidget>
#include <QPaintEvent>
#include "mainwindow.h"
namespace Ui {
class InitDlg;
}

class InitDlg : public QWidget
{
    Q_OBJECT

public:
    explicit InitDlg(QWidget *parent = nullptr);
    ~InitDlg();
    void paintEvent(QPaintEvent *event);

private slots:
    void on_btnExit_clicked();
    void on_btn1_clicked();
    void on_btn2_clicked();
private:
    Ui::InitDlg *ui;
    MainWindow *w;
};

#endif // INITDLG_H
