/**
 * @file mapwidget.cpp
 * @author xstrna14
 * @brief TODO
 * Additional description TODO
 */

#include "mapwidget.h"
#include "mainwindow.h"
#include "connectionHandler.h"

MapWidget::MapWidget(QWidget *parent) : QWidget(parent)
{
    streets = new AllStreets();
    streets->loadStreets();

    //TODO actual solution
    streetNamesToggled = false;
    streetTimeToggled = false;
    streetIdToggled = false;

    conHandler = new connectionHandler;
    conHandler->loadConnections(streets->street_list);
    internalClock = new QTimer(this);
    connect(internalClock, &QTimer::timeout, conHandler, &connectionHandler::printConnections);
    internalClock->start(1000);
    updateClock = new QTimer(this);
    connect(updateClock, &QTimer::timeout, conHandler, &connectionHandler::printConnections);
    updateClock->start(20000);
}

MapWidget::~MapWidget()
{
    delete streets;
    delete conHandler;
    delete internalClock;
    delete updateClock;
}


void MapWidget::onToggleStreetNames(bool val)
{
    if(val) this->streetNamesToggled = true;
    else this->streetNamesToggled = false;
    update();
}

void MapWidget::onToggleStreetId(bool val)
{
    if(val) this->streetIdToggled = true;
    else this->streetIdToggled = false;
    update();
}

void MapWidget::onToggleStreetTime(bool val)
{
    if(val) this->streetTimeToggled = true;
    else this->streetTimeToggled = false;
    update();
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    //getting default pen
    QPen p_pen = p.pen();
    //getting default font
    QFont p_font = p.font();


    p.setWindow(QRect(0,0,100,100));

    /* TODO figure out
    float scl = 1;
    p.scale(scl,scl);
    int trans = 0;
    p.translate(trans, trans);
    //p.translate(50,50);
    int off = 40;
    int start = 0 + off;
    int end = 100 - off;
    p.setWindow(50, 50, 100, 100);*/

    //Map outline
    p.drawLine(0,0,100,0);
    p.drawLine(0,0,0,100);
    p.drawLine(100,0,100,100);
    p.drawLine(0,100,100,100);

    //Formating for street painting
    QPen street_pen = p_pen;
    street_pen.setCapStyle(Qt::RoundCap);
    street_pen.setWidth(3);
    street_pen.setBrush(Qt::gray);
    p.setPen(street_pen);

    //Street painting (just lines)
    for(auto const & s : this->streets->street_list) {
        p.drawLine(s->x1, s->y1, s->x2, s->y2);
    }

    // THis part handles radio buttons about displaying street properties
    if(this->streetNamesToggled || this->streetTimeToggled || this->streetIdToggled) {
        //setting drawing properties for writing street properties (where there are)
        QPen s_name_pen = p_pen;
        QFont s_name_font = p_font;
        s_name_font.setPointSize(2);

        p.setFont(s_name_font);
        p.setPen(s_name_pen);

        //if it should display name
        if(this->streetNamesToggled) {
            //Painting street names
            for(auto const & s : this->streets->street_list) {
                p.drawText(((s->x1+s->x2)/2)-3, ((s->y1+s->y2)/2)+1, s->name);
            }
        }
        //if it should display id
        else if(this->streetIdToggled) {
            for(auto const & s : this->streets->street_list) {
                p.drawText(((s->x1+s->x2)/2)-1, ((s->y1+s->y2)/2)+1, QString::number(s->id));
            }
        }
        //if it should display time to go through street
        else if(this->streetTimeToggled) {
            for(auto const & s : this->streets->street_list) {
                p.drawText(((s->x1+s->x2)/2)-1, ((s->y1+s->y2)/2)+1, QString::number(s->time));
            }
        }
     }
}


/* @brief Override resizeEvent to ensure the map widget is square after resizing main window.
 */
void MapWidget::resizeEvent(QResizeEvent *event)
{
    int nWidth = width();
    int nHeight = height();
    if(nWidth < nHeight) {
        nHeight = nWidth;
    }
    else nWidth = nHeight;
    this->resize(nWidth, nHeight);
    update();
    QWidget::resizeEvent(event);
}
