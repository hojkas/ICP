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
    modeModifyTrafficMode = false;
    timeModifier = 1;
    drawConnectionToggle = false;
    drawConnection = nullptr;

    conHandler = new connectionHandler;
    conHandler->loadConnections(streets->street_list);
    conHandler->currentTime.setHMS(0,0,0);
    internalClock = new QTimer(this);
    connect(internalClock, &QTimer::timeout, conHandler, &connectionHandler::busUpdate);
    internalClock->start(30000);
    connect(conHandler, &connectionHandler::busUpdated, conHandler, &connectionHandler::printBuses);
    connect(conHandler,&connectionHandler::busUpdated, this, QOverload<>::of(&MapWidget::update));
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

void MapWidget::onToggleModifyTrafficMode(int val)
{
    if(val) modeModifyTrafficMode = true;
    else modeModifyTrafficMode = false;
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


    paintStreets(&p);
    paintStreetInfo(&p);
    paintBuses(&p);
    if(drawConnectionToggle) paintConnection(&p);
}

/* @brief Help function for paintEvent to draw all streets in color according to flag info
 */
void MapWidget::paintStreets(QPainter* p)
{
    //This part handles color of streets
    //Formating for street painting
    QPen street_pen = QPen();
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
                p->setPen(tier1);
            }
            else if(s->count_time() < 250) {
                p->setPen(tier2);
            }
            else if(s->count_time() < 350) {
                p->setPen(tier3);
            }
            else if(s->count_time() < 450) {
                p->setPen(tier4);
            }
            else {
                p->setPen(tier5);
            }
            p->drawLine(s->x1, s->y1, s->x2, s->y2);
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
                p->setPen(tier1);
            }
            else if(s->traffic == 2) {
                p->setPen(tier2);
            }
            else if(s->traffic == 3) {
                p->setPen(tier3);
            }
            else{
                p->setPen(tier4);
            }
            p->drawLine(s->x1, s->y1, s->x2, s->y2);
        }
    }
    else {
        p->setPen(street_pen);
        //Street painting grey (just grey lines, default)
        for(auto const & s : this->streets->street_list) {
            p->drawLine(s->x1, s->y1, s->x2, s->y2);
        }
    }
}

/* @brief Help function for paintEvent to draw all street info if right bool flag is set
 */
void MapWidget::paintStreetInfo(QPainter* p)
{
    // THis part handles radio buttons about displaying street properties
    if(this->streetNamesToggled || this->streetTimeToggled || this->streetIdToggled) {
        //setting drawing properties for writing street properties (where there are)
        QFont s_name_font = QFont();
        s_name_font.setPointSize(2);
        p->setFont(s_name_font);
        p->setPen(QPen());
        //if it should display name
        if(this->streetNamesToggled) {
            //Painting street names
            for(auto const & s : this->streets->street_list) {
                p->drawText(((s->x1+s->x2)/2)-3, ((s->y1+s->y2)/2)+1, s->name);
            }
        }
        //if it should display id
        else if(this->streetIdToggled) {
            for(auto const & s : this->streets->street_list) {
                p->drawText(((s->x1+s->x2)/2)-1, ((s->y1+s->y2)/2)+1, QString::number(s->id));
            }
        }
        //if it should display time to go through street
        else if(this->streetTimeToggled) {
            for(auto const & s : this->streets->street_list) {
                p->drawText(((s->x1+s->x2)/2)-1, ((s->y1+s->y2)/2)+1, QString::number(s->time));
            }
        }
    }
}

/* @brief Help function for paintEvent to draw all bus positions.
 */
void MapWidget::paintBuses(QPainter* p)
{
    //This part handles painting current position of all buses
    p->setPen(Qt::NoPen);
    p->setBrush(Qt::black);

    //Drawing of curr position of buses
    for(busElem* bus : this->conHandler->busList){
        if(bus->onMap) p->drawEllipse(QPoint(bus->x, bus->y), 2, 2);
    }
}

/* @brief Help function for paintEvent to draw active bus position.
 */
