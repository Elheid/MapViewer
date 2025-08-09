#include "draw_manager.h"
//он к сожалению почему-то аварийно завершается при инициализации
DrawManager::DrawManager(){
    dataLoader = new DataLoader();

    bounds.minlat = 56.171;
    bounds.maxlat = 56.62;
    bounds.minlon = 61.4;
    bounds.maxlon = 62.684;
    //dataLoader->loadFromDatabaseAll(dbFilename);

    //dataLoader->loadFromDatabase(dbFilename, 0, 180, 0, 180, 14);

    //loadVisibleNodes();
}

void DrawManager::Initialize(){
   /* initializeOpenGLFunctions();
    qDebug() << "draw start init";
    qDebug() << "open gl context" << QOpenGLContext::currentContext();
    shaderProgramm.addShaderFromSourceCode(QOpenGLShader::Vertex,
        "attribute vec2 a_position;\n"
        "uniform mat4 u_mvp;\n"
        "void main() {\n"
        "  gl_Position = u_mvp * vec4(a_position, 0.0, 1.0);\n"
        "  gl_PointSize = 10.0;\n"
        "}");
    shaderProgramm.addShaderFromSourceCode(QOpenGLShader::Fragment,
        "uniform vec4 u_color;\n"
        "void main() {\n"
        "  gl_FragColor = u_color;\n"
        "}");
    shaderProgramm.link();
    qDebug() << "draw start init shader prog link";

    // Создание VAO
    vao.create();
    vao.bind();

    // Создание VBO
    vboData = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vboData.create();
    vboData.bind();
    vboData.setUsagePattern(QOpenGLBuffer::DynamicDraw); // Можно изменить на StaticDraw

    // Настройка layout атрибутов
    shaderProgramm.bind();
    shaderProgramm.enableAttributeArray("a_position");
    shaderProgramm.setAttributeBuffer("a_position", GL_FLOAT, 0, 2);

    //
    vboData.release();
    vao.release();
    shaderProgramm.release();
    qDebug() << "draw manager init";*/
}

DrawManager::~DrawManager(){

}

void DrawManager::loadVisibleNodes(float zoomNum, int detalization, float width, float height, float tileSize, float offsetX, float offsetY){//double startPositionX, double startPositionY
    vboNeedsUpdate = false;
    double delta = tileSize / zoomNum ;


    double scaledOffsetX = linearScaling(offsetX, -1* width/2, width/2, bounds.minlon, bounds.maxlon);
    double scaledOffsetY = linearScaling(offsetY, -1* height/2, height/2, bounds.minlat, bounds.maxlat);

//   qDebug() << bounds.minlon << " bounds.minlon";
//   qDebug() << bounds.minlat << " bounds.minlat";
//   qDebug() << bounds.maxlon  << " bounds.maxlon";
//   qDebug() << bounds.maxlat  << " bounds.maxlat";


//   qDebug() << offsetX << " offsetX";
//   qDebug() << offsetY << " offsetY";
//   qDebug() << scaledOffsetX << " scaledOffsetX";
//   qDebug() << scaledOffsetY << " scaledOffsetY";




   double currCenterLat = /*(bounds.minlat + bounds.maxlat)/2*/ scaledOffsetY;
   double currCenterLon = /*(bounds.minlon + bounds.maxlon) /2*/scaledOffsetX;

   //qDebug() << currCenterLon << " currCenterLon";
   //qDebug() << currCenterLat << " currCenterLat";

   /*float lat1 = offsetY - delta;
   float lat2 = offsetY + delta;

   float lon1 = offsetX - delta;
   float lon2 = offsetX + delta;*/

   /*float intrestedDirectionX = std::clamp(abs(vectorOfInterest->x1 - vectorOfInterest->x2), 0.2f, 1.0f)/10;
   float intrestedDirectionY = std::clamp(abs(vectorOfInterest->y1 - vectorOfInterest->y2), 0.2f, 1.0f)/10;

   qDebug() << intrestedDirectionX << " intrestedDirectionX";
   qDebug() << intrestedDirectionY << " intrestedDirectionY";*/


   float lat1 = currCenterLat - delta;
   float lat2 = (currCenterLat + delta);

   float lon1 = currCenterLon - delta;
   float lon2 = (currCenterLon + delta);

//   qDebug() << delta << " delta";
//   qDebug() << lat1 << "- lat1, " << lat2 << " -lat2, " << lon1 << " -lon1, " << lon2 << " -lon2*/";

   dataLoader->shapesToDraw.clear();
   //dataLoader->loadFromDatabase(dbFilename, lat1, lat2, lon1, lon2, detalization);
   dataLoader->loadFromDatabaseWithTimer(dbFilename, lat1, lat2, lon1, lon2, detalization);
   qDebug() << "data loaded";
   //update();
}




//мб не нужна или по умолчанию откл, скорее всего
    //barrier:fence заборы не нужны мб
    //service:parking_aisle дорожка между рядами парковочных мест
    //highway:service маленькие дороги у домов
    //building:garage одинокий гараж

