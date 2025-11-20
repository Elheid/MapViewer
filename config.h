#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <algorithm>
#include <sstream>

#include <GL/gl.h>
#include <QVector4D>
#include <cmath>
#include <QDebug>

//constexpr const char* dbFilename = "/home/zvarich/Projects/komarovSource/MapViewerWithShadersNewBd/database/kamenskWithEkb.sqlite";
//constexpr const char* filename = "/home/zvarich/Projects/komarovSource/MapViewerWithShaders/kamenskWithEkb.osm";


//constexpr const char* filename = "/home/zvarich/Projects/komarovSource/MapViewerWithShaders/kamenskWithEkb.osm";

//outputNewEpsilon2.sqlite //outputShorter
constexpr const char* dbFilename = "C:\\c++\\WKB\\MapViewerNewData\\database\\outputWKB_win10v2.sqlite";
//"/home/zvarich/Projects/komarovSource/MapViewerTiles/WKB/MapViewerNewData/database/outputWKB.sqlite";
constexpr const char* filename = "C:\\c++\\WKB\\MapViewerNewData\\kamensk.osm";
//"/home/zvarich/Projects/komarovSource/MapViewerTiles/WKB/MapViewerNewData/kamensk.osm";

const std::string logsFolder = "C:\\c++\\WKB\\MapViewerNewData\\database_metrics_shorter";
//"/home/zvarich/Projects/komarovSource/MapViewerTiles/WKB/MapViewerNewData/database_metrics_shorter";



/*struct Node {
    long long id;
    double lat, lon;
    std::string type;//Тип пока что для цвета проверка
    std::vector<long long> nodeRefs;//те что "связаны" т.е. одного типа подряд
};*/

/*struct Way {
    long long id;
    std::vector<long long> nodeRefs;
    std::string type; // например, highway
};*/

struct Bounds {
    float minlat;
    float maxlat;
    float minlon;
    float maxlon;
};

struct Way {
    long long id;
    int zoom;
    int tile_x;
    int tile_y;
    std::string geometry;
    std::string type;
    std::string tags;
};



static std::string strArrToString (const std::vector<std::string>& strArr){
    std::ostringstream oss;
    bool first;
    for (const auto& value: strArr){
        if (first) first = false;
        else oss << " ";
        oss << value << ";";
    }
    return oss.str();
}

static double normalizeScaling(double number, double origScaleLeft, double origScaleRight, double newScaleLeft, double newScaleRight){
    return static_cast<GLfloat>((number - origScaleLeft)/(origScaleRight - origScaleLeft)) * (newScaleRight - newScaleLeft);
}

static double linearScaling(double number, double origScaleLeft, double origScaleRight, double newScaleLeft, double newScaleRight){
    return newScaleLeft + ((number - origScaleLeft)/(origScaleRight - origScaleLeft)) * (newScaleRight - newScaleLeft);
}

static QVector4D getColorForType(std::map<std::string, std::string> tags){
    QVector4D green = {0.2f, 0.7f, 0.0f, 1.0f};
    //QVector4D wheet = {0.96f, 0.87f, 0.7f, 1.0f};
    QVector4D wheet = {0.96f, 0.87f, 0.7f, 1.0f};
    //QVector4D brown = {0.6f, 0.2f, 0.2f, 1.0f};

    QVector4D darkGrey = {0.7f, 0.7f, 0.7f, 1.0f};
    QVector4D blue = {0.0f, 0.0f, 1.0f, 1.0f};
    QVector4D black = {0.0f, 0.0f, 0.0f, 1.0f};
    if (tags.find("natural") != tags.end()){
        if (tags.find("natural")->second == "water") return blue;
        //if (tags.find("natural")->second == "water") return brown;
        return green;
    }
    else if (tags.find("building") != tags.end()){
        return darkGrey;
    }
    else if(tags.find("waterway") != tags.end()){
        return blue;
    }
    else{
        if (tags.find("landuse")->second == "farmland") return wheet;
        if (tags.find("landuse")->second == "grass") return green;
        return black;
    }
}
static auto calcLength = [](const std::vector<std::pair<double, double>>& pts) {
    double total = 0.0;
    for (std::size_t i = 1; i < pts.size(); ++i) {
        double dx = pts[i].first - pts[i-1].first;
        double dy = pts[i].second - pts[i-1].second;
        total += std::sqrt(dx * dx + dy * dy);
    }
    return total;
};

static std::string tagsToString (const std::map<std::string, std::string>& map){
    std::ostringstream oss;
    bool first;
    for (const auto& [k,v]: map){
        if (first) first = false;
        else oss << ", ";
        oss << k << ':' << v;
    }
    return oss.str();
}

static bool excudeTypesByTag(std::map<std::string, std::string> tags, std::vector<std::string> tagsKeysToExclude){
    //qDebug() << strArrToString(tagsKeysToExclude).c_str();
    for(auto tagKeyToExclude:tagsKeysToExclude){
        if (tags.find(tagKeyToExclude) != tags.end()){
            //qDebug() << tagToExclude.c_str();
            return true;
        }
        continue;
    }
    //qDebug() << tagsToString(tags).c_str();
    return false;
}

static bool excudeTypes(std::map<std::string, std::string> tags, std::map<std::string, std::string> tagsToExclude){
    //qDebug() << strArrToString(tagsKeysToExclude).c_str();
    for(auto [key, value]:tagsToExclude){
        auto findedKey = tags.find(key);
        if (findedKey != tags.end()){
            if (findedKey->second == value) {
                //qDebug() << tagsToExclude[key].c_str();
                return true;
            }
            else continue;
        }
        continue;
    }

    return false;
}




