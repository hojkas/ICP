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

class connectionElem
{
public:
    QString name;
    std::list<std::tuple<Street*, bool, bool>> streetList;
};

class busElem
{
public:
    bool onMap;
    connectionElem* con;
    int departure;
    int timeOnStreet;
    int x;
    int y;
    Street* curStreet;
    bool returning;
    busElem(bool map, connectionElem *connnection, int dep, int tOS, int X, int Y, Street *strt, bool r);
};

class connectionHandler : public QObject
{
    Q_OBJECT
public:
    explicit connectionHandler(QObject *parent = nullptr);
    QTime currentTime;
    std::list<connectionElem> conList;
    std::list<busElem*> busList;
    void loadConnections(std::list<Street *> street_list);
    void resetBus(busElem* bus);
    std::tuple<Street*, bool, bool> findStreet(Street* currStreet, std::list<std::tuple<Street*, bool, bool>> streetList, bool next);
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
