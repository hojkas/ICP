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
            tupleElem street(streetPtr,direction,stop);
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
        qDebug() << con.closure;
        for(tupleElem street : con.alternateStreets){
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

tupleElem connectionHandler::findStreet(Street* currStreet, tupleList streetList, bool next)
{
    bool found = false;
    for(tupleElem streetTuple : streetList){
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
    if(bus->con->closure){
        bus->curStreet = std::get<0>(bus->con->alternateStreets.front());
    }
    else{
        bus->curStreet = std::get<0>(bus->con->streetList.front());
    }
    bus->x = bus->curStreet->x1;
    bus->y = bus->curStreet->y1;
    bus->streetIndex = 0;
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
                if(bus->con->closure){
                    bus->curStreet = std::get<0>(this->findStreet(bus->curStreet, bus->con->alternateStreets, true));
                }
                else{
                    bus->curStreet = std::get<0>(this->findStreet(bus->curStreet, bus->con->streetList, true));
                }
            }
            tupleElem streetTuple;
            if(bus->con->closure){
                streetTuple = this->findStreet(bus->curStreet,bus->con->alternateStreets,false);
            }
            else{
                streetTuple = this->findStreet(bus->curStreet,bus->con->streetList,false);
            }
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
    for(auto i = begin(conList); i != end(conList); i++){
        connectionElem* conPtr = &(*i);
        tupleList streetList;
        if(conPtr->closure){
            streetList = conPtr->alternateStreets;
        }
        else{
            streetList = conPtr->streetList;
        }
        for(auto streetTuple : streetList){
            Street *street = std::get<0>(streetTuple);
            if(street == closed){
                if(conPtr->closure){
                    conPtr->alternateStreets = this->updateClosure(closed, alternativeStreets, conPtr->alternateStreets);
                    conPtr->alternateStreets = this->shortenPath(conPtr->alternateStreets);
                }
                else{
                    conPtr->closure = true;
                    conPtr->alternateStreets = this->updateClosure(closed, alternativeStreets, conPtr->streetList);
                    conPtr->alternateStreets = this->shortenPath(conPtr->alternateStreets);
                }
            }

        }
    }
}

tupleList connectionHandler::shortenPath(tupleList streetList)
{
    // Create return list of the alternate Route
    tupleList returnList;
    bool duplicateFound = false;
    Street *prevStreet;
    for(tupleElem streetTuple : streetList){
        Street* currentStreet = std::get<0>(streetTuple);
        bool isStop = std::get<2> (streetTuple);
        if(streetTuple == streetList.front()){
            prevStreet = currentStreet;
            returnList.push_back(streetTuple);
            continue;
        }

        if(currentStreet == prevStreet && !isStop){
            duplicateFound = true;
            returnList.pop_back();
        }

        else{
            returnList.push_back(streetTuple);
        }
    }
    if(duplicateFound){
        returnList = this->shortenPath(returnList);
    }
    return returnList;
}

tupleList connectionHandler::updateClosure(Street* closed, std::list<Street*> alternateStreets, tupleList streetList)
{
    // Create return list of the alternate Route
    tupleList alternateList;
   // Go through every street
    for(tupleElem streetTuple : streetList){
        // Obtain the street pointer and the direction
        Street *street = std::get<0>(streetTuple);
        bool direction = std::get<1>(streetTuple);
        // If the street is closed, do not push to the new list
        if(street == closed){
            // Determining the direction of first element is oposite to the rest
            Street* altStreet = alternateStreets.front();

            if(direction){
                if(street->x1 == altStreet->x2 && street->y1 == altStreet->y2){
                    direction = false;
                }
                else if(street->x1 == altStreet->x1 && street->y1 == altStreet->y1){
                    direction = true;
                }
                else{
                    alternateStreets.reverse();
                    altStreet = alternateStreets.front();
                    if(street->x1 == altStreet->x2 && street->y1 == altStreet->y2){
                        direction = false;
                    }
                    else if(street->x1 == altStreet->x1 && street->y1 == altStreet->y1){
                        direction = true;
                    }
                }
            }

            else{
                if(street->x2 == altStreet->x1 && street->y2 == altStreet->y1){
                    direction = true;
                }
                else if(street->x2 == altStreet->x2 && street->y2 == altStreet->y2){
                    direction = false;
                }
                else{
                    alternateStreets.reverse();
                    altStreet = alternateStreets.front();
                    if(street->x2 == altStreet->x1 && street->y2 == altStreet->y1){
                        direction = true;
                    }
                    else if(street->x2 == altStreet->x2 && street->y2 == altStreet->y2){
                        direction = false;
                    }
                }
            }
            // Add the first street to the list
            tupleElem temp(altStreet, direction, false);
            alternateList.push_back(temp);
            // In the for loop Street* street represents the previous street and direction
            // represents the direction of the previous street
            street = alternateStreets.front();
            // Go through the entire list of alternate streets
            for(Street* altStreet : alternateStreets){
                // First element was already handled
                if(altStreet == alternateStreets.front()) continue;

                // If switching direction from true -> false
                if(direction){
                    if(street->x2 == altStreet->x2 && street->y2 == altStreet->y2){
                       direction = false;
                    }
                }
                // If switching direction from false -> true
                else{
                    if(street->x1 == altStreet->x1 && street->y1 == altStreet->y1){
                        direction = true;
                    }
                }
                tupleElem temp(altStreet, direction, false);
                // Add to the list
                alternateList.push_back(temp);
                // Set previous street
                street = altStreet;
            }

        }
        // if the street is not closed, push to the list
        else{
            alternateList.push_back(streetTuple);
        }
    }
    return alternateList;
}
