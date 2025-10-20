#include "MainWindow.h"
#include "ImageView.h"
#include "ImageTransform.h"
#include "ImageLoader.h"

#include <vector>
#include <string>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    central_ = new QWidget(this);
    grid_ = new QGridLayout(central_);

    ImageLoader loader("resources/project.json");

    auto sharedTransform = std::make_shared<ImageTransform>();

    auto views = loader.prepareViews();

    for (const auto& [row, col, img, grayscale] : views) {
        auto *view = new ImageView(img, grayscale, sharedTransform, this);
        grid_->addWidget(view, row, col);
    }

    setCentralWidget(central_);
    resize(1200, 800);
}
