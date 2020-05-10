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
#include <QMouseEvent>

#include "street.h"
#include "connectionHandler.h"

class MapWidget : public QWidget
{
    Q_OBJECT
private:
    bool streetNamesToggled;
    bool streetIdToggled;
    bool streetTimeToggled;
    bool streetColorTime;
    bool streetColorTraffic;

    QTimer *internalClock;
public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();
    AllStreets* streets;
    connectionHandler* conHandler;

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

signals:

public slots:
    void onToggleStreetId(bool);
    void onToggleStreetNames(bool);
    void onToggleStreetTime(bool);

    void onToggleColorTime(bool);
    void onToggleColorTraffic(bool);
};

#endif // MAPWIDGET_H
