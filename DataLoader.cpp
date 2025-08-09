#include "DataLoader.h"



DataLoader::DataLoader(){}

std::map<std::string, std::string> DataLoader::deserializeTags(const std::string& str){
    std::map<std::string, std::string> result;
    std::stringstream ss(str);
    std::string token;


    while (std::getline(ss, token, ';')){
        auto pos = token.find("=");
        if(pos != std::string::npos){
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos+1);
            result[key]=value;
        }
    }
    return result;
}

std::vector<std::pair<double, double>> DataLoader::parseWKT(const std::string& wkt){
    std::vector<std::pair<double, double>> points;

    std::size_t start = wkt.find("(");
    std::size_t end = wkt.find(")");
    if (start == std::string::npos || end == std::string::npos) return points;

    std::string coordStr = wkt.substr(start + 1, end - start - 1);
    std::istringstream iss(coordStr);
    std::string token;

    while (std::getline(iss, token, ',')) {
        std::istringstream pairStream(token);
        double lon, lat;
        pairStream >> lon >> lat;
        if(pairStream.fail()) continue; //добавил недавно, должно помочб с острыми углами?
        points.emplace_back(lon, lat);
    }
    return points;
}

std::vector<std::pair<double, double>> parseWKB(const std::string& wkb) {//wkb  а не wkt
    std::vector<std::pair<double, double>> points;

    if (wkb.size() < 1 + 4 + 4) return points;

    const char* ptr = wkb.data();
    ptr += 1 + 4; // Пропускаем byte order и тип

    uint32_t numPoints;
    memcpy(&numPoints, ptr, 4);
    ptr += 4;

    points.reserve(numPoints);
    for (uint32_t i = 0; i < numPoints; ++i) {
        double lon, lat;
        memcpy(&lon, ptr, 8);
        ptr += 8;
        memcpy(&lat, ptr, 8);
        ptr += 8;
        points.emplace_back(lon, lat);
    }

    return points;
}



void DataLoader::loadFromDatabaseAll(const std::string& dbPath){
    qDebug() << "start load from db";
    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        qDebug() << "Can't open db:" << sqlite3_errmsg(db);
        return;
    }

    //const char* sql = "SELECT geom FROM features;"; // тестовый лимит

    const char* sql = "SELECT ways.geometry , ways.tags "
    "FROM ways_index "
    "JOIN ways ON ways.id = ways_index.id "
    "WHERE maxX >= 0 AND minX <= 180 "
    "AND maxY >= 0 AND minY <= 180 "
    "AND ways.zoom = 14;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {

        qDebug() << "Query failed:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        return;
    }
    qDebug() << "loaded data, start preparing";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt,0);//wkt


        //int size = sqlite3_column_bytes(stmt, 0);
        const unsigned char* tags = sqlite3_column_text(stmt,1);

        if (text){
            auto geom = parseWKT(reinterpret_cast<const char*>(text));
            //auto geom = parseWKB(reinterpret_cast<const char*>(text));
            auto tagsMap = deserializeTags(reinterpret_cast<const char*>(tags));
            shapesToDraw.push_back(ShapeToDraw{geom, tagsMap});
        }
    }




    sqlite3_finalize(stmt);
    sqlite3_close(db);
}


void DataLoader::loadFromDatabase(const std::string& dbPath, double minLat, double maxLat, double minLon, double maxLon, int zoom){
    //std::vector<WKTData> result;

    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        qDebug() << "Can't open db:" << sqlite3_errmsg(db);
        return;
    }

    const char* sql = "SELECT ways.geometry, ways.tags "
    "FROM ways_index "
    "JOIN ways ON ways.id = ways_index.id "
    "WHERE maxX >= ? AND minX <= ? "
    "AND maxY >= ? AND minY <= ? "
    "AND ways.zoom = ?;";

    sqlite3_stmt* stmt;

    // Подготовка и выполнение запроса
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        qDebug() << "Prepare error:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_double(stmt, 1, minLon);
    sqlite3_bind_double(stmt, 2, maxLon);
    sqlite3_bind_double(stmt, 3, minLat);
    sqlite3_bind_double(stmt, 4, maxLat);
    sqlite3_bind_int(stmt, 5, zoom);



    while (sqlite3_step(stmt) == SQLITE_ROW) {

        const void* blob = sqlite3_column_blob(stmt,0);
        int blob_size = sqlite3_column_bytes(stmt,0);
        std::string wkb;
        wkb.assign(reinterpret_cast<const char*>(blob), blob_size);
        const char* text = wkb.c_str();

        //const unsigned char* text = sqlite3_column_text(stmt,0);
        //int size = sqlite3_column_bytes(stmt, 0);
        const unsigned char* tags = sqlite3_column_text(stmt,1);
        if (text){

            //auto geom = parseWKT(reinterpret_cast<const char*>(text));
            auto geom = parseWKB(wkb);
            /*for (auto [first, second]: geom){
                qDebug() << "parsed geom";
                qDebug() << first << second;
            }*/
            auto tagsChar = reinterpret_cast<const char*>(tags);
            auto tagsMap = deserializeTags(tagsChar);
            //std::map<std::string, std::string> tagsMap = {"", ""};
            shapesToDraw.push_back(ShapeToDraw{geom, tagsMap});

        }
    }
    //sqlite3_stmt* stmt = getFromDB(db, minLat, maxLat, minLon, maxLon, zoom);

    sqlite3_finalize(stmt);

    //char* resQuery = sqlite3_expanded_sql(stmt);
    qDebug()<< shapesToDraw.size() << " nodes searched with query - "; //<< resQuery;
    return;

}