//не уверен, что не нужно на самом деле, спросить
//Можно порсто убирать до определённой детализации
    //public_transport:platform
    //power:line
    //amenity:parking парковки
const std::map<std::string, std::string> unnecesaryTags = {
    {"barrier","fence"},
    {"service","parking_aisle"},
    {"highway","service"},
    {"building","garage"},
    {"public_transport","platform"},
    {"power","line"},
    {"amenity","parking"},
};
void DrawManager::paintData(const QMatrix4x4 &mvp, double minLon, double maxLon, double minLat, double maxLat, int detalization, std::vector<std::string> tagsToExclude) {
    qDebug() << "draw manager start paint data";
    shaderProgramm.bind();
    shaderProgramm.setUniformValue("u_mvp", mvp);

    vao.bind();

    for (const auto &shapeToDraw : dataLoader->shapesToDraw) {
        if (excudeTypesByTag(shapeToDraw.tags, tagsToExclude)) continue;
        if (excudeTypes(shapeToDraw.tags, unnecesaryTags)) continue;

        QVector4D color = getColorForType(shapeToDraw.tags);
        shaderProgramm.setUniformValue("u_color", color);

        std::vector<GLfloat> vertices;

        for (const auto &[lon, lat] : shapeToDraw.points) {
            float x = linearScaling(lon, bounds.minlon, bounds.maxlon, minLon, maxLon);
            float y = linearScaling(lat, bounds.minlat, bounds.maxlat, minLat, maxLat);
            vertices.push_back(x);
            vertices.push_back(y);
        }

        // Пропускаем пустые или бессмысленные геометрии, только правильно ли это?
        if (vertices.size() < 4 && vertices.size() != 0){//Вроде бы таких нет даже, но на всякий
            qDebug() << "Не рисовалось" << vertices.size();
            continue;
        }
        auto length = calcLength(shapeToDraw.points);
        if (length < 0.000005 && detalization >= 13 && length != 0){
            qDebug() << "Не рисовалось" << length;
            continue;
        }

        vboData.bind();
        vboData.allocate(vertices.data(), vertices.size() * sizeof(GLfloat));

        // Выбор режима отрисовки
        if (vertices.size() == 2) {//Вроде бы их нет, но на всякий
            // 1 точка
            qDebug() << "Точка";
            glDrawArrays(GL_POINTS, 0, 2);
        }
        else if (vertices.size() == 4) {//их достаточно много рисуется
            // 2 точки → GL_LINES
            //qDebug() << "Линия";
            if (length == 0 && detalization >= 13) {
                qDebug() << "точка";
                glDrawArrays(GL_POINTS, 0, 2);
            }
            else glDrawArrays(GL_LINES, 0, 2);
        } else {//это основное, что рисуется
            // ≥ 3 точки → GL_LINE_STRIP
            glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 2);
        }

        vboData.release();
    }

    vao.release();
    shaderProgramm.release();
}

void DrawManager::drawGrid(const QMatrix4x4 &projection, float width, float height, float gridStep){
    //float lonSize = bounds.maxlon - bounds.minlon;
    //float latSize = bounds.maxlat - bounds.minlat;

    //float gridStep = 100.0f;//gcd(width(), height());//100.0f;

    //qDebug() << gcd(lonSize, latSize) * 1000.0f << " gcd(lonSize, latSize) * 1000.0f ";

    //size - gcd between min max, for lon and lat?

    std::vector<GLfloat> gridVertices;
    //qDebug() << visibleNodes.size();
     //нижняя граница
     gridVertices.push_back(0);
     gridVertices.push_back(height);
     gridVertices.push_back(width);
     gridVertices.push_back(height);
     //правая граница
     gridVertices.push_back(width);
     gridVertices.push_back(0);
     gridVertices.push_back(width);
     gridVertices.push_back(height);

    for(int x = 0; x <= width; x+=gridStep) {
        gridVertices.push_back(x);
        gridVertices.push_back(0);
        gridVertices.push_back(x);
        gridVertices.push_back(height);
    }

    for(int y = 0; y <= height; y+=gridStep) {
        gridVertices.push_back(0);
        gridVertices.push_back(y);
        gridVertices.push_back(width);
        gridVertices.push_back(y);
    }


    shaderProgramm.bind();
    shaderProgramm.setUniformValue("u_color", QVector4D(0.0f, 0.0f, 0.0f, 0.0f));
    shaderProgramm.setUniformValue("u_mvp", projection);

    shaderProgramm.enableAttributeArray("a_position");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderProgramm.setAttributeArray("a_position", GL_FLOAT, gridVertices.data(), 2);

    glDrawArrays(GL_LINES, 0, gridVertices.size()/2);
    shaderProgramm.disableAttributeArray("a_position");

}

