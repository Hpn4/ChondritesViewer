#include "ImageLoader.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace std;

ImageLoader::ImageLoader(const std::string &path) 
    : path_(path) {
    load();
}

VImage ImageLoader::loadSingle(const std::string &path) {
    if(VIPS_INIT("ImageLoader"))
        throw std::runtime_error("Impossible d'initialiser libvips");

    VImage img = VImage::new_from_file(path.c_str(),
                                       VImage::option()->set("access","random"));

    width_ = img.width();
    height_ = img.height();

    // RGB to grayscale, only one channel holds the data
    VImage r = img[0];
    VImage g = img[1];
    VImage b = img[2];

    VImage gray = r.ifthenelse(r, g.ifthenelse(g, b));

    return gray;
}

void ImageLoader::load() {
    ifstream f(path_);
    if (!f.is_open()) {
        cerr << "Failed to open JSON file!" << endl;
        return;
    }
    data_ = json::parse(f);
    
    if (data_.contains("imgs") && data_["imgs"].is_object()) {
        for (auto& [key, pathJson] : data_["imgs"].items()) {
            string path = pathJson.get<string>();
            
            try {
                VImage img = loadSingle(path);
                imageMap_[key] = img;
                height_ = img.height();
                width_ = img.width();

                cout << "Loaded image for " << key << " from " << path << endl;
            } catch (const exception& e) {
                cerr << "Failed to load image " << key << ": " << e.what() << endl;
            }
        }
    } else {
        cerr << "\"imgs\" section missing or invalid in JSON" << endl;
    }

    // Add dummy black image
    imageMap_["0"] = VImage::black(width_, height_);
}

std::vector<std::tuple<int,int,VImage,bool>> ImageLoader::prepareViews() const {
    std::vector<std::tuple<int,int,VImage,bool>> views;

    if (!data_.contains("views") || !data_["views"].contains("datas"))
        return views;

    for (const auto& viewData : data_["views"]["datas"]) {
        int row = viewData.value("row", 0);
        int col = viewData.value("col", 0);
        bool grayscale = viewData.value("grayscale", true);

        VImage img;

        if (grayscale) {
            std::string chan = viewData.value("r", "0");
            img = imageMap_.at(chan);
        } else {
            std::string rChan = viewData.value("r", "0");
            std::string gChan = viewData.value("g", "0");
            std::string bChan = viewData.value("b", "0");

            img = VImage::bandjoin({
                imageMap_.at(rChan),
                imageMap_.at(gChan),
                imageMap_.at(bChan)
            });
        }

        views.emplace_back(row, col, img, grayscale);
    }

    return views;
}
