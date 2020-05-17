/**
 * @file connectionHandler.cpp
 * @author xstrna14, xlebod00
 * @brief Class for storing information about connections.
 */

#include "connectionHandler.h"
#include "backend.h"

busElem::busElem(bool map, connectionElem *connection, int dep, int tOS, int X, int Y, Street *strt, bool r)
{
    onMap = map;
    connnecton = connection;
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

/**
 * @brief Creates all connectionElem and busElem objects according to the "../../examples/connections.json" file.
 * @param streetList List off all streets on the map.
 *
 * When called, loads the contents of the connections.json file and extracts all the information
 * necessary to successfully create all the elements. If any error occurs while loading the file or
 * parsing the JSON structure, the program is ended with the return code of 1 and an error message is
 * printed in the std::cerr stream.
 */
void connectionHandler::loadConnections(std::list<Street*> streetList)
{
    QJsonDocument jsonDocument;
    BackEnd backEnd;
    // Error in loading from file
    if(!(backEnd.loadFile("../../examples/connections.json", &jsonDocument))){
        exit(1);
    }
    // Unexpected data in JSON file
    if(!jsonDocument.isArray()){
        std::cerr << "Loaded json document is not an array";
        exit(1);
    }

    QJsonArray jArray = jsonDocument.array();
    if(jArray.isEmpty()){
        std::cerr << "Loaded array is empty";
        exit(1);
    }

    // Load all connections
    for(int it = 0; it < jArray.size() ; it++){
        connectionElem connection;
        QJsonObject connectionObj = jArray[it].toObject();
        // Load all connection information
        connection.name = connectionObj["name"].toString();
        connection.closure = false;
        QJsonArray streetArray = connectionObj["streets"].toArray();
        // Load all streets of the connection
        for(int it2 = 0; it2 < streetArray.size(); it2++){
            // Load all street information
            int streetID = streetArray[it2].toObject()["id"].toInt();
            bool direction = streetArray[it2].toObject()["right_direction"].toBool();
            bool stop = streetArray[it2].toObject()["stop"].toBool();
            Street* streetPtr;
            // Find street in streetList
            for(Street *street: streetList){
                if (street->id == streetID) {
                    streetPtr = street;
                }
            }
            // Push street to connections streetList
            tupleElem street(streetPtr,direction,stop);
            connection.streetList.push_back(street);
        }
        conList.push_back(connection);
        QJsonArray busArray = connectionObj["buses"].toArray();
        // Load all buses on the connection
        for(int it2 = 0; it2 < busArray.size(); it2++){
            // Load or set all bus information
            bool onMap = false;
            int timeOnStreet = 0;
            bool returning = busArray[it2].toObject()["return"].toBool();
            int departure = busArray[it2].toObject()["time"].toInt();
            connectionElem *con =  &conList.back();
            Street *currStreet = std::get<0>(con->streetList.front());
            if (returning) currStreet = std::get<0>(con->streetList.back());
            int x = (currStreet->x1 + currStreet->x2) / 2;
            int y = (currStreet->y1 + currStreet->y2) / 2;
            // Construct busElem
            busElem* bus = new busElem(onMap, con, departure, timeOnStreet, x, y, currStreet, returning);
            busList.push_back(bus);
        }
    }
 }

/**
 * @brief Prints the IDs of all streets within a connection.
 *
 * Mainly for debugging purposes.
 */
void connectionHandler::printConnections()
{
    for(connectionElem con : conList){
        qDebug() << "Connection -" << con.name;
        qDebug() << "Streets:";
        for(tupleElem street : con.streetList){
            qDebug() << std::get<0>(street)->id;
        }
        qDebug() << "-----------------";
    }
}
/**
 * @brief Prints the closure status of all streets within a connection.
 *
 * Mainly for debugging purposes.
 */
void connectionHandler::printClosures()
{
    for(connectionElem con : conList){
        qDebug() << "Connection -" << con.name;
        qDebug() << "Closure:" << con.closure;
        if(con.closure){
            for(tupleElem street : con.alternateStreets){
                qDebug() << std::get<0>(street)->id;
            }
        }
        qDebug() << "-----------------";
    }
}
/**
 * @brief Prints the all the currently active buses.
 *
 * Mainly for debugging purposes.
 */
void connectionHandler::printBuses()
{
    qDebug() << "------BUSES---------";
    for(busElem* bus : busList){
        if(bus->onMap) qDebug() <<"Connection" << bus->connnecton->name << "Street:" << bus->curStreet->id << "Time spent on street:" << bus->timeOnStreet <<"X:" <<bus->x << "Y:" << bus->y;
    }
}

/**
 * @brief Finds the tupleElem containing the currStreet in the desired tupleList
 * @param currStreet Street to search for
 * @param streetList tupleList to search in
 * @param streetIndex Index of the desired street
 * @param next return the next element instead
 * @return tupleElem with currStreet, or if next is true, returns the following tupleElem
 *
 * First tries to find the element according to the streetIndex position, if no match is found, searches
 * the streetList again, and looks for the first match.
 */
tupleElem connectionHandler::findStreet(Street* currStreet, tupleList streetList, int streetIndex, bool next)
{
    bool found = false;
    auto it = begin(streetList);
    advance(it, streetIndex);
    if(std::get<0>(*it) == currStreet){
        // Found object at expected streetIndex
        if(next) return *(++it);
        else return *it;
    }
    else{
        // Try to find first matching street
        for(tupleElem streetTuple : streetList){
            Street * street = std::get<0>(streetTuple);
            if(found) return streetTuple;
            if(currStreet == street) {
                if(next) found = true;
                else return streetTuple;
            }
        }
    }
    std::cerr << "Internal error: findStreet - No street found.";
    exit(99);
}

/**
 * @brief Resets the busElem so it is "ready" to depart again.
 * @param bus busElem to reset to default values.
 *
 * Resets the busElem so the correct values are set for the next departure.
 */
void connectionHandler::resetBus(busElem* bus)
{
    // Reset starting values
    bus->onMap = false;
    if(bus->returning) bus->curStreet = std::get<0>(bus->connnecton->streetList.back());
    else bus->curStreet = std::get<0>(bus->connnecton->streetList.front());
    bus->x = bus->curStreet->x1;
    bus->y = bus->curStreet->y1;
    bus->streetIndex = 0;
    bus->timeOnStreet = 0;
}

/**
 * @brief Updates the position of all busElems.
 *
 * Every call of the busUpdate() function is treated as 30 seconds passing, marked by the
 * secodnsPerTick variable. The function also updates whether a current bus is to be drawn on the map.
 * If the bus is to be displayed, the function takes into account the direction of its travel, whether
 * it's on a return route and calculates it's current X and Y coordinates accordingly.
 * It also updates the "curStreet" property of the busElem and calls the resetBus() function when a
 * bus has reached its final stop.
 */
void connectionHandler::busUpdate(){
    int secondsPerTick = 30;
    currentTime = currentTime.addSecs(secondsPerTick);
    QTime temp;
    temp.setHMS(0,0,0,0);
    // Calculate time since midnight
    timePassed = temp.secsTo(currentTime);

    for(busElem *bus: busList){
        bool justEntered = false;
        if(!bus->onMap){
            if(bus->departure < timePassed && (timePassed - bus->departure) <= secondsPerTick){
                // Bus becomes active
                bus->onMap = true;
                bus->timeOnStreet = bus->curStreet->count_time() / 2;
                justEntered = true;
            }
        }
        if(bus->onMap){
            // Set the correct list of streets
            tupleList streetList = bus->connnecton->streetList;
            if(bus->connnecton->closure) streetList = bus->connnecton->alternateStreets;
            if(bus->returning) streetList.reverse();

            // Obtain current street information
            tupleElem streetTuple = this->findStreet(bus->curStreet,streetList ,bus->streetIndex,false);
            float streetTime = bus->curStreet->count_time();
            bool direction = std::get<1>(streetTuple);

            // If the bus just entered it's timeOnStreet has already been updated
            if(!justEntered) bus->timeOnStreet += secondsPerTick;
            if(streetTuple == streetList.back() && (bus->timeOnStreet >= bus->curStreet->count_time() / 2)){
                // Bus has reached final stop
                this->resetBus(bus);
                continue;
            }

            else if(bus->timeOnStreet >= bus->curStreet->count_time()){
                // Bus has entered the following street
                bus->timeOnStreet -= bus->curStreet->count_time();
                streetTuple = this->findStreet(bus->curStreet, streetList, bus->streetIndex, true);
                bus->curStreet = std::get<0>(streetTuple);
                streetTime = bus->curStreet->count_time();
                direction = std::get<1>(streetTuple);
                bus->streetIndex++;
            }

            if(bus->returning) direction = !direction;
            // Calculate the position based on the direction of travel, and the ration of time already
            // traveled on street and the total time to pass the street
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

/**
 * @brief The "root" function of creating a closure on a street. Calls the updateClosure() and shortenPath() functions.
 * @param closed Street to be closed.
 * @param alternativeStreets Streets to follow instead.
 *
 * The function finds all connections with the closed street and updates their values accordingly, by
 * calling for the updateClosure() and shortenPath() functions.
 */
void connectionHandler::createClosure(Street* closed, std::list<Street*> alternativeStreets)
{
    for(auto i = begin(conList); i != end(conList); i++){
        connectionElem* conPtr = &(*i);
        tupleList streetList = conPtr->streetList;
        if(conPtr->closure) streetList = conPtr->alternateStreets;

        for(auto streetTuple : streetList){
            Street *street = std::get<0>(streetTuple);
            if(street == closed){
                // Street to be closed found - updated the necessary infromation
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

/** @brief Function goes through streetList, checks if two same streets are next to each other without stop, and removes such (recursively if needed)
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

/**
 * @brief Replaces the closed street with the go-around route in the given streetList
 * @param closed Street to be replaced
 * @param alternateStreets Streets to replace with
 * @param streetList Streets to create closure on
 * @return Updated list of streets with closure completed
 */
tupleList connectionHandler::updateClosure(Street* closed, std::list<Street*> alternateStreets, tupleList streetList)
{
    // Return list
    tupleList alternateList;

    for(tupleElem streetTuple : streetList){
        Street *street = std::get<0>(streetTuple);
        bool direction = std::get<1>(streetTuple);

        if(street == closed){
            Street* altStreet = alternateStreets.front();
            // Setting the direction of the new street based on it's position to ensure a continous route
            if(direction){
                if(street->x1 == altStreet->x2 && street->y1 == altStreet->y2){
                    direction = false;
                }
                // Technically reduntant, as the next else could be replaced with a negation of
                // this condition. However, kept for easier readability of code
                else if(street->x1 == altStreet->x1 && street->y1 == altStreet->y1){
                    direction = true;
                }
                else{
                    // The alternateStreets were input in the "opposite" direction in respect to
                    // the direction of the connection
                    alternateStreets.reverse();
                    altStreet = alternateStreets.front();
                    // Repeat of the above conditions to set the direction
                    if(street->x1 == altStreet->x2 && street->y1 == altStreet->y2){
                        direction = false;
                    }
                    else if(street->x1 == altStreet->x1 && street->y1 == altStreet->y1){
                        direction = true;
                    }
                    else{
                        std::cerr << "Internal error on line 369";
                        exit(99);
                    }
                }
            }
            // Similar functionality to the if branch
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

            // In the following for loop Street* street represents the previous street and direction
            // represents the direction of the previous street
            street = alternateStreets.front();
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
                alternateList.push_back(temp);
                // Set previous street
                street = altStreet;
            }

        }
        else{
            // If the street is not closed, push to the list
            alternateList.push_back(streetTuple);
        }
    }
    return alternateList;
}
