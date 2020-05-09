/**
 * @file street.cpp
 * @author xstrna14
 * @brief TODO
 */

#include "street.h"
#include "backend.h"

Street::Street(QObject *parent) : QObject(parent)
{

}

Street::Street(int x_1, int y_1, int x_2, int y_2, int s_id, QString s_name, QObject *parent) : QObject(parent)
{
    x1 = x_1;
    x2 = x_2;
    y1 = y_1;
    y2 = y_2;
    id = s_id;
    name = s_name;
    difficulty = 1;
}

AllStreets::AllStreets(QObject *parent) : QObject(parent)
{

}

void AllStreets::loadStreets()
{
    QJsonDocument jsonDocument;
    BackEnd backEnd;


    //opening document with json, checking if it's not in undesired format
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

    //Cycle for going through each json object, creating Street object of it and appending to AllStreets list
    for(int i = 0; i < jArray.size(); i++){
        QJsonObject obj = jArray[i].toObject();

        //if any of important data to form street is missing, closes program
        if(obj["x1"].toString().size() == 0 || obj["x2"].toString().size() == 0 || obj["y1"].toString().size() == 0 ||
                obj["y2"].toString().size() == 0 || obj["id"].toString().size() == 0 || obj["time"].toString().size() == 0) {
            std::cerr << "Missing vital information about " << i << ". street in file.";
            exit(1);
        }

        Street *new_street = new Street(obj["x1"].toInt(), obj["y1"].toInt(), obj["x2"].toInt(), obj["y2"].toInt(), obj["id"].toInt(), obj["name"].toString());
        street_list.push_back(new_street);
     }
}

