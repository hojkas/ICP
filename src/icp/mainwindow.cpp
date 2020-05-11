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
}

MainWindow::~MainWindow()
{
    delete ui;
}
