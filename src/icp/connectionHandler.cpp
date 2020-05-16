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
    streetIndex = 0;
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
            bool returning = busArray[a].toObject()["return"].toBool();
            int departure = busArray[a].toObject()["time"].toInt();
            int timeOnStreet = 0;
            Street *currStreet;
            if (returning) currStreet = std::get<0>(con->streetList.back());
            else currStreet = std::get<0>(con->streetList.front());
            int x = (currStreet->x1 + currStreet->x2) / 2;
            int y = (currStreet->y1 + currStreet->y2) / 2;
            busElem* bus = new busElem(onMap, con, departure, timeOnStreet, x, y, currStreet, returning);
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

tupleElem connectionHandler::findStreet(Street* currStreet, tupleList streetList, int streetIndex, bool next)
{
    bool found = false;
    auto it = begin(streetList);
    advance(it, streetIndex);
    if(std::get<0>(*it) == currStreet){
        if(next) return *(++it);
        else return *it;
    }
    else{
        qDebug() << "Not found through itter";
        for(tupleElem streetTuple : streetList){
            Street * street = std::get<0>(streetTuple);
            if(found) return streetTuple;
            if(currStreet == street) {
                if(next) found = true;
                else return streetTuple;
            }
        }
    }
}

void connectionHandler::resetBus(busElem* bus)
{
    bus->onMap = false;
    if(bus->returning) std::get<0>(bus->con->streetList.back());
    else bus->curStreet = std::get<0>(bus->con->streetList.front());
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
            tupleList streetList = bus->con->streetList;
            if(bus->con->closure) streetList = bus->con->alternateStreets;
            if(bus->returning) streetList.reverse();

            if(!justEntered) bus->timeOnStreet += secondsPerTick;
            if(bus->curStreet == std::get<0>(streetList.back()) && \
               (bus->timeOnStreet >= bus->curStreet->count_time() / 2)){
                this->resetBus(bus);
                continue;
            }
            else if(bus->timeOnStreet >= bus->curStreet->count_time()){
                bus->timeOnStreet -= bus->curStreet->count_time();
                bus->curStreet = std::get<0>(this->findStreet(bus->curStreet, streetList, bus->streetIndex, true));
                bus->streetIndex++;
            }
            tupleElem streetTuple;
            streetTuple = this->findStreet(bus->curStreet,streetList ,bus->streetIndex,false);
            float streetTime = bus->curStreet->count_time();
            bool direction = std::get<1>(streetTuple);
            if(bus->returning) direction = !direction;
            if(direction){
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
        tupleList streetList = conPtr->streetList;
        if(conPtr->closure) streetList = conPtr->alternateStreets;

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

/* @brief Function goes through streetList, checks if two same streets are next to each other without stop, and removes such (recursively if needed)
 * @param tupleList List of all streets of connection.
 * @return tupleList list of connection with removed duplicit streets.
 */
tupleList connectionHandler::shortenPath(tupleList streetList)
{
    tupleElem prev;
    tupleList newList;
    bool foundDuplicate = false;
    for(std::tuple<Street*,bool,bool> streetTuple : streetList) {
        //for first item on the lists
        if(streetTuple == streetList.front()) {
            prev = streetTuple;
            newList.push_back(streetTuple);
            continue;
        }

        if(std::get<0>(prev) == std::get<0>(streetTuple)) {
            //the same street twice in row
            if(std::get<1>(prev) != std::get<1>(streetTuple) && (!std::get<2>(prev)) && (!std::get<2>(streetTuple))) {
                //direction is different, neither of them are a stop
                foundDuplicate = true;
                //erases last element and doesn't push this one
                newList.pop_back();
                continue;
            }
        }

        newList.push_back(streetTuple);
        prev = streetTuple;
    }

    if(foundDuplicate) return shortenPath(newList);
    else return newList;
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
