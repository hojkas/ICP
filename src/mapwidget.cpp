/**
 * @file mapwidget.cpp
 * @author xstrna14, xlebod00
 * @brief Class to handle all communication with Map widget and its display.
 * Class emits signals to UI to hide/display information, overrides paint and mouse events
 * and accepts many signals from UI and reacts to them.
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
    detourStreets = std::list<Street*>();
    closedStreet = nullptr;
    chosingDetourStreets = false;
    selectedConnectionColor = QColor(122,16,179);
    selectedBusColor = QColor(122,16,179);
    zoomLevel = 1;
    xPan = 0;
    yPan = 0;
    mouseDrag = false;

    conHandler = new connectionHandler;
    conHandler->loadConnections(streets->street_list);
    conHandler->currentTime.setHMS(0,0,0);
    internalClock = new QTimer(this);
    connect(internalClock, &QTimer::timeout, conHandler, &connectionHandler::busUpdate);
    internalClock->start(30000);
    //connect(conHandler, &connectionHandler::busUpdated, conHandler, &connectionHandler::printBuses);
    connect(conHandler,&connectionHandler::busUpdated, this, QOverload<>::of(&MapWidget::update));

    emit hideFinishButton();
}

MapWidget::~MapWidget()
{
    delete streets;
    delete conHandler;
    //delete internalClock;
    //delete updateClock;
}

/** @defgroup slots Slot functions to handle signals from UI.
 * @{ */

/** @brief Function to handle Quickguide selection.
 * @param val New value of quickguide.
 * Based on value of dropdown menu of quickguide, set of signals
 * is emitted to hide all information text windows but one.
 */
void MapWidget::onQuickguideSelection(int val)
{
    if(val == 0) emit show0(true);
    else emit show0(false);
    if(val == 1) emit show1(true);
    else emit show1(false);
    if(val == 2) emit show2(true);
    else emit show2(false);
    if(val == 3) emit show3(true);
    else emit show3(false);
    if(val == 4) emit show4(true);
    else emit show4(false);
}

/** @brief Handles press of "Reset Time" button.
 * When pressed, simulation time is set to zero and all
 * on all busElem is called resetting function.
 */
void MapWidget::onResetButtonPress()
{
    conHandler->currentTime.setHMS(0,0,0);
    for(busElem* bus : this->conHandler->busList){
        conHandler->resetBus(bus);
    }

    update();
}

/** @brief Handles check of Display Street Info: Names.
 * @param val Whether the button is checked or not.
 * If it is checked, inner bool deciding what info to paint is set to true.
 * If not checked, inner bool is set to false.
 */
void MapWidget::onToggleStreetNames(bool val)
{
    if(val) this->streetNamesToggled = true;
    else this->streetNamesToggled = false;
    update();
}

/** @brief Handles check of Display Street Info: Names.
 * @param val Whether the button is checked or not.
 * If it is checked, inner bool deciding what info to paint is set to true.
 * If not checked, inner bool is set to false.
 */
void MapWidget::onToggleStreetId(bool val)
{
    if(val) this->streetIdToggled = true;
    else this->streetIdToggled = false;
    update();
}

/** @brief Handles check of Display Street Info: Time to ride through.
 * @param val Whether the button is checked or not.
 * If it is checked, inner bool deciding what info to paint is set to true.
 * If not checked, inner bool is set to false.
 */
void MapWidget::onToggleStreetTime(bool val)
{
    if(val) this->streetTimeToggled = true;
    else this->streetTimeToggled = false;
    update();
}

/** @brief Handles check of Street Color: Time scale.
 * @param val Whether the button is checked or not.
 * If it is checked, inner bool deciding if paint street in time scale colors is set to true.
 * If not checked, inner bool is set to false.
 */
void MapWidget::onToggleColorTime(bool val)
{
    if(val) this->streetColorTime = true;
    else this->streetColorTime = false;
    update();
}

/** @brief Handles check of Street Color: Time scale.
 * @param val Whether the button is checked or not.
 * If it is checked, inner bool deciding if paint streets in traffic scale colors is set to true.
 * If not checked, inner bool is set to false.
 */
void MapWidget::onToggleColorTraffic(bool val)
{
    if(val) this->streetColorTraffic = true;
    else this->streetColorTraffic = false;
    update();
}

/** @brief Sets corresponding variables on beggining or ending Closed Street Editor.
 * @param val Whether the mode was activated or deactivated.
 */
