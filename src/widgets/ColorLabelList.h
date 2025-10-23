#pragma once

#include <QWidget>
#include <QFrame>
#include <QColor>

class QVBoxLayout;
class QButtonGroup;

class ColorLabelList : public QFrame {
    Q_OBJECT
public:
    explicit ColorLabelList(QWidget* parent = nullptr);

signals:
    void labelSelected(int index);

private:
    QVBoxLayout* contentLayout_;
    QButtonGroup* buttonGroup_;
};
