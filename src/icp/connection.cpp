#include "connection.h"
#include "backend.h"


connection::connection(QObject *parent) : QObject(parent)
{

}

void connection::loadConnections()
{
    QJsonDocument jsonDocument;
    BackEnd backEnd;
    if(!(backEnd.loadFile("../../examples/map.json", &jsonDocument))){
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
    for(int i = 0; i < 3; i++){
        QJsonObject obj = jArray[i].toObject();
        qDebug() << obj["name"].toString();
     }
}
