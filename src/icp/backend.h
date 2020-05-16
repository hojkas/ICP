/**
 * @file backend.h
 * @author xlebod00
 * @brief Class for loading information from external files.
 */

#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QJsonDocument>
#include <QFile>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <fstream>
#include <err.h>

class BackEnd : public QObject
{
    Q_OBJECT
public:
    explicit BackEnd(QObject *parent = nullptr);


signals:

public slots:
    bool loadFile(QString file_path, QJsonDocument* jsonDocument);

private:
    QJsonDocument json_doc;
};

#endif // BACKEND_H
