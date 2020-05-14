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

    //hide quickguide and some members
    ui->Time->setVisible(false);
    ui->MoveZoom->setVisible(false);
    ui->TrafficEdit->setVisible(false);
    ui->ClosedEdit->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}
