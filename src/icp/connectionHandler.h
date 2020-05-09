#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>


class connectionHandler : public QObject
{
    Q_OBJECT
public:
    explicit connectionHandler(QObject *parent = nullptr);

signals:

public slots:
    void loadConnections();

 private:

};

//class c

#endif // CONNECTION_H
