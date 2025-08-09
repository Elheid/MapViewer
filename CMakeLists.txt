cmake_minimum_required(VERSION 3.14)
project(MapViewerNewData)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt

find_package(Qt5 COMPONENTS Core Widgets Concurrent OpenGL Sql Gui REQUIRED)
find_package(OpenGL REQUIRED)
#find_package(SQLite3 REQUIRED)

set(SQLIT3_DIR ${CMAKE_SOURCE_DIR}/sqlite3)

include_directories(${SQLIT3_DIR})



# Основной исполняемый файл
add_executable(MapViewerNewData
    main.cpp
    widgets/mapwidget.cpp
    widgets/mapwidget.h
    widgets/checkbox.h

    draw_manager.h
    draw_manager.cpp


    DataLoader.h
    DataLoader.cpp



    config.h
)

# Линки
target_link_libraries(MapViewerNewData
    Qt5::Core
    Qt5::Widgets
    Qt5::Sql
    Qt5::Concurrent
    Qt5::Gui
    #Qt5::OpenGL GL # linux
    #GL X11 #для linux
    opengl32 #для windows
    sqlite3
)