void MapWidget::paintConnection(QPainter *p)
{
    //check if drawConnection was succesfully loaded after click
    if(drawConnection == nullptr) {
        emit ErrorMessage("Error, connection not found.");
        return;
    }

    //Setting up pen
    QPen con_pen = QPen();
    con_pen.setCapStyle(Qt::RoundCap);
    con_pen.setWidth(2);
    con_pen.setBrush(Qt::black);
    p->setPen(con_pen);

    //TODO add closed streets option
    if(true) {
        //if there are no closed streets on the connection
        for(std::tuple<Street*, bool, bool> streetTouple : drawConnection->streetList) {
            Street* s = std::get<0>(streetTouple);
            bool stop = std::get<2>(streetTouple);
            p->drawLine(s->x1, s->y1, s->x2, s->y2);
        }
    }
    else {
        //for closed streets on route
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
    //gets relative position to window coordinates 0-100
    int relX = (event->pos().x() * 100) / width();
    int relY = (event->pos().y() * 100) / height();

    //clearing drawings based on previous mouse clicks
    drawConnection = nullptr;
    drawConnectionToggle = false;

    if(modeModifyClosed) mouseEventModifyClosed(relX, relY);
    else if(modeModifyTraffic) mouseEventModifyTraffic(relX, relY);
    else mouseEventNormal(relX, relY);

    update();
}

/* @brief Function to handle mouse clicks in mode on modifying closed streets.
 * @param x Coordinate X of where mouse was pressed.
 * @param y Coordinate Y of where mouse was pressed.
 */
void MapWidget::mouseEventModifyClosed(int x, int y)
{
    //create some thing to finish creating elsewhere
}

/* @brief Function checks if there is street on given coordinates and if so, adjusts traffic level according to current mode.
 * @param x Coordinate X of where mouse was pressed.
 * @param y Coordinate Y of where mouse was pressed.
 */
void MapWidget::mouseEventModifyTraffic(int x, int y)
{
    //check if mouse is close to street (if to more, applies to all)
    for(auto const & s : this->streets->street_list) {
        QPointF intersectPoint;
        int offset = 1; //toleration of click out of line
        //two lines from line so it works even on line street in similar dirrection
        QLineF pline(x - offset, y - offset, x + offset, y + offset);
        QLineF pline2(x-offset, y+offset, x+offset, y-offset);
        QLineF sline(s->x1, s->y1, s->x2, s->y2);
        if(sline.intersect(pline, &intersectPoint)==QLineF::BoundedIntersection ||
                sline.intersect(pline2, &intersectPoint)==QLineF::BoundedIntersection) {
            //Check if there is bus on currently adjusted street, opertaion won't go through
            bool bus_on_line = false;
            for(busElem* bus : this->conHandler->busList){
                if(bus->onMap)
                    if(bus->curStreet->id == s->id) bus_on_line = true;
            }
            if(bus_on_line){
                emit ErrorMessage("Street traffic level not changed because of bus going through the street.");
                continue;
            }

            if(modeModifyTrafficMode) {
                //increasing mode of traffic
                if(s->traffic < 4) s->traffic++;
                else emit ErrorMessage("Traffic already maxed out on this street.");
            }
            else {
                if(s->traffic > 1) s->traffic--;
                else emit ErrorMessage("Traffic already at lowest level on this street.");
            }
        }
    }
}

/* @brief Funciton handles mouse event in view mode. Checks if there is bus under press and shows if connection route if it is.
 * @param x Coordinate X of where mouse was pressed.
 * @param y Coordinate Y of where mouse was pressed.
 */
void MapWidget::mouseEventNormal(int x, int y)
{
    QPoint mouseXY = QPoint(x, y);
    for(busElem* bus : this->conHandler->busList){
        if(bus->onMap) {
            QPoint busXY = QPoint(bus->x, bus->y) - mouseXY;
            if(busXY.manhattanLength() < 2) {
                drawConnectionToggle = true;
                drawConnection = bus->con;
                return;
            }
        }
    }
}