void MapWidget::onToggleModifyClosed(bool val)
{
    //clearing display connection info
    if(drawConnectionToggle) {
        drawConnection = nullptr;
        drawConnectionToggle = false;
        showConnectionInfo(false);
    }

    if(val) modeModifyClosed = true;
    else modeModifyClosed = false;
    //setting or resetting modifyClosed variables
    if(val) {
        emit showOpenAllOption(true);
        emit ErrorMessage("Closed Roads Editor\nClick on road you wish to close.");
    }
    else {
        emit hideFinishButton();
        emit showOpenAllOption(false);
        closedStreet = nullptr;
        detourStreets.clear();
        chosingDetourStreets = false;
        emit ErrorMessage("");
    }
    update();
}

/** @brief Sets corresponding variables on beggining or ending Traffic Editor.
 * @param val Whether the mode was activated or deactivated.
 */
void MapWidget::onToggleModifyTraffic(bool val)
{
    //clearing display connection info
    if(drawConnectionToggle) {
        drawConnection = nullptr;
        drawConnectionToggle = false;
        showConnectionInfo(false);
    }

    if(val) {
        modeModifyTraffic = true;
        if(drawConnectionToggle) {
            drawConnection = nullptr;
            drawConnectionToggle = false;
            showConnectionInfo(false);
        }
    }
    else modeModifyTraffic = false;
    if(val) emit showModifyTrafficOptions(true);
    else emit showModifyTrafficOptions(false);
    update();
}

/** @brief Handles Increase/Decrease slider changes in Traffic Editor.
 * @param val 1 means increasing, 0 decreasing.
 */
void MapWidget::onToggleModifyTrafficMode(int val)
{
    if(val) {
        modeModifyTrafficMode = true;
        if(drawConnectionToggle) {
            drawConnection = nullptr;
            drawConnectionToggle = false;
            showConnectionInfo(false);
        }
    }
    else modeModifyTrafficMode = false;
}

void MapWidget::onTimeSliderChange(int val)
{
    if(val == 0) timeModifier = 1;
    if(val == 1) timeModifier = 5;
    if(val == 2) timeModifier = 15;
    if(val == 3) timeModifier = 30;
    if(val == 4) timeModifier = 40;
    if(val == 5) timeModifier = 50;
    if(val == 6) timeModifier = 60;
    if(val == 7) timeModifier = 70;
    if(val == 8) timeModifier = 80;
    if(val == 9) timeModifier = 90;
    if(val == 10) timeModifier = 100;

    //resets internal clock with modified value
    int clock_time = 30000 / timeModifier;
    internalClock->start(clock_time);
    update();
}

/** @brief Finishes closure of a street.
 * Checks if the closure is valid and if so, stores data about it.
 */
void MapWidget::onModifyClosedFinish()
{
    bool allOK = true;

    int startX = closedStreet->x1;
    int startY = closedStreet->y1;
    int endX = closedStreet->x2;
    int endY = closedStreet->y2;
    bool front = true;

    for(Street *s: detourStreets) {
        if(front) {
            //special treatment of first street in detour list
            if((s->x1 == endX && s->y1 == endY) || (s->x2 == endX && s->y2 == endY)) {
                //switches end and start point of closure for  easier checking
                endX = closedStreet->x1;
                endY = closedStreet->y1;
                startX = closedStreet->x2;
                startY = closedStreet->y2;
            }
            front = false;
        }
        //checks if next street s follows the previous one (at the point that wasn't connected before)
        if((s->x1 == startX && s->y1 == startY)) {
            startX = s->x2;
            startY = s->y2;
        }
        else if((s->x2 == startX && s->y2 == startY)) {
            startX = s->x1;
            startY = s->y1;
        }
        else {
            emit ErrorMessage("Streets are not continuous detour.");
            allOK = false;
            break;
        }
    }

    //check if closed street isn't beggining or end of some connection
    for(connectionElem con: conHandler->conList) {
        if(std::get<0>(con.streetList.front()) == closedStreet || std::get<0>(con.streetList.back()) == closedStreet) {
            emit ErrorMessage("Can't close street that is the end stop of some connection.");
            allOK = false;
            break;
        }
    }

    if(allOK && (startX != endX || startY != endY)) {
        emit ErrorMessage("Detour isn't connected to closed street.");
        allOK = false;
    }

    for(busElem* bus : this->conHandler->busList){
        if(bus->onMap)
            if(bus->curStreet->id == closedStreet->id) {
                //bus on line we just want to close
                emit ErrorMessage("Bus on line that was selected to close, please select another one or wait until there is no bus there.");
                allOK = false;
            }
    }

    if(allOK){
        streets->addClosedStreet(closedStreet);
        if(allOK) emit ErrorMessage("Detour added. Press another street if you wish to add another closure.");
        conHandler->createClosure(closedStreet, detourStreets);
        //conHandler->printConnections();
    }

    //reseting state
    chosingDetourStreets = false;
    closedStreet = nullptr;
    detourStreets.clear();
    emit hideFinishButton();
    emit showOpenAllOption(true);
    update();
}

