#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QDebug>
#include <QLabel>
#include <vector>
#include <string>
#include "mapwidget.h"


class CheckboxWidget : public QWidget {
    Q_OBJECT
public:
    explicit CheckboxWidget(MapWidget* map, QWidget *parent = nullptr)
        : QWidget(parent) {
        // Исходные данные
        originalArray = map->allTagsToExclude;
        checkedFlags.resize(originalArray.size(), true);
        updateFilteredArray(); // Инициализация фильтрованного массива

        // Создаем макет
        QVBoxLayout *layout = new QVBoxLayout(this);

        QLabel* tagsLabelHeader = new QLabel;
        tagsLabelHeader->setText(QString("Excluded objects: "));
        layout->addWidget(tagsLabelHeader);


        // Создаем чекбоксы для каждого элемента
        for (std::size_t i = 0; i < originalArray.size(); ++i) {
            QCheckBox *cb = new QCheckBox(QString::fromStdString(originalArray[i]), this);
            cb->setChecked(true);

            // Связываем сигнал изменения состояния с обработчиком
            connect(cb, &QCheckBox::stateChanged, [this, i, map](int state) {
                checkedFlags[i] = (state == Qt::Checked);
                updateFilteredArray();
                //logArrays(); // Для демонстрации (можно убрать)
                map->changeTagsToExclude(filteredArray);
            });

            layout->addWidget(cb);
        }

        // Кнопка для отображения текущего состояния
        /*QPushButton *btnShow = new QPushButton("Show Filtered Array", this);
        connect(btnShow, &QPushButton::clicked, [this]() {
            qDebug() << "Current filtered array:";
            for (const auto& s : filteredArray) {
                qDebug() << QString::fromStdString(s);
            }
        });
        layout->addWidget(btnShow);*/
    }

    const std::vector<std::string>& getFilteredArray() const {
        return filteredArray;
    }

private:
    void updateFilteredArray() {
        filteredArray.clear();
        for (std::size_t i = 0; i < originalArray.size(); ++i) {
            if (checkedFlags[i]) {
                filteredArray.push_back(originalArray[i]);
            }
        }
    }

    void logArrays() {
        qDebug() << "Original array size:" << originalArray.size();
        qDebug() << "Filtered array size:" << filteredArray.size();
    }

    std::vector<std::string> originalArray;   // Исходный массив (не изменяется)
    std::vector<bool> checkedFlags;            // Состояния чекбоксов
    std::vector<std::string> filteredArray;    // Динамически обновляемый массив
};
