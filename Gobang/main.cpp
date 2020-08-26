#include <QApplication>
#include "initdlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InitDlg w;
    w.show();
    return a.exec();
}