/** @brief Handles Open All Closures button.
 */
void MapWidget::onResetAllButtonPress()
{
    //check if no bus is currently taking detour route
    for(busElem *bus: conHandler->busList) {
        bool onClassicRounte = false;
        for(std::tuple<Street*, bool, bool> streetInfo: bus->connnecton->streetList) {
            if(std::get<0>(streetInfo) == bus->curStreet) {
                onClassicRounte = true;
                break;
            }
        }
        if(!onClassicRounte) {
            emit ErrorMessage("Can't clear closed streets if some bus is riding on detour street. Nothing cleared.");
            return;
        }
    }

    streets->closed_streets.clear();
    chosingDetourStreets = false;
    closedStreet = nullptr;
    detourStreets.clear();
    emit hideFinishButton();

    for(auto i = begin(conHandler->conList); i != end(conHandler->conList); i++){
        connectionElem *con = &*i;
        con->closure = false;
        con->alternateStreets.clear();
    }
    update();
}

//map zoom/pan slot functions ahead

/** @brief Checks if map move arrows should be clickable or not (if the move is possible).
 */
void MapWidget::setMapPanButtons()
{
    if(xPan > 0) emit allowMapMoveLeft(true);
    else emit allowMapMoveLeft(false);
    if(yPan > 0) emit allowMapMoveUp(true);
    else emit allowMapMoveUp(false);

    int viewWidth = 100 - (zoomLevel-1)*25;
    if(xPan + viewWidth < 100) emit allowMapMoveRight(true);
    else emit allowMapMoveRight(false);
    if(yPan + viewWidth < 100) emit allowMapMoveDown(true);
    else emit allowMapMoveDown(false);
}

/** @brief Handles changes to map zoom slider.
 * @param val New value of slider.
 */
void MapWidget::onMapZoomChange(int val)
{
    if(val == zoomLevel) return; //function called from mousewheelevent, where it was already done
    if(val < zoomLevel) {
        //additional adjustment of xPan, yPan points to fit the frame
        zoomLevel = val;
        int viewWidth = 100 - (zoomLevel - 1) * 25;
        if(xPan + viewWidth > 100) xPan = 100 - viewWidth;
        if(yPan + viewWidth > 100) yPan = 100 - viewWidth;

        xPan -= 12;
        yPan -= 12;
        if(xPan < 0) xPan = 0;
        if(yPan < 0) yPan = 0;
    }
    else {
        zoomLevel = val;
        xPan += 12;
        yPan += 12;
    }

    setMapPanButtons();
    update();
}

/** @brief Handles press on direction button: right.
 */
void MapWidget::onMapMoveRight()
{
    int viewWidth = 100 - (zoomLevel - 1) * 25;
    int step = viewWidth / 5;
    int newX = xPan + step;
    if(newX + viewWidth > 100) xPan = 100 - viewWidth;
    else xPan = newX;

    setMapPanButtons();
    update();
}

/** @brief Handles press on direction button: left.
 */
void MapWidget::onMapMoveLeft()
{
    int viewWidth = 100 - (zoomLevel - 1) * 25;
    int newX = xPan - (viewWidth/5);
    if(newX < 0) xPan = 0;
    else xPan = newX;

    setMapPanButtons();
    update();
}

/** @brief Handles press on direction button: up.
 */
void MapWidget::onMapMoveUp()
{
    int viewWidth = 100 - (zoomLevel - 1) * 25;
    int newY = yPan - (viewWidth/5);
    if(newY < 0) yPan = 0;
    else yPan = newY;

    setMapPanButtons();
    update();
}

/** @brief Handles press on direction button: down.
 */
void MapWidget::onMapMoveDown()
{
    int viewWidth = 100 - (zoomLevel - 1) * 25;
    int step = viewWidth / 5;
    int newY = yPan + step;
    if(newY + viewWidth > 100) yPan = 100 - viewWidth;
    else yPan = newY;

    setMapPanButtons();
    update();
}

