/**
 * @file backend.h
 * @author xlebod00
 * @brief Class for loading information from external files.
 */

#include "backend.h"


BackEnd::BackEnd(QObject *parent) : QObject(parent)
{

}

bool BackEnd::loadFile(QString file, QJsonDocument* jsonDocument)
{
    // Otvorit file
    QFile file_obj(file);
    // Overime ci je file spravne nacitany
    if(!file_obj.exists()){
        std::cerr << "Could not find file: " << file.toUtf8().constData() << std::endl;
        exit(1);
    }
    if (!file_obj.open(QIODevice::ReadOnly)) {
        std::cerr << "Failed to open file " << file.toUtf8().constData() <<std::endl;
        exit(1);
        }
    // Nacitame text zo suboru do QByteArray a dalej convertujeme na JsonDocument
    const QByteArray json_bytes = file_obj.readAll();
    file_obj.close();
    json_doc = QJsonDocument::fromJson(json_bytes);
    // Overi spravne vytvorenie JsonDocumentu
    if (json_doc.isNull()) {
        std::cerr << "Failed to create JSON doc." << std::endl;
        return false;
    }
    *jsonDocument = json_doc;
    return true;
}
