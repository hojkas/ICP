/**
 * @file mapwidget.h
 * @author xstrna14, xlebod00
 * @brief Class to handle all communication with Map widget and its display.
 * Class emits signals to UI to hide/display information, overrides paint and mouse events
 * and accepts many signals from UI and reacts to them.
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
#include "math.h"


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
    bool mouseDrag;
    int dragX;
    int dragY;
public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();
    AllStreets* streets;
    connectionHandler* conHandler;

protected:
    //Overrides of mouse or paint events.
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

signals:
    //All signals are used to signal UI what to display/hide.
    void TimerMessage(QString);             /**< Sends QString text to display for simulation time. */
    void ErrorMessage(QString);             /**< Sends QString text to display "Event Log" */
    void hideFinishButton();                /**< Hides Finish Closure button. */
    void showFinishButton();                /**< Shows Finish Closure button. */
    void showModifyTrafficOptions(bool);    /**< If bool is true, shows options for Traffic editor, hides them if false. */
    void showOpenAllOption(bool);           /**< If bool is true, shows button to open all closures, hides it otherwise. */

    void showConnectionInfo(bool);          /**< If bool is true, shows panel with information about clicked connection, hides if false. */
    void conName(QString);                  /**< Sends QString into connection name display. */
    void conGenTime(QString);               /**< Sends QString into connection generated time display. */
    void conGenTT(QString);                 /**< Sends QString into connection General Timetable display. */
    void conCurTT(QString);                 /**< Sends QString into connection Current Timetable display. */
    void conBuses(QString);                 /**< Sends QString into connection Buses Departure display. */

    //map zoom/pan symbols signals
    void adjustMapZoom(int);                /**< Sends new value to map zoom slider.*/
    void allowMapMoveRight(bool);           /**< Bool decides whether move move right button is clickable or not.*/
    void allowMapMoveLeft(bool);            /**< Bool decides whether move move left button is clickable or not.*/
    void allowMapMoveUp(bool);              /**< Bool decides whether move move up button is clickable or not.*/
    void allowMapMoveDown(bool);            /**< Bool decides whether move move down button is clickable or not.*/

    //quickguide selection
    void show1(bool);                       /**< If true, shows panel #1 in quickguide. Hides it if false.*/
    void show0(bool);                       /**< If true, shows panel 0 in quickguide. Hides it if false.*/
    void show2(bool);                       /**< If true, shows panel #2 in quickguide. Hides it if false.*/
    void show3(bool);                       /**< If true, shows panel #3 in quickguide. Hides it if false.*/
    void show4(bool);                       /**< If true, shows panel #4 in quickguide. Hides it if false.*/

public slots:
    //All slots accept signals from UI and decide how the map and its displayed contents are affected by them.
    void onQuickguideSelection(int);
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
    void onResetAllButtonPress();
    void onModifyClosedFinish();

    //map zoom/pan slots
    void onMapZoomChange(int);
    void onMapMoveRight();
    void onMapMoveLeft();
    void onMapMoveUp();
    void onMapMoveDown();
};

#endif // MAPWIDGET_H