/** @} */

/** @defgroup helpFunc Private functions to collect data.*/
/** @{ */

/** @brief Creates message about current state of time and sends it to widget to draw.
 */
void MapWidget::createTimerMessage()
{
    QString msg = "";
    msg.append("Time modifier: ");
    msg.append(QString::number(this->timeModifier));
    msg.append("x\n");
    msg.append("Current time:                  ");
    msg.append(QString::number(conHandler->currentTime.hour()).rightJustified(2, '0'));
    msg.append(":");
    msg.append(QString::number(conHandler->currentTime.minute()).rightJustified(2, '0'));
    msg.append(":");
    msg.append(QString::number(conHandler->currentTime.second()).rightJustified(2, '0'));
    emit TimerMessage(msg);
}

/** @brief Takes times in seconds and creates QString out of it.
 * @param time Time value to work into message.
 */
QString MapWidget::createTimeString(int time)
{
    int sec = time % 60;
    int min = time / 60;
    int hour = min / 60;
    min = min % 60;

    QString msg = "";
    msg.append(QString::number(hour).rightJustified(2, '0'));
    msg.append(":");
    msg.append(QString::number(min).rightJustified(2, '0'));
    msg.append(":");
    msg.append(QString::number(sec).rightJustified(2, '0'));
    return msg;
}

/** @brief Collects all relevant information about currently clicked connection and writes it on Widget.
 * @param con ConnectionElem* to collect information about.
 * Writes simulation time the data is relevant to. Lists departure time of ev
 */
void MapWidget::collectConnectionInfo(connectionElem *con)
{
    emit conName(con->name);

    QString msg = "";
    msg.append(QString::number(conHandler->currentTime.hour()).rightJustified(2, '0'));
    msg.append(":");
    msg.append(QString::number(conHandler->currentTime.minute()).rightJustified(2, '0'));
    msg.append(":");
    msg.append(QString::number(conHandler->currentTime.second()).rightJustified(2, '0'));
    emit conGenTime(msg);

    //generating bus departure info
    msg = "";
    bool any = false;
    for(busElem* bus : conHandler->busList) {
        if(bus->connnecton == con) {
            any = true;
            msg.append(createTimeString(bus->departure));

            if(bus->onMap || bus->returning) msg.append("    ");
            if(bus->onMap) msg.append(" (x)");
            if(bus->returning) msg.append(" (r)");
            msg.append("\n");
        }
    }
    if(any == false) msg.append("No buses on this connection.\n");
    emit conBuses(msg);

    //generating General timetable info
    msg = "";
    int time = 0;
    bool first_stop = true;
    std::list<std::tuple<int, QString>> timetable;
    int extra = 0; //for leaving half of street with stop in case it was the last one


    for(std::tuple<Street*, bool, bool> street : con->streetList) {
        Street* s = std::get<0>(street);
        bool stop = std::get<2>(street);
        if(stop) {
            //means there is a stop at street
            if(first_stop) {
                //means it is the first stop, so we don't add half the street to time
                extra = (s->time / 2) + s->time % 2;
                first_stop = false;
            }
            else {
                if(extra != 0) {
                    time += extra;
                    extra = 0;
                }
                time += (s->time / 2) + (s->time % 2);
                extra = s->time / 2;
            }
            if(s->name.isEmpty()) timetable.push_back(std::make_tuple(time, "<" + QString::number(s->id) + ">"));
            else timetable.push_back(std::make_tuple(time, s->name));//msg.append(s->name);
        }
        else {
            //not stop
            time += s->time;
            if(extra != 0) {
                time += extra;
                extra = 0;
            }
        }
    }

    //making message out of collected tuple list
    for(std::tuple<int, QString> stop : timetable) {
        msg.append(createTimeString(std::get<0>(stop)));
        msg.append("   ");
        msg.append(createTimeString((time - std::get<0>(stop))));
        msg.append("   |   ");
        msg.append(std::get<1>(stop));
        msg.append("\n");
    }
    emit conGenTT(msg);

    //generating current timetable info
    msg = "";
    time = 0;
    first_stop = true;
    extra = 0; //for leaving half of street with stop in case it was the last one
    timetable.clear();

    //if there is closure, it will count timetable from alternate street list (aka list of street including detour)
    //if there is none, it will do so from standart street list
    std::list<std::tuple<Street*,bool, bool>> curConList;
    if(con->closure) curConList = con->alternateStreets;
    else curConList = con->streetList;

    for(std::tuple<Street*, bool, bool> street : curConList) {
        Street* s = std::get<0>(street);
        bool stop = std::get<2>(street);
        if(stop) {
            //means there is a stop at street
            if(first_stop) {
                //means it is the first stop, so we don't add half the street to time
                extra = (s->count_time() / 2) + s->count_time() % 2;
                first_stop = false;
            }
            else {
                if(extra != 0) {
                    time += extra;
                    extra = 0;
                }
                time += (s->count_time() / 2) + (s->count_time() % 2);
                extra = s->count_time() / 2;
            }
            if(s->name.isEmpty()) timetable.push_back(std::make_tuple(time, "<" + QString::number(s->id) + ">"));
            else timetable.push_back(std::make_tuple(time, s->name));//msg.append(s->name);
        }
        else {
            //not stop
            time += s->count_time();
            if(extra != 0) {
                time += extra;
                extra = 0;
            }
        }
    }
    //making message out of collected tuple list
    for(std::tuple<int, QString> stop : timetable) {
        msg.append(createTimeString(std::get<0>(stop)));
        msg.append("   ");
        msg.append(createTimeString((time - std::get<0>(stop))));
        msg.append("   |   ");
        msg.append(std::get<1>(stop));
        msg.append("\n");
    }
    emit conCurTT(msg);

    showConnectionInfo(true);
}

