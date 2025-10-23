#include "MainWindow.h"
#include "ImageView.h"
#include "ImageTransform.h"
#include "ImageLoader.h"
#include "PaintLabel.h"
#include "ImageLabel.h"
#include "SharedGLResources.h"
#include "widgets/ColorLabelList.h"
#include "widgets/CollapsibleWidget.h"

#include <vector>
#include <string>
#include <iostream>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QOpenGLWidget>
#include <QDebug>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    central_ = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(central_);
    mainLayout->setContentsMargins(0,0,0,0);

    // ---- Left panel ----
    QWidget* leftPanel = new QWidget;
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftPanel->setFixedWidth(200);

    ColorLabelList* labelSection = new ColorLabelList();

    CollapsibleWidget* collapsible = new CollapsibleWidget("Labels", labelSection);
    leftLayout->addWidget(collapsible);

    leftLayout->addStretch();

    mainLayout->addWidget(leftPanel);

    // ---- Right panel ----
    QWidget* rightPanel = new QWidget;
    QGridLayout* grid_ = new QGridLayout(rightPanel);

    ImageLoader loader("resources/project.json");
    auto *sharedResources = new SharedGLResources(loader.getWidth(), loader.getHeight(), this);
    auto sharedTransform = std::make_shared<ImageTransform>();

    grid_->addWidget(sharedResources, 0, 0);
    grid_->setContentsMargins(0,0,0,0);

    auto views = loader.prepareViews();
    for (const auto& [row, col, img, grayscale] : views) {
        auto *view = new ImageView(img, grayscale, sharedTransform, this, sharedResources);
        grid_->addWidget(view, row, col);

        connect(labelSection, &ColorLabelList::labelSelected,
        view, &ImageView::onLabelSelected);
    }

    mainLayout->addWidget(rightPanel);
    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 1);

    setCentralWidget(central_);
    resize(1200, 800);
}
