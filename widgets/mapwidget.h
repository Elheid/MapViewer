#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QString>

#include <QOpenGLBuffer>

#include <QDebug>
#include "DataLoader.h"


//#include "draw_manager.h"


enum Axis{
    AxisX,
    AxisY
};

class MapWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    MapWidget(QWidget *parent = nullptr);
    void zoomIn();
    void zoomOut();

    void resetPosition();
    void resetZoom(){
        zoomNum = initZoom;
    }

    void changeTagsToExclude(std::vector<std::string> newTags){
        tagsToExclude = newTags;
        update();
    }

    void resetRotation();

    void changeDetalization(int detalization);
    void increaseDetalization();
    void decreaseDetalization();


    float initZoom = 1.0f;
    float initOffsetX = 0.0f;
    float initOffsetY = 0.0f;
    int detalization = 12;

    std::vector<std::string> allTagsToExclude = {"highway", "building", "natural" ,"waterway"};//это можно вынести в другой класс, предобработки
    std::vector<std::string> tagsToExclude = allTagsToExclude;//это можно вынести в другой класс, предобработки

    Bounds bounds;//это можно вынести в другой класс

    float rotationViewAngle = 0.0f;
protected:
    void initializeGL() override;//инициализация open gl
    void resizeGL(int w, int h) override;//функция при изменении размера окна
    void paintGL() override;//функция отрисовки

    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;


    void loadVisibleNodes();//это можно вынести в другой класс

    //через шейдер
    //это можно вынести в другой класс
    void drawGrid(const QMatrix4x4 &projection);
    void drawCross(const QMatrix4x4 &projection);

    void drawArrow(const QMatrix4x4 &projection);
    void drawLoadingArea(const QMatrix4x4 &projection);


    void paintData(const QMatrix4x4 &mvp, double minLon, double maxLon, double minLat, double maxLat); //это метод для новой версии бд

    void changeVectorOfInterest(float x1, float y1, float x2, float y2){//это можно вынести в другой класс
        vectorOfInterest.x1 = x1;
        vectorOfInterest.x2 = x2;
        vectorOfInterest.y1 = y1;
        vectorOfInterest.y2 = y2;
    }
    //

    void offsetChange(Axis axis,float shift, bool load);




private:
    //DrawManager* drawManager;

    float zoomNum = initZoom;
    float offsetY= initOffsetY;
    float offsetX = initOffsetX;
    QPoint lastMousePosition;
    QPoint mousePosition;

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