/** @} */

/** @defgroup overrides Override functions or functions called only by them.
 * @{ */

/** @brief Override of paintEvent to include painting all that map displays.
 * @param event Information about event.
 * @see paintStreets(QPainter*)
 * @see paintCloseModeInfo(QPainter*)
 * @see paintStreetInfo(QPainter*)
 * @see paintBuses(QPainter*)
 */
void MapWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    int zoomValue = 100 - ((zoomLevel-1)*25);
    p.setWindow(xPan, yPan, zoomValue, zoomValue);


    //refreshes time message
    createTimerMessage();

    //Map outline
    p.drawLine(0,0,100,0);
    p.drawLine(0,0,0,100);
    p.drawLine(100,0,100,100);
    p.drawLine(0,100,100,100);


    paintStreets(&p);
    if(modeModifyClosed) paintCloseModeInfo(&p);
    if(drawConnectionToggle) paintConnection(&p);
    paintStreetInfo(&p);
    paintBuses(&p);
    event->accept();
}

/** @brief Help function for paintEvent to draw all streets in color according to flag info
 * @param p Reference to painter used to draw everything on pan.
 * @see paintEvent
 */
void MapWidget::paintStreets(QPainter* p)
{
    //This part handles color of streets
    //Formating for street painting
    QPen street_pen = QPen();
    street_pen.setCapStyle(Qt::RoundCap);
    if(zoomLevel == 1) street_pen.setWidthF(2.5);
    else if(zoomLevel == 2) street_pen.setWidthF(2);
    else if(zoomLevel == 3) street_pen.setWidthF(1.5);
    else street_pen.setWidth(1);
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

    QPen closure_mark = QPen(Qt::red);
    if(zoomLevel == 1) closure_mark.setWidthF(2.5);
    else if(zoomLevel == 2) closure_mark.setWidthF(2);
    else if(zoomLevel == 3) closure_mark.setWidthF(1.5);
    else closure_mark.setWidth(1);
    p->setPen(closure_mark);
    for(Street* c : streets->closed_streets) {
        float offset;
        if(zoomLevel == 1) offset = 2.0;
        else if(zoomLevel == 2) offset = 1.7;
        else if(zoomLevel == 3) offset = 1.4;
        else offset = 1.1;
        p->drawLine(QPointF(((c->x1+c->x2)/2)-offset, ((c->y1+c->y2)/2)-offset), QPointF(((c->x1+c->x2)/2)+offset, ((c->y1+c->y2)/2)+offset));
        p->drawLine(QPointF(((c->x1+c->x2)/2)+offset, ((c->y1+c->y2)/2)-offset), QPointF(((c->x1+c->x2)/2)-offset, ((c->y1+c->y2)/2)+offset));
    }
}

/** @brief Help function for paintEvent to draw all street info if right bool flag is set
 * @param p Reference to painter used to draw everything on pan.
 * @see paintEvent
 */
