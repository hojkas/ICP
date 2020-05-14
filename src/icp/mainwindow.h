/**
 * @file mainwindow.h
 * @author TODO
 * @brief TODO
 * Additional description TODO
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:
    void onResizeForConnectionInfo(bool);
};

#endif // MAINWINDOW_H
