#pragma once

#include <string>
#include <map>
#include <vips/vips8>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;
using namespace vips;

class ImageLoader {
public:
    explicit ImageLoader(const std::string &path);

    const std::map<std::string, VImage>& getMap() const { return imageMap_; }

    std::vector<std::tuple<int,int,VImage,bool>> prepareViews() const;

    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

private:
    std::string path_;
    std::map<std::string, VImage> imageMap_;
    nlohmann::json data_;
    int width_ = 0;
    int height_ = 0;

    VImage loadSingle(const std::string &path);

    void load();
};
