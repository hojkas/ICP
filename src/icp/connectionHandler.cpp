#include "connectionHandler.h"
#include "backend.h"

busElem::busElem(bool map, connectionElem *connection, int dep, int tOS, int X, int Y, Street *strt, bool r)
{
    onMap = map;
    con = connection;
    departure = dep;
    timeOnStreet = tOS;
    x = X;
    y = Y;
    curStreet = strt;
    returning = r;
}

connectionHandler::connectionHandler(QObject *parent) : QObject(parent)
{
    timePassed = 0;
}

connectionHandler::~connectionHandler()
{
    for(busElem *bus : busList) delete bus;
}

void connectionHandler::loadConnections(std::list<Street*> streetList)
{
    QJsonDocument jsonDocument;
    BackEnd backEnd;
    if(!(backEnd.loadFile("../../examples/connections.json", &jsonDocument))){
        exit(1);
    }
    if(!jsonDocument.isArray()){
        std::cerr << "Loaded json document is not an array";
        exit(1);
    }

    QJsonArray jArray = jsonDocument.array();
    if(jArray.isEmpty()){
        std::cerr << "Loaded array is empty";
        exit(1);
    }

    for(int i = 0; i < jArray.size() ; i++){
        connectionElem con;
        QJsonObject obj = jArray[i].toObject();
        con.name = obj["name"].toString();
        con.closure = false;
        QJsonArray streetArray = obj["streets"].toArray();
        for(int a = 0; a < streetArray.size(); a++){
            int streetID = streetArray[a].toObject()["id"].toInt();
            bool direction = streetArray[a].toObject()["right_direction"].toBool();
            bool stop = streetArray[a].toObject()["stop"].toBool();
            Street* streetPtr;
            for(Street *street: streetList){
                if (street->id == streetID) {
                    streetPtr = street;
                }
            }
            std::tuple<Street*,bool,bool> street(streetPtr,direction,stop);
            con.streetList.push_back(street);
        }
        conList.push_back(con);
        QJsonArray busArray = obj["buses"].toArray();
        for(int a = 0; a < busArray.size(); a++){

            bool onMap = false;
            connectionElem *con =  &conList.back();
            int departure = busArray[a].toObject()["time"].toInt();
            int timeOnStreet = 0;
            Street* currStrt = std::get<0>(con->streetList.front());
            int x = (currStrt->x1 + currStrt->x2) / 2;
            int y = (currStrt->y1 + currStrt->y2) / 2;
            bool returning = false;
            busElem* bus = new busElem(onMap, con, departure, timeOnStreet, x, y, currStrt, returning);
            busList.push_back(bus);
        }
    }
 }

void connectionHandler::printConnections()
{
    for(connectionElem con : conList){
        for(std::tuple<Street*,bool,bool> street : con.streetList){
            qDebug() << std::get<0>(street)->id;
        }
        qDebug() << "-----------------";
    }
}

void connectionHandler::printBuses()
{
    qDebug() << "------BUSES---------";
    for(busElem* bus : busList){
        if(bus->onMap) qDebug() <<"Connection" << bus->con->name << "Street:" << bus->curStreet->id << "Time spent on street:" << bus->timeOnStreet <<"X:" <<bus->x << "Y:" << bus->y;
    }
}

std::tuple<Street*, bool, bool> connectionHandler::findStreet(Street* currStreet, std::list<std::tuple<Street*, bool, bool>> streetList, bool next)
{
    bool found = false;
    for(std::tuple<Street*, bool, bool> streetTuple : streetList){
        Street * street = std::get<0>(streetTuple);
        if(found) return streetTuple;
        if(currStreet == street) {
            if(next) found = true;
            else return streetTuple;
        }
    }
}

void connectionHandler::resetBus(busElem* bus)
{
    bus->onMap = false;
    bus->curStreet = std::get<0>(bus->con->streetList.front());
    int x = bus->curStreet->x1;
    int y = bus->curStreet->y1;
    bus->timeOnStreet = 0;
}

void connectionHandler::busUpdate(){
    int secondsPerTick = 30;
    currentTime = currentTime.addSecs(secondsPerTick);
    QTime temp;
    temp.setHMS(0,0,0,0);
    timePassed = temp.secsTo(currentTime);
    for(busElem *bus: busList){
        bool justEntered = false;
        if(!bus->onMap){
            if(bus->departure < timePassed && (timePassed - bus->departure) <= secondsPerTick){
                bus->onMap = true;
                bus->timeOnStreet = bus->curStreet->count_time() / 2;
                justEntered = true;
            }
        }
        if(bus->onMap){
            if(!justEntered) bus->timeOnStreet += secondsPerTick;
            if(bus->curStreet == std::get<0>(bus->con->streetList.back()) && \
               (bus->timeOnStreet >= bus->curStreet->count_time() / 2)){
                this->resetBus(bus);
                continue;
            }
            else if(bus->timeOnStreet >= bus->curStreet->count_time()){
                bus->timeOnStreet -= bus->curStreet->count_time();
                bus->curStreet = std::get<0>(this->findStreet(bus->curStreet, bus->con->streetList, true));
            }
            std::tuple<Street*, bool, bool> streetTuple = this->findStreet(bus->curStreet,bus->con->streetList,false);
            float streetTime = bus->curStreet->count_time();
            if(std::get<1>(streetTuple)){
                bus->x = (bus->curStreet->x1 + (bus->timeOnStreet/streetTime) * \
                        (bus->curStreet->x2 - bus->curStreet->x1));
                bus->y = bus->curStreet->y1 + (bus->timeOnStreet/streetTime) * \
                        (bus->curStreet->y2 - bus->curStreet->y1);
            }
            else{
                bus->x = bus->curStreet->x2 + (bus->timeOnStreet/streetTime) * \
                        (bus->curStreet->x1 - bus->curStreet->x2);
                bus->y = bus->curStreet->y2 + (bus->timeOnStreet/streetTime) * \
                        (bus->curStreet->y1 - bus->curStreet->y2);
            }
        }
    }
    emit busUpdated();
}

void connectionHandler::createClosure(Street* closed, std::list<Street*> alternativeStreets)
{
    for(connectionElem connection : conList){
        connectionElem* conPtr = &connection;
        for(auto streetTuple : connection.streetList){
            Street *street = std::get<0>(streetTuple);
            if(street == closed){
                if(connection.closure){
                    conPtr->alternateStreets = this->updateClosure(closed, alternativeStreets, connection.alternateStreets, connection);
                }
                else{
                    conPtr->closure = true;
                    conPtr->alternateStreets = this->updateClosure(closed, alternativeStreets, connection.streetList, connection);
                }
            }

        }
    }
}

std::list<std::tuple<Street*, bool, bool>> connectionHandler::updateClosure(Street* closed, std::list<Street*> alternateStreets, auto streetList, connectionElem connection)
{
    Street *prevStreet;
    bool prevDirection;
    bool prevReturning;
    for(auto i = begin(streetList); i!= end(streetList);){
        auto streetTuple = *i;
        Street *street = std::get<0>(streetTuple);
        bool direction = std::get<1>(streetTuple);
        bool returning = std::get<2>(streetTuple);

        if((direction && !returning ) || (!direction && returning)){

            }
    }
}
