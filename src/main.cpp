#include <QtGui/QApplication>

#include "mainwindow.h"  // MainWindow

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow mainWindow;
    mainWindow.init();
    mainWindow.show();

    return a.exec();
}
