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
    connectionElem* con;
    int departure;
    int sinceDeparture;
    int x;
    int y;
    Street* curStreet;
    bool returning;
};

class connectionHandler : public QObject
{
    Q_OBJECT
public:
    explicit connectionHandler(QObject *parent = nullptr);
    std::list<connectionElem> conList;
    std::list<busElem> busList;
signals:

public slots:
    void loadConnections(std::list<Street *> street_list);
    void printConnections();
    Street* findStreet(int id, std::list<Street*> streetList);
 private:

};
#endif // CONNECTION_H
