#include "MainWindow.h"
#include "ImageView.h"
#include "ImageTransform.h"
#include "ImageLoader.h"
#include "PaintLabel.h"
#include "ImageLabel.h"

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

    auto sharedTransform = std::make_shared<ImageTransform>();
    auto sharedPaint = std::make_shared<PaintLabel>(loader.getWidth(), loader.getHeight());
    auto sharedLabel = std::make_shared<ImageLabel>();

    QOpenGLContext *mainCtx = new QOpenGLContext();
    mainCtx->setFormat(QSurfaceFormat::defaultFormat());
    mainCtx->create();

    //sharedPaint->setBrushLabel(1);
    //sharedPaint->setBrushRadius(500.f);

    auto views = loader.prepareViews();

    for (const auto& [row, col, img, grayscale] : views) {
        auto *view = new ImageView(img, grayscale, sharedTransform, this, mainCtx);
        //view->context()->setShareContext(mainCtx);
        view->setPaintLabel(sharedPaint);
        view->setImageLabel(sharedLabel);

        grid_->addWidget(view, row, col);
    }

    setCentralWidget(central_);
    resize(1200, 800);
}