void MapWidget::paintStreetInfo(QPainter* p)
{
    // THis part handles radio buttons about displaying street properties
    if(this->streetNamesToggled || this->streetTimeToggled || this->streetIdToggled) {
        //setting drawing properties for writing street properties (where there are)
        QFont s_name_font = QFont();
        if(zoomLevel == 1) s_name_font.setPointSizeF(1.9);
        else if(zoomLevel == 2) s_name_font.setPointSizeF(1.7);
        else if(zoomLevel == 3) s_name_font.setPointSizeF(1.3);
        else s_name_font.setPointSizeF(1.1);

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
                p->drawText(((s->x1+s->x2)/2)-1, ((s->y1+s->y2)/2)+1, QString::number(s->count_time()));
            }
        }
    }
}

/** @brief Help function for paintEvent to draw all bus positions.
 * @param p Reference to painter used to draw everything on pan.
 * @see paintEvent
 */
void MapWidget::paintBuses(QPainter* p)
{
    //This part handles painting current position of all buses
    p->setBrush(Qt::black);

    float bus_size;
    if(zoomLevel == 1) bus_size = 2;
    else if(zoomLevel == 2) bus_size = 1.7;
    else if(zoomLevel == 3) bus_size = 1.4;
    else bus_size = 1.1;

    //Drawing of curr position of buses
    for(busElem* bus : this->conHandler->busList){
        if(drawConnectionToggle && drawConnection != nullptr) {
            //check if bus is on currently drawn connection
            if(drawConnection->name == bus->connnecton->name) p->setBrush(selectedBusColor);
            else p->setBrush(Qt::black);
        }
        else p->setBrush(Qt::black);
        if(bus->onMap) {
            if(zoomLevel < 3) p->setPen(QPen(Qt::black, 1));
            else p->setPen(QPen(Qt::black, 0.5));
            p->drawEllipse(QPointF(bus->x, bus->y), bus_size, bus_size);
            p->setPen(Qt::NoPen);
            p->drawEllipse(QPointF(bus->x, bus->y), bus_size, bus_size);

        }
    }
}

/** @brief Help function for paintEvent to draw active bus position.
 * @param p Reference to painter used to draw everything on pan.
 * @see paintEvent
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
    if(zoomLevel == 1) con_pen.setWidthF(2.5);
    else if(zoomLevel == 2) con_pen.setWidthF(2);
    else if(zoomLevel == 3) con_pen.setWidthF(1.5);
    else con_pen.setWidthF(1);
    con_pen.setBrush(selectedConnectionColor);
    p->setPen(con_pen);

    int conLen;
    std::list<std::tuple<Street*, bool, bool>> listToDraw;

    if(!drawConnection->closure) {
        conLen = drawConnection->streetList.size();
        listToDraw = drawConnection->streetList;
    }
    else {
        conLen = drawConnection->alternateStreets.size();
        listToDraw = drawConnection->alternateStreets;
    }

    int i = 0;
    for(std::tuple<Street*, bool, bool> streetTouple : listToDraw) {
        Street* s = std::get<0>(streetTouple);
        bool stop = std::get<2>(streetTouple);
        int x = (s->x1 + s->x2)/2;
        int y = (s->y1 + s->y2)/2;

        if(i == 0) {
            //first street, to draw just a half
            bool direction = std::get<1>(streetTouple);
            if(direction) p->drawLine(x, y, s->x2, s->y2);
            else p->drawLine(x, y, s->x1, s->y1);
        }
        else if(i+1 == conLen) {
            //last street, to draw just half
            bool direction = std::get<1>(streetTouple);
            if(direction) p->drawLine(x, y, s->x1, s->y1);
            else p->drawLine(x, y, s->x2, s->y2);
        }
        else {
            //draw full street
            p->drawLine(s->x1, s->y1, s->x2, s->y2);
        }

        //drawing stop if there is one
        if(stop) {
            QPen backup = p->pen();
            p->setPen(Qt::NoPen);
            p->setBrush(selectedConnectionColor);
            if(zoomLevel == 1) p->drawRect(x-2, y-2, 4, 4);
            else if(zoomLevel == 2) p->drawRect(QRectF(x-1.7, y-1.7, 3.4, 3.4));
            else if(zoomLevel == 3) p->drawRect(QRectF(x-1.3, y-1.3, 2.6, 2.6));
            else p->drawRect(x-1, y-1, 2, 2);
            p->setPen(backup);
        }

        i++;
    }
}

/** @brief When in Closed Editor, it handles drawing recently clicked roads to be part of closure.
 * @param p Reference to painter used to draw everything on pan.
 * @see paintEvent
 */
