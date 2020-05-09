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
        QJsonObject obj = jArray[i].toObject();
        QString name = obj["name"].toString();
        std::list<QString> crossing;
        for(int a = 0; a < obj["crossing"].toArray().size(); a++){
            crossing.push_back(obj["crossing"].toArray()[a].toString());
        }
        std::list<QString> stops;
        for(int a = 0; a < obj["stops"].toArray().size(); a++){
            stops.push_back(obj["stops"].toArray()[a].toString());
        }
    }
 }
