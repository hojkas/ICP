#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string>
#include <fstream>

class BackEnd : public QObject
{
    Q_OBJECT
public:
    explicit BackEnd(QObject *parent = nullptr);


signals:

public slots:
    bool loadFile(QString file_path, QJsonObject* jsonObject);

private:
    QJsonDocument json_doc;
    QJsonObject json_obj;
};

#endif // BACKEND_H
