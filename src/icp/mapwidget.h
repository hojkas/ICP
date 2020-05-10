/**
 * @file mapwidget.h
 * @author xstrna14
 * @brief TODO
 * Additional description TODO
 */

#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QImage>
#include <QCheckBox>
#include <QTimer>

#include "street.h"
#include "connectionHandler.h"

class MapWidget : public QWidget
{
    Q_OBJECT
private:
    bool streetNamesToggled;
    bool streetIdToggled;
    bool streetTimeToggled;
    QTimer *internalClock;
public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();
    AllStreets* streets;
    connectionHandler* conHandler;

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

signals:

public slots:
    void onToggleStreetId(bool);
    void onToggleStreetNames(bool);
    void onToggleStreetTime(bool);
};

#endif // MAPWIDGET_H
