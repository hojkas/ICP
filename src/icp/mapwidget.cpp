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

    //default values
    streetNamesToggled = false;
    streetTimeToggled = false;
    streetIdToggled = false;
    streetColorTime = false;
    streetColorTraffic = false;
    modeModifyClosed = false;
    modeModifyTraffic = false;
    timeModifier = 0;

    conHandler = new connectionHandler;
    conHandler->loadConnections(streets->street_list);
    conHandler->currentTime.setHMS(0,0,0);
    internalClock = new QTimer(this);
    connect(internalClock, &QTimer::timeout, conHandler, &connectionHandler::busUpdate);
<<<<<<< HEAD
    internalClock->start(2000);
    //connect(conHandler, &connectionHandler::busUpdated, conHandler, &connectionHandler::printConnections);
=======
    internalClock->start(500);
    connect(conHandler, &connectionHandler::busUpdated, conHandler, &connectionHandler::printBuses);
    connect(conHandler,&connectionHandler::busUpdated, this, QOverload<>::of(&MapWidget::update));
>>>>>>> master
}

MapWidget::~MapWidget()
{
    delete streets;
    delete conHandler;
    //delete internalClock;
    //delete updateClock;
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

void MapWidget::onToggleColorTime(bool val)
{
    if(val) this->streetColorTime = true;
    else this->streetColorTime = false;
    update();
}

void MapWidget::onToggleColorTraffic(bool val)
{
    if(val) this->streetColorTraffic = true;
    else this->streetColorTraffic = false;
    update();
}

void MapWidget::onToggleModifyClosed(bool val)
{
    if(val) modeModifyClosed = true;
    else modeModifyClosed = false;
    update();
}

void MapWidget::onToggleModifyTraffic(bool val)
{
    if(val) modeModifyTraffic = true;
    else modeModifyTraffic = false;
    update();
}

void MapWidget::onTimeSliderChange(int val)
{
    if(val == 0) timeModifier = 1;
    if(val == 1) timeModifier = 2;
    if(val == 2) timeModifier = 3;
    if(val == 3) timeModifier = 5;
    if(val == 4) timeModifier = 10;
    if(val == 5) timeModifier = 20;
    if(val == 6) timeModifier = 30;
    if(val == 7) timeModifier = 40;
    if(val == 8) timeModifier = 50;
    if(val == 9) timeModifier = 75;
    if(val == 10) timeModifier = 100;

    //resets internal clock with modified value
    qDebug() << timeModifier;
    int clock_time = 30000 / timeModifier;
    internalClock->start(clock_time);
    update();
}

void MapWidget::createTimerMessage()
{
    QString msg = "";
    msg.append("Time modifier: ");
    msg.append(QString::number(this->timeModifier));
    msg.append("\n");
    msg.append("Current time:                  ");
    msg.append(QString::number(conHandler->currentTime.hour()).rightJustified(2, '0'));
    msg.append(":");
    msg.append(QString::number(conHandler->currentTime.minute()).rightJustified(2, '0'));
    msg.append(":");
    msg.append(QString::number(conHandler->currentTime.second()).rightJustified(2, '0'));
    emit TimerMessage(msg);
}


void MapWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    //getting default pen
    QPen p_pen = p.pen();
    //getting default font
    QFont p_font = p.font();


    p.setWindow(QRect(0,0,100,100));
    //refreshes time message
    createTimerMessage();

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

    //This part handles color of streets
    //Formating for street painting
    QPen street_pen = p_pen;
    street_pen.setCapStyle(Qt::RoundCap);
    street_pen.setWidth(3);
    street_pen.setBrush(Qt::gray);

    if(streetColorTime) {
        //create pens of different colors for different time tiers
        QPen tier1 = street_pen;
        tier1.setBrush(QColor(135,201,232));
        QPen tier2 = street_pen;
        tier2.setBrush(QColor(91,173,212));
        QPen tier3 = street_pen;
        tier3.setBrush(QColor(36,142,191));
        QPen tier4 = street_pen;
        tier4.setBrush(QColor(3,111,161));
        QPen tier5 = street_pen;
        tier5.setBrush(QColor(2,60,87));

        //loop to draw streets
        for(auto const & s : this->streets->street_list) {
            if(s->count_time() < 150) {
                p.setPen(tier1);
            }
            else if(s->count_time() < 250) {
                p.setPen(tier2);
            }
            else if(s->count_time() < 350) {
                p.setPen(tier3);
            }
            else if(s->count_time() < 450) {
                p.setPen(tier4);
            }
            else {
                p.setPen(tier5);
            }
            p.drawLine(s->x1, s->y1, s->x2, s->y2);
        }
    }
    else if(streetColorTraffic) {
        //Setting up pens to draw traffic tiers
        QPen tier1 = street_pen;
        tier1.setBrush(QColor(103,222,29));
        QPen tier2 = street_pen;
        tier2.setBrush(QColor(242, 232,46));
        QPen tier3 = street_pen;
        tier3.setBrush(QColor(242,183,46));
        QPen tier4 = street_pen;
        tier4.setBrush(QColor(230,43,14));

        //loop to draw streets
        for(auto const & s : this->streets->street_list) {
            if(s->traffic == 1) {
                p.setPen(tier1);
            }
            else if(s->traffic == 2) {
                p.setPen(tier2);
            }
            else if(s->traffic == 3) {
                p.setPen(tier3);
            }
            else{
                p.setPen(tier4);
            }
            p.drawLine(s->x1, s->y1, s->x2, s->y2);
        }
    }
    else {
        p.setPen(street_pen);
        //Street painting grey (just grey lines, default)
        for(auto const & s : this->streets->street_list) {
            p.drawLine(s->x1, s->y1, s->x2, s->y2);
        }
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

    //This part handles painting current position of all buses
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);

    //Drawing of curr position of buses
    for(busElem* bus : this->conHandler->busList){
        if(bus->onMap) p.drawEllipse(QPoint(bus->x, bus->y), 2, 2);
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

/* @brief Override for capturing all mouse clicking inside widget.
 */
void MapWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "clicked at" << event->globalX() << ":" << event->globalY();
}
