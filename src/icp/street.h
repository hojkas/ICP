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

class Street : public QObject
{
    Q_OBJECT
private:
    int x1, y1, x2, y2;


public:
    explicit Street(QObject *parent = nullptr);
    Street(int x_1, int y_1, int x_2, int y_2);

signals:

public slots:
    void loadStreets();
};

#endif // STREET_H
