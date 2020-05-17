/**
 * @file street.cpp
 * @author xstrna14
 * @brief Class for creating and handling object Street representing street on map.
 */

#include "street.h"
#include "backend.h"

Street::Street(QObject *parent) : QObject(parent)
{

}

Street::Street(int x_1, int y_1, int x_2, int y_2, int s_id, int s_time, QString s_name, QObject *parent) : QObject(parent)
{
    x1 = x_1;
    x2 = x_2;
    y1 = y_1;
    y2 = y_2;
    id = s_id;
    name = s_name;
    time = s_time;
    traffic = 1;
}

AllStreets::AllStreets(QObject *parent) : QObject(parent)
{
    closed_streets = std::list<Street*>();
}

int Street::count_time()
{
    return (this->time * this->traffic);
}

/*
 * @brief Destructor to delete all objects referenced in Street* list.
 */
AllStreets::~AllStreets()
{
    for(auto const & s : street_list) {
        delete s;
    }
}

void AllStreets::addClosedStreet(Street *s)
{
    closed_streets.push_back(s);
}

void AllStreets::removeClosedStreet(Street *s)
{
    closed_streets.remove(s);
}

/*
 * @brief Function loads streets from map file into street_list of this class.
 * TODO fix to take argument name of file
 */
void AllStreets::loadStreets()
{
    QJsonDocument jsonDocument;
    BackEnd backEnd;


    //opening document with json, checking if it's not in undesired format
    if(!(backEnd.loadFile("../../examples/map.json", &jsonDocument))){
        exit(2);
    }
    if(!jsonDocument.isArray()){
        std::cerr << "Loaded json document is not an array";
        exit(2);
    }
    QJsonArray jArray = jsonDocument.array();
    if(jArray.isEmpty()){
        std::cerr << "Loaded array is empty";
        exit(2);
    }

    //Cycle for going through each json object, creating Street object of it and appending to AllStreets list
    for(int i = 0; i < jArray.size(); i++){
        QJsonObject obj = jArray[i].toObject();

        //if any of important data to form street is missing, closes program
        if(!obj.contains("x1") || !obj.contains("x2") || !obj.contains("y1") || !obj.contains("y2") || !obj.contains("time") ||
                !obj.contains("id")) {
            std::cerr << "Missing vital information about " << i+1 << ". street in file.";
            exit(2);
        }

        Street *new_street = new Street(obj["x1"].toInt(), obj["y1"].toInt(), obj["x2"].toInt(), obj["y2"].toInt(), obj["id"].toInt(), obj["time"].toInt(), obj["name"].toString());
        street_list.push_back(new_street);
     }
}

