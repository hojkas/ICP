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
    std::list<std::tuple<QString, bool>> streetList;
};

class busElem
{
    connectionElem con;
    time_t time;
    int x;
    int y;
    Street curStreet;
};

class connectionHandler : public QObject
{
    Q_OBJECT
public:
    explicit connectionHandler(QObject *parent = nullptr);
    std::list<connectionElem> conList;
signals:

public slots:
    void loadConnections();
    void printConnections();
 private:

};



#endif // CONNECTION_H
