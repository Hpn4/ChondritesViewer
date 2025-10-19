#include "MainWindow.h"
#include "ImageView.h"
#include <vector>
#include <string>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    central_ = new QWidget(this);
    grid_ = new QGridLayout(central_);

    std::vector<std::string> paths = {
        "resources/data/Mosaic element_BSE.tif",
        "resources/data/Mosaic element_Al.tif",
        "resources/data/Mosaic element_Ca.tif",
        "resources/data/Mosaic element_Fe.tif",
        "resources/data/Mosaic element_Mg.tif",
        "resources/data/Mosaic element_O.tif",
        "resources/data/Mosaic element_S.tif",
        "resources/data/Mosaic element_Si.tif"
    };

    int rows = 1, cols = 1;
    int idx = 0;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            auto *view = new ImageView(paths[idx++], this);
            //auto *view = new ImageView(this);
            grid_->addWidget(view, r, c);
            //sync_.registerView(view);
        }
    }

    setCentralWidget(central_);
    resize(1200, 800);
}

