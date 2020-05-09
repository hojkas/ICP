#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>


class connection : public QObject
{
    Q_OBJECT
public:
    explicit connection(QObject *parent = nullptr);

signals:

public slots:
    void loadConnections();

 private:

};

class stringList{
public:
    QString stringVal;
    stringList *nextElem;
};

#endif // CONNECTION_H
