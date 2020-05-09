#include "connectionHandler.h"
#include "backend.h"


connectionHandler::connectionHandler(QObject *parent) : QObject(parent)
{

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
            Street* streetPtr = this->findStreet(streetID, streetList);
            std::tuple<Street*,bool,bool> street(streetPtr,direction,stop);
            con.streetList.push_back(street);
        }
        conList.push_back(con);
        QJsonArray busArray = obj["buses"].toArray();
        for(int a = 0; a < busArray.size(); a++){
            busElem bus;
            bus.con =  &conList.back();
            bus.departure = busArray[a].toObject()["time"].toInt();
            bus.sinceDeparture = 0;
            Street* currStrt = std::get<0>(con.streetList.front());
            bus.x = (currStrt->x1 + currStrt->x2) / 2;
            bus.y = (currStrt->y1 + currStrt->y2) / 2;
            bus.curStreet = currStrt;
            bus.returning = false;
            busList.push_back(bus);
        }

    }
 }

void connectionHandler::printConnections()
{
    for(connectionElem con : conList){
        for(std::tuple<Street*,bool> street : con.streetList){
            qDebug() << std::get<0>(street)->id;
        }
        qDebug() << "-----------------";
    }
    qDebug() << "------BUSES---------";
    for(busElem bus : busList){
        qDebug() << "X: " <<bus.x << "Y: " << bus.y;
    }
}

Street* connectionHandler::findStreet(int id, std::list<Street*> streetList){
    for(Street *street: streetList){
        if (street->id == id) {
            return street;
        }
    }
}
