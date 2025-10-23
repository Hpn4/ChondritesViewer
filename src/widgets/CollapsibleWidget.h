#pragma once

#include <QWidget>

class QPushButton;
class QVBoxLayout;

class CollapsibleWidget : public QWidget {
    Q_OBJECT
public:
    explicit CollapsibleWidget(const QString& title, QWidget* contentWidget, QWidget* parent = nullptr);

    void setCollapsed(bool collapsed);
    bool isCollapsed() const;

private:
    QPushButton* headerButton_;
    QWidget* contentWidget_;
};