#include <QElapsedTimer>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>

void DataLoader::loadFromDatabaseWithTimer(const std::string& dbPath, double minLat, double maxLat, double minLon, double maxLon, int zoom) {
    QElapsedTimer totalTimer;
    totalTimer.start();

    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        qDebug() << "Can't open db:" << sqlite3_errmsg(db);
        return;
    }


    // Замер времени выполнения SQL-запроса
    QElapsedTimer queryTimer;
    queryTimer.start();

    sqlite3_stmt* stmt;

    const char* sql = "SELECT ways.geometry , ways.tags "
                      "FROM ways_index "
                      "JOIN ways ON ways.id = ways_index.id "
                      "WHERE maxX >= ? AND minX <= ? "
                      "AND maxY >= ? AND minY <= ? "
                      "AND ways.zoom = ? ;";


    // Подготовка и выполнение запроса
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        qDebug() << "Prepare error:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_double(stmt, 1, minLon);
    sqlite3_bind_double(stmt, 2, maxLon);
    sqlite3_bind_double(stmt, 3, minLat);
    sqlite3_bind_double(stmt, 4, maxLat);
    sqlite3_bind_int(stmt, 5, zoom);

    int rowCount = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        const void* blob = sqlite3_column_blob(stmt,0);
        int blob_size = sqlite3_column_bytes(stmt,0);
        std::string wkb;
        wkb.assign(reinterpret_cast<const char*>(blob), blob_size);
        const char* text = wkb.c_str();

        //const unsigned char* text = sqlite3_column_text(stmt,0);
        //int size = sqlite3_column_bytes(stmt, 0);
        const unsigned char* tags = sqlite3_column_text(stmt,1);
        if (text){

            //auto geom = parseWKT(reinterpret_cast<const char*>(text));
            auto geom = parseWKB(wkb);
            /*for (auto [first, second]: geom){
                qDebug() << "parsed geom";
                qDebug() << first << second;
            }*/
            auto tagsChar = reinterpret_cast<const char*>(tags);
            auto tagsMap = deserializeTags(tagsChar);
            //std::map<std::string, std::string> tagsMap = {"", ""};
            shapesToDraw.push_back(ShapeToDraw{geom, tagsMap});
            rowCount++;
        }
    }


    qint64 queryTime = queryTimer.elapsed(); // Время выполнения запроса в мс
    qint64 totalTime = totalTimer.elapsed(); // Общее время работы метода

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // Логирование результатов
    logQueryPerformance(minLat, maxLat, minLon, maxLon, zoom, rowCount, queryTime, totalTime);

    qDebug() << shapesToDraw.size() << " nodes searched in " << queryTime << "ms";
}

void DataLoader::logQueryPerformance(double minLat, double maxLat, double minLon, double maxLon,
                                    int zoom, int rowCount, qint64 queryTime, qint64 totalTime) {
    // Создаем папку для логов если её нет
    QDir().mkpath(logsFolder.c_str());
    //qDebug() << "Создана папка для логов в " << logsFolder.c_str();

    // Файл с датой в имени
    std::string metric = "/query_metrics_%1.log";
    std::string logs = logsFolder + metric;
    //qDebug() << "Создана лог файл в " << logs.c_str();
    QString fileName = QString(logs.c_str())
                      .arg(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm] "));//("yyyy_MM_dd"));

    QFile file(fileName);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz] ");
        stream << "Query: BBOX("
               << minLat << "," << maxLat << ","
               << minLon << "," << maxLon << "), ";
        stream << "Zoom: " << zoom << ", ";
        stream << "Rows: " << rowCount << ", ";
        stream << "SQL Time: " << queryTime << "ms, ";
        stream << "Total Time: " << totalTime << "ms\n";
    }
    //qDebug() << "Записанор в логи";
}
