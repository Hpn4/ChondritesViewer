#include "ColorLabelList.h"

#include <QVBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QDebug>

ColorLabelList::ColorLabelList(QWidget* parent)
    : QFrame(parent)
{
    setObjectName("colorListContainer");

    contentLayout_ = new QVBoxLayout(this);
    contentLayout_->setContentsMargins(0,0,0,0);
    contentLayout_->setSpacing(2);

    buttonGroup_ = new QButtonGroup(this);
    buttonGroup_->setExclusive(true);

    struct LabelColor { QString name; QColor color; };
    LabelColor labels[] = {
        {"Chondre I", QColor("#577277")},
        {"Chondre II", QColor("#468232")},
        {"Matrix", QColor("#3c5e8b")},
        {"CAIs", QColor("#73bed3")},
        {"Carbonate", QColor("#a8ca58")},
        {"Soufre", QColor("#e7d5b3")},
        {"Fe Oxydé", QColor("#7a367b")},
        {"Fe", QColor("#a53030")},
        {"Epoxy", QColor("#da863e")},
        {"Mesostase I", QColor("#c65197")},
        {"Mesostase II", QColor("#884b2b")}
    };

    for (int i = 0; i < 11; ++i)
    {
        auto& lc = labels[i];
        QRadioButton* btn = new QRadioButton(lc.name);
        btn->setProperty("color", lc.color);
        btn->setProperty("index", i); // stocke l’index

        btn->setStyleSheet(QString(
            "QRadioButton {"
            "    min-height: 30px;"
            "    spacing: 8px;"
            "    color: white;"
            "    padding-left: 4px;"
            "    border-radius: 4px;"
            "    border: 0px;"
            "    background-color: transparent;"
            "}"
            "QRadioButton:hover { background-color: #444444; }"
            "QRadioButton::indicator {"
            "    width: 30px; height: 12px; border-radius: 5px;"
            "    background-color: %1;"
            "}"
            "QRadioButton::indicator:checked { border: 2px solid #4287f5; }"
            "QRadioButton:checked { background-color: #555555; }"
        ).arg(lc.color.name()));

        buttonGroup_->addButton(btn);
        contentLayout_->addWidget(btn);

        connect(btn, &QRadioButton::toggled, [btn, this](bool checked){
            if (checked)
            {
                int index = btn->property("index").toInt();
                emit labelSelected(index + 1);  // émet l’index
            }
        });
    }
}
