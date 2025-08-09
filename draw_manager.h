#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QString>

#include <QOpenGLBuffer>

#include <QDebug>

#include <GL/gl.h>

#include "config.h"
#include "DataLoader.h"
#include <cmath>

class DrawManager : protected QOpenGLFunctions {

public:
    DrawManager();
    ~DrawManager();

    Bounds bounds;//это можно вынести в другой класс

    void Initialize();
    void loadVisibleNodes(float zoomNum, int detalization,  float width, float height, float tileSize, float offsetX, float offsetY);//это можно вынести в другой класс

    //через шейдер
    //это можно вынести в другой класс
    void drawGrid(const QMatrix4x4 &projection, float width, float height, float gridStep);
    void drawCross(const QMatrix4x4 &projection, float width, float height);

    void drawArrow(const QMatrix4x4 &projection, float width, float height, float offsetX, float offsetY);
    void drawLoadingArea(const QMatrix4x4 &projection);


    void paintData(const QMatrix4x4 &mvp, double minLon, double maxLon, double minLat, double maxLat, int detalization, std::vector<std::string> tagsToExclude); //это метод для новой версии бд

    void changeVectorOfInterest(float x1, float y1, float x2, float y2){//это можно вынести в другой класс
        vectorOfInterest.x1 = x1;
        vectorOfInterest.x2 = x2;
        vectorOfInterest.y1 = y1;
        vectorOfInterest.y2 = y2;
    }
    //

    struct VectorOfInterest {//это можно вынести в другой класс
        float x1 = 0.0;
        float y1 = 0.0;
        float x2 = 0.0;
        float y2 = 0.0;
    };

    VectorOfInterest vectorOfInterest;//вектор текущего направления перемещения//это можно вынести в другой класс

    bool vboNeedsUpdate = true;//это можно вынести в другой класс
    std::vector<GLfloat> vertexData;//это можно вынести в другой класс
    int vertexCount;//это можно вынести в другой класс

    DataLoader* dataLoader;//это можно вынести в другой класс
    QOpenGLShaderProgram shaderProgramm;//это можно вынести в другой класс
    QOpenGLBuffer vboData;//это можно вынести в другой класс
    QOpenGLVertexArrayObject vao;//это можно вынести в другой класс

};
