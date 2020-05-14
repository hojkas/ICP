/**
 * @file mainwindow.cpp
 * @author xstrna14
 * @brief TODO
 * Additional description TODO
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //line to hide finish button on setup
    ui->FinishButton->hide();
    ui->DecIncFrame->setVisible(false);
    ui->extraInfo->setVisible(false);
    ui->OpenAllButton->setVisible(false);

    //default disable of map pan buttons
    ui->mapDown->setEnabled(false);
    ui->mapUp->setEnabled(false);
    ui->mapLeft->setEnabled(false);
    ui->mapRight->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onResizeForConnectionInfo(bool val) {
    if(val) resize(width() + 206, height());
    else resize(width() - 206, height());
    //206 -> 200 size of extra info layout + 6 as spacing in curent layout
    qDebug() << "here";
    update();
}
