#include "mainwindow.h"
#include "ftpgetwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //FtpgetWindow w;
    w.show();

    return a.exec();
}
