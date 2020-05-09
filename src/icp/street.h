/**
 * @file street.h
 * @author xstrna14
 * @brief Class for creating and handling object Street representing street on map.
 */


#ifndef STREET_H
#define STREET_H

#include <QObject>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

class Street : public QObject
{
    Q_OBJECT
public:
    int x1, y1, x2, y2;
    QString name;
    int id, time, difficulty;

    explicit Street(QObject *parent = nullptr);
    Street(int x_1, int y_1, int x_2, int y_2, int s_id, QString s_name, QObject *parent = nullptr);

signals:

public slots:

};

class AllStreets : public QObject
{
    Q_OBJECT
private:

public:
    std::list<Street> street_list;
    explicit AllStreets(QObject *parent = nullptr);
    void loadStreets();

signals:

public slots:
};


#endif // STREET_H
