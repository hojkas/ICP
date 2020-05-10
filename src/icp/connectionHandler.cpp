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
    timeSinceUpdate = 0;
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
    qDebug() << "------BUSES---------";
    for(busElem* bus : busList){
        qDebug() << "X: " <<bus->x << "Y: " << bus->y;
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

void connectionHandler::internalTimer()
{
    for(busElem *bus : busList){
        if(bus->departure == timePassed) bus->onMap = true;
    }
    timePassed++;
    timeSinceUpdate++;
}

void connectionHandler::busUpdate(){
    for(busElem *bus: busList){
        if(bus->onMap){
            bus->timeOnStreet += timeSinceUpdate;
            if(bus->timeOnStreet >= bus->curStreet->time){
                bus->timeOnStreet -= bus->curStreet->time;
                bus->curStreet = std::get<0>(this->findStreet(bus->curStreet, bus->con->streetList, true));
            }
            std::tuple<Street*, bool, bool> streetTuple = this->findStreet(bus->curStreet,bus->con->streetList,false);
            if(std::get<1>(streetTuple)){
                bus->x = bus->curStreet->x1 + (bus->timeOnStreet/bus->curStreet->time) * \
                        (bus->curStreet->x2 - bus->curStreet->x1);
                bus->y = bus->curStreet->y1 + (bus->timeOnStreet/bus->curStreet->time) * \
                        (bus->curStreet->y2 - bus->curStreet->y1);
            }
            else{
                bus->x = bus->curStreet->x2 + (bus->timeOnStreet/bus->curStreet->time) * \
                        (bus->curStreet->x1 - bus->curStreet->x2);
                bus->y = bus->curStreet->y2 + (bus->timeOnStreet/bus->curStreet->time) * \
                        (bus->curStreet->y1 - bus->curStreet->y2);
            }
        }
    }
}