void DrawManager::drawCross(const QMatrix4x4 &projection, float width, float height){
    float centerX = width/2.0f;
    float centerY = height/2.0f;
    std::vector<GLfloat> crossVertices;
    //qDebug() << visibleNodes.size();

    crossVertices.push_back(centerX-10);
    crossVertices.push_back(centerY);
    crossVertices.push_back(centerX+10);
    crossVertices.push_back(centerY);

    crossVertices.push_back(centerX);
    crossVertices.push_back(centerY-10);

    crossVertices.push_back(centerX);
    crossVertices.push_back(centerY+10);

   // glColor3f(1.0f, 0.0f, 0.0f);//лучше менять цвет по другому
    //glUniform4f(shaderProgramm.uniformLocation("u_color"), 1.0f, 0.0f, 0.0f, 1.0f);

    shaderProgramm.bind();
    shaderProgramm.setUniformValue("u_color", QVector4D(1.0f, 0.0f, 0.0f, 0.0f));
    shaderProgramm.setUniformValue("u_mvp", projection);

    shaderProgramm.enableAttributeArray("a_position");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderProgramm.setAttributeArray("a_position", GL_FLOAT, crossVertices.data(), 2);

    glDrawArrays(GL_LINES, 0, crossVertices.size()/2);

    shaderProgramm.disableAttributeArray("a_position");
}

void DrawManager::drawArrow(const QMatrix4x4 &projection, float width, float height, float offsetX, float offsetY){
    float centerX = width/2.0f;
    float centerY = height/2.0f;
    std::vector<GLfloat> arrowVertices;
    //qDebug() << visibleNodes.size();
    float min = -40.0f;
    float max = 40.0f;

    float normMouseX = centerX + std::clamp(offsetX, min, max); //linearScaling(mousePosition.x(), -1 * abs(mousePosition.x()-centerX), abs(mousePosition.x()-centerX), 0, 1);
    float normMouseY = centerY + std::clamp(offsetY, min, max);//linearScaling(mousePosition.y(), -1 * abs(mousePosition.y()-centerY), abs(mousePosition.y()-centerY), 0, 1);

    changeVectorOfInterest(centerX, centerY, normMouseX, normMouseY);

    arrowVertices.push_back(centerX);
    arrowVertices.push_back(centerY);

    arrowVertices.push_back(normMouseX);
    arrowVertices.push_back(normMouseY);


    //glColor3f(1.0f, 0.0f, 0.0f);//лучше менять цвет по другому через шейдер, только починить бы его
    //shaderProgramm.setUniformValue("u_color", QVector4D(1.0f, 0.0f, 0.0f, 0.0f));

    shaderProgramm.bind();
    shaderProgramm.setUniformValue("u_mvp", projection);

    shaderProgramm.setUniformValue("u_color", QVector4D(1.0f, 0.0f, 0.0f, 0.0f));


    shaderProgramm.enableAttributeArray("a_position");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderProgramm.setAttributeArray("a_position", GL_FLOAT, arrowVertices.data(), 2);

    glDrawArrays(GL_LINES, 0, arrowVertices.size()/2);
    shaderProgramm.disableAttributeArray("a_position");

    //glColor3f(0.0f, 0.0f, 0.0f);//лучше менять цвет по другому
}



/*
void MapWidget::drawLoadingArea(const QMatrix4x4 &projection){

    std::vector<GLfloat> areaVertices;
    //qDebug() << visibleNodes.size();

    double scaledLan1 = linearScaling(lon1, bounds.minlon, bounds.maxlon, -1, width()/2);
    double scaledLan2 = linearScaling(lon2, bounds.minlat, bounds.maxlat, -1, height()/2);
    double scaledLat1 = linearScaling(lat1, bounds.minlon, bounds.maxlon, -1, width()/2);
    double scaledLat2 = linearScaling(lat2, bounds.minlat, bounds.maxlat, -1, height()/2);

    qDebug() << "center " << width()/2 << height()/2;
    qDebug() << "scaledLan1 " << scaledLan1;
    qDebug() << "scaledLan2 " << scaledLan2;
    qDebug() << "scaledLat1 " << scaledLat1;
    qDebug() << "scaledLat2 " << scaledLat2;



    areaVertices.push_back(scaledLan1);
    areaVertices.push_back(scaledLat1);

    areaVertices.push_back(scaledLan1);
    areaVertices.push_back(scaledLat2);

    areaVertices.push_back(scaledLan2);
    areaVertices.push_back(scaledLat2);

    areaVertices.push_back(scaledLan2);
    areaVertices.push_back(scaledLat1);


   // glColor3f(1.0f, 0.0f, 0.0f);//лучше менять цвет по другому
    //glUniform4f(shaderProgramm.uniformLocation("u_color"), 1.0f, 0.0f, 0.0f, 1.0f);
    shaderProgramm.setUniformValue("u_color", QVector4D(1.0f, 0.0f, 0.0f, 0.0f));

    shaderProgramm.bind();
    shaderProgramm.setUniformValue("u_mvp", projection);

    shaderProgramm.enableAttributeArray("a_position");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderProgramm.setAttributeArray("a_position", GL_FLOAT, areaVertices.data(), 2);

    glDrawArrays(GL_LINES, 0, areaVertices.size()/2);

    shaderProgramm.disableAttributeArray("a_position");
}
*/
