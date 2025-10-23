#include "CollapsibleWidget.h"

#include <QPushButton>
#include <QVBoxLayout>

CollapsibleWidget::CollapsibleWidget(const QString& title, QWidget* contentWidget, QWidget* parent)
    : QWidget(parent), contentWidget_(contentWidget)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);

    // Header
    headerButton_ = new QPushButton(title);
    headerButton_->setCheckable(true);
    headerButton_->setChecked(true);
    headerButton_->setObjectName("headerButton");
    mainLayout->addWidget(headerButton_);

    // Contenu
    mainLayout->addWidget(contentWidget_);
    contentWidget_->setVisible(true);

    connect(headerButton_, &QPushButton::toggled, contentWidget_, &QWidget::setVisible);
}

void CollapsibleWidget::setCollapsed(bool collapsed)
{
    headerButton_->setChecked(!collapsed);
    contentWidget_->setVisible(!collapsed);
}

bool CollapsibleWidget::isCollapsed() const
{
    return !headerButton_->isChecked();
}