// #pragma once
// #include <vector>
// #include <iostream>
// #include <map>
// #include <algorithm>
// #include <sstream>

// #include <GL/gl.h>
// #include <QVector4D>
// #include <cmath>
// #include <QDebug>

// //constexpr const char* dbFilename = "/home/zvarich/Projects/komarovSource/MapViewerWithShadersNewBd/database/kamenskWithEkb.sqlite";
// //constexpr const char* filename = "/home/zvarich/Projects/komarovSource/MapViewerWithShaders/kamenskWithEkb.osm";

// //outputNewEpsilon2.sqlite //outputShorter //outputWKB
// constexpr const char* dbFilename = "/home/zvarich/Projects/komarovSource/MapViewerTiles/WKB/MapViewerNewData/database/outputWKB.sqlite";
// constexpr const char* filename = "/home/zvarich/Projects/komarovSource/MapViewerTiles/WKB/MapViewerNewData/kamensk.osm";


// //database_metrics_wkb
// //database_metrics_shorter
// //database_metrics
// const std::string logsFolder = "/home/zvarich/Projects/komarovSource/MapViewerTiles/WKB/MapViewerNewData/database_metrics";



// /*struct Node {
//     long long id;
//     double lat, lon;
//     std::string type;//Тип пока что для цвета проверка
//     std::vector<long long> nodeRefs;//те что "связаны" т.е. одного типа подряд
// };*/

// /*struct Way {
//     long long id;
//     std::vector<long long> nodeRefs;
//     std::string type; // например, highway
// };*/

// struct Bounds {
//     float minlat;
//     float maxlat;
//     float minlon;
//     float maxlon;
// };

// struct Way {
//     long long id;
//     int zoom;
//     int tile_x;
//     int tile_y;
//     std::string geometry;
//     std::string type;
//     std::string tags;
// };



// static std::string strArrToString (const std::vector<std::string>& strArr){
//     std::ostringstream oss;
//     bool first;
//     for (const auto& value: strArr){
//         if (first) first = false;
//         else oss << " ";
//         oss << value << ";";
//     }
//     return oss.str();
// }

// static double normalizeScaling(double number, double origScaleLeft, double origScaleRight, double newScaleLeft, double newScaleRight){
//     return static_cast<GLfloat>((number - origScaleLeft)/(origScaleRight - origScaleLeft)) * (newScaleRight - newScaleLeft);
// }

// static double linearScaling(double number, double origScaleLeft, double origScaleRight, double newScaleLeft, double newScaleRight){
//     return newScaleLeft + ((number - origScaleLeft)/(origScaleRight - origScaleLeft)) * (newScaleRight - newScaleLeft);
// }

// static QVector4D getColorForType(std::map<std::string, std::string> tags){
//     QVector4D green = {0.2f, 0.7f, 0.0f, 1.0f};
//     //QVector4D wheet = {0.96f, 0.87f, 0.7f, 1.0f};
//     QVector4D wheet = {0.96f, 0.87f, 0.7f, 1.0f};
//     //QVector4D brown = {0.6f, 0.2f, 0.2f, 1.0f};

//     QVector4D darkGrey = {0.7f, 0.7f, 0.7f, 1.0f};
//     QVector4D blue = {0.0f, 0.0f, 1.0f, 1.0f};
//     QVector4D black = {0.0f, 0.0f, 0.0f, 1.0f};
//     if (tags.find("natural") != tags.end()){
//         if (tags.find("natural")->second == "water") return blue;
//         //if (tags.find("natural")->second == "water") return brown;
//         return green;
//     }
//     else if (tags.find("building") != tags.end()){
//         return darkGrey;
//     }
//     else if(tags.find("waterway") != tags.end()){
//        return blue;
//     }
//     else{
//        if (tags.find("landuse")->second == "farmland") return wheet;
//        if (tags.find("landuse")->second == "grass") return green;
//        return black;
//     }
// }
// static auto calcLength = [](const std::vector<std::pair<double, double>>& pts) {
//     double total = 0.0;
//     for (std::size_t i = 1; i < pts.size(); ++i) {
//         double dx = pts[i].first - pts[i-1].first;
//         double dy = pts[i].second - pts[i-1].second;
//         total += std::sqrt(dx * dx + dy * dy);
//     }
//     return total;
// };

// static std::string tagsToString (const std::map<std::string, std::string>& map){
//     std::ostringstream oss;
//     bool first;
//     for (const auto& [k,v]: map){
//         if (first) first = false;
//         else oss << ", ";
//         oss << k << ':' << v;
//     }
//     return oss.str();
// }

// static bool excudeTypesByTag(std::map<std::string, std::string> tags, std::vector<std::string> tagsKeysToExclude){
//     //qDebug() << strArrToString(tagsKeysToExclude).c_str();
//     for(auto tagKeyToExclude:tagsKeysToExclude){
//         if (tags.find(tagKeyToExclude) != tags.end()){
//             //qDebug() << tagToExclude.c_str();
//             return true;
//         }
//         continue;
//     }
//     //qDebug() << tagsToString(tags).c_str();
//     return false;
// }

// static bool excudeTypes(std::map<std::string, std::string> tags, std::map<std::string, std::string> tagsToExclude){
//     //qDebug() << strArrToString(tagsKeysToExclude).c_str();
//     for(auto [key, value]:tagsToExclude){
//         auto findedKey = tags.find(key);
//         if (findedKey != tags.end()){
//             if (findedKey->second == value) {
//                 //qDebug() << tagsToExclude[key].c_str();
//                 return true;
//             }
//             else continue;
//         }
//         continue;
//     }

//     return false;
// }



