#include <QApplication>


#include <QVBoxLayout>
#include <QDebug>

#include <iostream>
#include "widgets/mapwidget.h"


#include <QPushButton>
#include <QObject>
#include <QLabel>
#include <QCheckBox>

#include "widgets/checkbox.h"


int main(int argc, char *argv[]) {
    std::cout << "start app" << std::endl;

    QApplication a(argc, argv);
    QApplication::setAttribute(Qt::AA_ForceRasterWidgets, false);

    // Главное окно
    QWidget window;
    QHBoxLayout* mainLayout = new QHBoxLayout(&window);

    // карта слева
    MapWidget* map = new MapWidget;
    map->setFocusPolicy(Qt::StrongFocus);
    mainLayout->addWidget(map, /*stretch*/ 1); // занимает всё доступное пространство

    // кнопки управления справа
    QWidget* controlPanel = new QWidget;
    QVBoxLayout* controlLayout = new QVBoxLayout(controlPanel);


    // Кнопка сброса
    QPushButton* resetButton = new QPushButton("Reset");
    controlLayout->addWidget(resetButton);
    QObject::connect(resetButton, &QPushButton::clicked, [=]() {
        map->resetPosition();
        map->resetZoom();
        map->resetRotation();
    });

    std::vector<std::string> tagsCheckbox =  map->tagsToExclude;
    /*// Надпись текущих исключенных тегов/объектов
    for (std::string tag : tagsCheckbox){
        QLabel* tagLabel = new QLabel;
        std::string tagsLabelText = " - " + tag;
        tagLabel->setText(QString(tagsLabelText.c_str()));
        tagLabel->setMargin(2);
        controlLayout->addWidget(tagLabel);
    }*/
    /*QLabel* tagsLabelHeader = new QLabel;
    tagsLabelHeader->setText(QString("Excluded objects: "));
    controlLayout->addWidget(tagsLabelHeader);*/

    CheckboxWidget* checkBoxWidget = new CheckboxWidget(map);
    controlLayout->addWidget(checkBoxWidget);

    // Надпись текущеог уровня детализации
    /*QLabel* label = new QLabel;
    label->setText(QString("Detalization: %1").arg(map->detalization));
    controlLayout->addWidget(label);*/

    // Увеличить детализацию
    QPushButton* buttonInc = new QPushButton("+ detalization");
    controlLayout->addWidget(buttonInc);
    QObject::connect(buttonInc, &QPushButton::clicked, [=]() {
        map->increaseDetalization();
        //label->setText(QString("Detalization: %1").arg(map->detalization));
    });

    // Уменьшить детализацию
    QPushButton* buttonDec = new QPushButton("- detalization");
    controlLayout->addWidget(buttonDec);
    QObject::connect(buttonDec, &QPushButton::clicked, [=]() {
        map->decreaseDetalization();
        //label->setText(QString("Detalization: %1").arg(map->detalization));
    });

    // Кнопка скрытия/показа панели
    /*QPushButton* togglePanel = new QPushButton("Toggle Panel");
    controlLayout->addWidget(togglePanel);
    QObject::connect(togglePanel, &QPushButton::clicked, [=]() {
        controlPanel->setVisible(!controlPanel->isVisible());
    });*/

    mainLayout->addWidget(controlPanel); // можно добавить stretch 0

    window.resize(1600, 900);
    window.setLayout(mainLayout);
    window.show();

    return a.exec();
}