void MapWidget::paintCloseModeInfo(QPainter *p)
{
    QPen base = QPen(Qt::black);
    if(zoomLevel == 1) base.setWidthF(2.6);
    else if(zoomLevel == 2) base.setWidth(2.1);
    else if(zoomLevel == 3) base.setWidthF(1.6);
    else base.setWidthF(1.1);
    base.setCapStyle(Qt::RoundCap);
    QPen closed = QPen(Qt::red);
    closed.setCapStyle(Qt::RoundCap);
    QPen detour = QPen(Qt::green);
    detour.setCapStyle(Qt::RoundCap);
    if(zoomLevel == 1) {
        closed.setWidthF(1.8);
        detour.setWidthF(1.8);
    }
    else if(zoomLevel == 2) {
        closed.setWidthF(1.3);
        detour.setWidthF(1.3);
    }
    else if(zoomLevel == 3) {
        closed.setWidthF(1);
        detour.setWidthF(1);
    }
    else {
        closed.setWidthF(0.7);
        detour.setWidthF(0.7);
    }

    if(!detourStreets.empty())
    {
        //draw detour streets
        p->setPen(base);
        for(Street* s: detourStreets) {
            p->drawLine(s->x1, s->y1, s->x2, s->y2);
        }
        p->setPen(detour);
        for(Street* s: detourStreets) {
            p->drawLine(s->x1, s->y1, s->x2, s->y2);
        }
    }

    if(closedStreet != nullptr)
    {
        //draw curr edited closed street
        p->setPen(base);
        p->drawLine(closedStreet->x1, closedStreet->y1, closedStreet->x2, closedStreet->y2);
        p->setPen(closed);
        p->drawLine(closedStreet->x1, closedStreet->y1, closedStreet->x2, closedStreet->y2);
    }
}

/** @brief Override resizeEvent to ensure the map widget is square after resizing main window.
 * @param event Reference to object holding all information about the event.
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

/** @brief Override for capturing all mouse clicking inside widget.
 * @param event Reference to object holding all information about the event.
 */
void MapWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        //right mouse button indicates start of dragging to move map, preparing variables
        mouseDrag = true;
        dragX = event->x();
        dragY = event->y();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    //gets relative position to window coordinates 0-100
    int relX = (event->pos().x() * (100 - (zoomLevel-1)*25)) / width() + xPan;
    int relY = (event->pos().y() * (100 - (zoomLevel-1)*25)) / height() + yPan;

    //clearing drawings based on previous mouse clicks
    if(drawConnectionToggle) {
        drawConnection = nullptr;
        drawConnectionToggle = false;
        showConnectionInfo(false);
    }

    if(modeModifyClosed) mouseEventModifyClosed(relX, relY);
    else if(modeModifyTraffic) mouseEventModifyTraffic(relX, relY);
    else mouseEventNormal(relX, relY);

    update();
}

/** @brief Function to handle mouse clicks in mode on modifying closed streets.
 * @param x Coordinate X of where mouse was pressed.
 * @param y Coordinate Y of where mouse was pressed.
 */
void MapWidget::mouseEventModifyClosed(int x, int y)
{
    if(!chosingDetourStreets) {
        //means chosing street to close
        Street* clicked = findClickedStreet(x, y);
        if(clicked == nullptr) return; //nothing clicked, wait for another event
        //check if street is not already closed
        for(Street* s : streets->closed_streets) {
            if(s == clicked) {
                emit ErrorMessage("This street is already closed. Chose another one.");
                return;
            }
        }

        for(busElem* bus : this->conHandler->busList){
            if(bus->onMap)
                if(bus->curStreet->id == clicked->id) {
                    //bus on line we just want to close
                    emit ErrorMessage("Bus on line that was selected to close, please select another one or wait until there is no bus there.");
                    return;
                }
        }

        chosingDetourStreets = true;
        emit showFinishButton();
        emit showOpenAllOption(false);
        emit ErrorMessage("Click streets that should make the detour one by one (must be continuous).");
        closedStreet = clicked;
    }
    else {
        //means chosing street to close
        Street* clicked = findClickedStreet(x, y);
        if(clicked == nullptr) return; //nothing clicked, wait for another event
        //check if street is not already closed
        for(Street* s : streets->closed_streets) {
            if(s == clicked) {
                emit ErrorMessage("This street is closed and cannot be part of detour route. Chose another one.");
                return;
            }
        }

        for(Street* s : detourStreets) {
            if(s == clicked) {
                emit ErrorMessage("This street is already part of detour. Choose another one or start again.");
                return;
            }
        }
        detourStreets.push_back(clicked);
    }
}

