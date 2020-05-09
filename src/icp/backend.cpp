#include "backend.h"


BackEnd::BackEnd(QObject *parent) : QObject(parent)
{

}

bool BackEnd::loadFile(QString file, QJsonObject* jsonObject)
{
    // Otvorit file
    QFile file_obj(file);
    // Overime ci je file spravne nacitany
    if(!file_obj.exists()){
        std::cout << "Could not find file: " << file.toUtf8().constData() << std::endl;
        exit(1);
    }
    if (!file_obj.open(QIODevice::ReadOnly)) {
        std::cout << "Failed to open file " << file.toUtf8().constData() <<std::endl;
        exit(1);
        }
    // Nacitame text zo suboru do QByteArray a dalej convertujeme na JsonDocument
    const QByteArray json_bytes = file_obj.readAll();
    file_obj.close();
    json_doc = QJsonDocument::fromJson(json_bytes);
    // Overi spravne vytvorenie JsonDocumentu
    if (json_doc.isNull()) {
        std::cout << "Failed to create JSON doc." << std::endl;
        return false;
    }
    if (!json_doc.isObject()) {
        std::cout << "JSON is not an object." << std::endl;
        return false;
    }
    // Konverujeme JsonDocument na JsonObject
    json_obj = json_doc.object();
    QJsonArray json_arr = json_doc.array();
    // Overime spravnost objektu
    if (json_obj.isEmpty()) {
        std::cout << "JSON object is empty." << std::endl;
        return false;
    }

    *jsonObject = json_obj;
    return true;
}
