#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QTime>
#include <time.h>
#include "street.h"

using tupleElem = std::tuple<Street*, bool, bool>;
using tupleList = std::list<tupleElem>;

class connectionElem
{
public:
    QString name;
    tupleList streetList;
    bool closure;
    tupleList alternateStreets;
};

class busElem
{
public:
    bool onMap;
    connectionElem* con;
    int departure;
    int timeOnStreet;
    float x;
    float y;
    Street* curStreet;
    bool returning;
    busElem(bool map, connectionElem *connnection, int dep, int tOS, int X, int Y, Street *strt, bool r);
};

class connectionHandler : public QObject
{
    Q_OBJECT
public:
    explicit connectionHandler(QObject *parent = nullptr);
    ~connectionHandler();
    QTime currentTime;
    std::list<connectionElem> conList;
    std::list<busElem*> busList;
    void loadConnections(std::list<Street *> street_list);
    void resetBus(busElem* bus);
    auto createAltRoute(std::list<Street*> altStreets, connectionElem *connection, Street* closed);
    tupleElem findStreet(Street* currStreet, tupleList streetList, bool next);
    void createClosure(Street* closed, std::list<Street*> alternativeStreets);
    tupleList updateClosure(Street* closed, std::list<Street*> alternateStreets, tupleList streetList);
    tupleList shortenPath(tupleList streetList);

signals:
    void busUpdated();
public slots:
    void printConnections();
    void printBuses();
    void busUpdate();
 private:
    int timePassed;
};
#endif // CONNECTION_H
