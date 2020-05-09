/**
 * @file main.cpp
 * @author TODO
 * @brief TODO
 * Additional description TODO
 */

#include "mainwindow.h"
#include "connectionHandler.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    connectionHandler conHandler;
    conHandler.loadConnections();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
