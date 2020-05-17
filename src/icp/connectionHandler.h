/**
 * @file connectionHandler.h
 * @author xstrna14, xlebod00
 * @brief Class for storing information about connections.
 */

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

// Added for easier readability
// tupleElem represents a single street in for a connection.
// The tuple consists of a Street pointer, boolean to mark direction of travel
// and boolean to mark whether the street is a stop on the respective connection
using tupleElem = std::tuple<Street*, bool, bool>;
// tupleList represents a list of streets
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
    connectionElem* connnecton;
    int departure;
    int timeOnStreet;
    float x;
    float y;
    Street* curStreet;
    int streetIndex;
    bool returning;
    busElem(bool map, connectionElem *connnection, int dep, int tOS, int X, int Y, Street *strt, bool r);
};

class connectionHandler : public QObject
{
    Q_OBJECT
public:
    explicit connectionHandler(QObject *parent = nullptr);
    ~connectionHandler();
    // Variables for access in mapwidget
    QTime currentTime;
    std::list<connectionElem> conList;
    std::list<busElem*> busList;
    // Functions called from mapwidget.cpp
    void loadConnections(std::list<Street *> street_list);
    void resetBus(busElem* bus);
    void createClosure(Street* closed, std::list<Street*> alternativeStreets);

signals:
    // Bus postions updated
    void busUpdated();
public slots:
    // Functions called by &QTimer::timeout()
    void busUpdate();
    // Debug functions
    void printConnections();
    void printBuses();
    void printClosures();
 private:
    tupleElem findStreet(Street* currStreet, tupleList streetList, int streetIndex, bool next);
    tupleList updateClosure(Street* closed, std::list<Street*> alternateStreets, tupleList streetList);
    tupleList shortenPath(tupleList streetList);
    int timePassed;
};
#endif // CONNECTION_H
