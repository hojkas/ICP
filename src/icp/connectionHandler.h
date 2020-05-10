#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
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
    std::list<connectionElem> conList;
    std::list<busElem*> busList;
signals:

public slots:
    void loadConnections(std::list<Street *> street_list);
    void printConnections();
    std::tuple<Street*, bool, bool> findStreet(Street* currStreet, std::list<std::tuple<Street*, bool, bool>> streetList, bool next);
    void internalTimer();
    void busUpdate();
 private:
    int timePassed;
    int timeSinceUpdate;
};
#endif // CONNECTION_H
