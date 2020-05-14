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
#include <QtGlobal>


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
    bool modeModifyTraffic;
    bool modeModifyClosed;
    bool modeModifyTrafficMode;
    bool modeModifyClosedAdd;
    int timeModifier;

    //closed streets edit mode variables
    bool chosingDetourStreets;
    Street* closedStreet;
    std::list<Street*> detourStreets;

    QTimer *internalClock;

    bool drawConnectionToggle;
    connectionElem * drawConnection;
    QColor selectedConnectionColor;
    QColor selectedBusColor;

    //functions
    void createTimerMessage();
    void collectConnectionInfo(connectionElem*);
    QString createTimeString(int);

    //paint functions
    void paintBuses(QPainter*);
    void paintStreets(QPainter*);
    void paintStreetInfo(QPainter*);
    void paintConnection(QPainter*);
    void paintCloseModeInfo(QPainter*);

    //mouse functions
    void mouseEventModifyTraffic(int x, int y);
    void mouseEventModifyClosed(int x, int y);
    void mouseEventNormal(int x, int y);
    Street* findClickedStreet(int x, int y);

    //zoom/pan
    int zoomLevel;
    int xPan;
    int yPan;
    void setMapPanButtons();
public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();
    AllStreets* streets;
    connectionHandler* conHandler;

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event);

signals:
    void TimerMessage(QString);
    void ErrorMessage(QString);
    void hideFinishButton();
    void showFinishButton();
    void showModifyTrafficOptions(bool);
    void showOpenAllOption(bool);

    void showConnectionInfo(bool);
    void conName(QString);
    void conGenTime(QString);
    void conGenTT(QString);
    void conCurTT(QString);
    void conBuses(QString);
    void resizeForConnectionInfo(bool);

    //map zoom/pan symbols signals
    void adjustMapZoom(int);
    void allowMapMoveRight(bool);
    void allowMapMoveLeft(bool);
    void allowMapMoveUp(bool);
    void allowMapMoveDown(bool);

public slots:
    void onToggleStreetId(bool);
    void onToggleStreetNames(bool);
    void onToggleStreetTime(bool);

    void onToggleColorTime(bool);
    void onToggleColorTraffic(bool);

    void onToggleModifyTraffic(bool);
    void onToggleModifyTrafficMode(int);
    void onToggleModifyClosed(bool);

    void onTimeSliderChange(int);
    void onResetButtonPress();
    void onModifyClosedFinish();

    //map zoom/pan slots
    void onMapZoomChange(int);
    void onMapMoveRight();
    void onMapMoveLeft();
    void onMapMoveUp();
    void onMapMoveDown();
};

#endif // MAPWIDGET_H
