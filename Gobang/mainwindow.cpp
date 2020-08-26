#include "mainwindow.h"
#include "initdlg.h"

MainWindow::MainWindow(int _mode, QWidget *parent)
    : QWidget(parent), gameMode(_mode)
{
    setWindowTitle("五子棋");

    setGeometry(100,100,WINDOW_WIDTH,WINDOW_HEIGHT);

    hBoxLayout=new QHBoxLayout(this);
    chessPanel=new ChessPanel(gameMode,this);

    chessSettingsPanel=new ChessSettings(gameMode,this,chessPanel);

    hBoxLayout->addWidget(chessPanel);
    hBoxLayout->addWidget(chessSettingsPanel);
    hBoxLayout->setStretchFactor(chessPanel,3);
    hBoxLayout->setStretchFactor(chessSettingsPanel,1);
    setLayout(hBoxLayout);
}

MainWindow::~MainWindow()
{
    delete hBoxLayout;
    delete chessPanel;
    delete chessSettingsPanel;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //
    chessSettingsPanel->safeExit=false;
    event->accept();
}