/** @brief Function checks if there is street on given coordinates and if so, adjusts traffic level according to current mode.
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

/** @brief Funciton handles mouse event in view mode. Checks if there is bus under press and shows if connection route if it is.
 * @param x Coordinate X of where mouse was pressed.
 * @param y Coordinate Y of where mouse was pressed.
 */
void MapWidget::mouseEventNormal(int x, int y)
{
    QPointF mouseXY = QPointF(x, y);
    for(busElem* bus : this->conHandler->busList){
        if(bus->onMap) {
            QPointF busXY = QPointF(bus->x, bus->y) - mouseXY;
            if(busXY.manhattanLength() < 3) {
                drawConnectionToggle = true;
                drawConnection = bus->connnecton;
                collectConnectionInfo(bus->connnecton);
                return;
            }
        }
    }
}

/** @brief Finds whether street was clicked and if yes, returns its pointer.
 * @param x Coordinate X of where mouse was pressed.
 * @param y Coordinate Y of where mouse was pressed.
 * It creates two help lines creating small cross over given point (x, y). Then it uses these lines to
 * check if they intersect with street line.
 */
Street* MapWidget::findClickedStreet(int x, int y)
{
    for(auto const & s : this->streets->street_list) {
        QPointF intersectPoint;
        int offset = 1; //toleration of click out of line
        //two lines from line so it works even on line street in similar dirrection
        QLineF pline(x - offset, y - offset, x + offset, y + offset);
        QLineF pline2(x - offset, y + offset, x + offset, y - offset);
        QLineF sline(s->x1, s->y1, s->x2, s->y2);
        if(sline.intersect(pline, &intersectPoint)==QLineF::BoundedIntersection ||
                sline.intersect(pline2, &intersectPoint)==QLineF::BoundedIntersection) {
            return s;
        }
    }
    return nullptr;
}

/** @brief Override of wheel event to use it for zooming in and out.
 * @param event Reference to object holding all information about the event.
 */
void MapWidget::wheelEvent(QWheelEvent *event)
{
    //zoom zoom
    if(event->delta() > 0) {
        //zoom in
        if(zoomLevel < 4) {
            zoomLevel++;
            emit adjustMapZoom(zoomLevel);
            //12 as half the value of change in view (25/12, rounded down)
            xPan += 12;
            yPan += 12;
        }
        else return;
    }
    else {
        //zoom out
        if(zoomLevel > 1) {
            zoomLevel--;
            emit adjustMapZoom(zoomLevel);
            int viewWidth = 100 - (zoomLevel - 1) * 25;
            if(xPan + viewWidth > 100) xPan = 100 - viewWidth;
            if(yPan + viewWidth > 100) yPan = 100 - viewWidth;

            xPan -= 12;
            yPan -= 12;
            if(xPan < 0) xPan = 0;
            if(yPan < 0) yPan = 0;
        }
        else return;
    }
    setMapPanButtons();
    update();
}

/** @brief Function to handle tiding variables after finished map drag.
 * @param event Reference to object holding all information about the event.
 */
void MapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton) {
        mouseDrag = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        setMapPanButtons();
        return;
    }
}

/** @brief Function to handle mouse drag as moving map.
 * @param event Reference to object holding all information about the event.
 */
void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(mouseDrag) {
        int viewWidth = 100 - (zoomLevel - 1) * 25;
        int smooth = 1;
        if(zoomLevel == 2) smooth = 3;
        else if(zoomLevel == 3) smooth = 6;
        else if(zoomLevel == 4) smooth = 9;
        int newX = xPan + ceil((dragX - event->x())/smooth);
        int newY = yPan + ceil((dragY - event->y())/smooth);

        //checks if newX/Y wound't cause view to go outside the map
        if(newX + viewWidth > 100) newX = 100 - viewWidth;
        if(newY + viewWidth > 100) newY = 100 - viewWidth;
        if(newX < 0) newX = 0;
        if(newY < 0) newY = 0;

        xPan = newX;
        yPan = newY;

        dragX = event->x();
        dragY = event->y();
        event->accept();
        update();
        return;
    }
}
/** @} */
