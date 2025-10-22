#include "MainWindow.h"
#include "ImageView.h"
#include "ImageTransform.h"
#include "ImageLoader.h"
#include "PaintLabel.h"
#include "ImageLabel.h"
#include "SharedGLResources.h"

#include <vector>
#include <string>
#include <iostream>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QOpenGLWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    central_ = new QWidget(this);
    grid_ = new QGridLayout(central_);

    ImageLoader loader("resources/project.json");

    auto *sharedResources = new SharedGLResources(this);

    auto sharedTransform = std::make_shared<ImageTransform>();
    auto sharedPaint = std::make_shared<PaintLabel>(loader.getWidth(), loader.getHeight());

    //sharedPaint->setBrushLabel(1);
    //sharedPaint->setBrushRadius(500.f);

    grid_->addWidget(sharedResources, 0, 0);

    auto views = loader.prepareViews();

    for (const auto& [row, col, img, grayscale] : views) {
        auto *view = new ImageView(img, grayscale, sharedTransform, this, sharedResources);

        view->setPaintLabel(sharedPaint);

        grid_->addWidget(view, row, col);
    }

    setCentralWidget(central_);
    resize(1200, 800);
}
