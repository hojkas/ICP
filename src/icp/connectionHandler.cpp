#include "connectionHandler.h"
#include "backend.h"


connectionHandler::connectionHandler(QObject *parent) : QObject(parent)
{

}

void connectionHandler::loadConnections()
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
            bool stop = streetArray[a].toObject()["stop"].toBool();
            QString streetID = streetArray[a].toObject()["id"].toString();
            std::tuple<QString,bool> street(streetID,stop);
            con.streetList.push_back(street);
        }


        conList.push_back(con);
    }
 }

void connectionHandler::printConnections()
{
    for(connectionElem con : conList){
        for(std::tuple<QString,bool> street : con.streetList){
            qDebug() << std::get<1>(street);
        }
        qDebug() << "-----------------";
    }
}
