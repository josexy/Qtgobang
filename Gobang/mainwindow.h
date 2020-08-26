#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>

#include "chesspanel.h"
#include "chesssettings.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(int _mode,QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
private:    
    int gameMode;
    QHBoxLayout *hBoxLayout;
    ChessPanel *chessPanel;
    ChessSettings *chessSettingsPanel;
};
#endif // MAINWINDOW_H
