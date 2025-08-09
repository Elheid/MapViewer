#pragma once
#include <vector>
#include "config.h"
#include <sstream>
#include <iostream>
#include <sqlite3.h>
#include <QtDebug>
#include <QOpenGLFunctions>


#include <QOpenGLShaderProgram>

struct ShapeToDraw{
    std::vector<std::pair<double, double>> points;
    std::map<std::string, std::string> tags;
};

class DataLoader : protected QOpenGLFunctions {
public :
    DataLoader();
    ~DataLoader();

    std::vector<std::pair<double, double>> parseWKT(const std::string& wkt);

    sqlite3_stmt* getFromDB(sqlite3* db, double minLat, double maxLat, double minLon, double maxLon, int zoom);


    void loadFromDatabaseAll(const std::string& dbPath);
    std::map<std::string, std::string> deserializeTags(const std::string& str);
    void loadFromDatabase(const std::string& dbPath, double minLat, double maxLat, double minLon, double maxLon, int zoom);
    void loadFromDatabaseWithTimer(const std::string& dbPath, double minLat, double maxLat, double minLon, double maxLon, int zoom);
    void logQueryPerformance(double minLat, double maxLat, double minLon, double maxLon,
                                        int zoom, int rowCount, qint64 queryTime, qint64 totalTime);

    std::vector<ShapeToDraw> shapesToDraw;
};

