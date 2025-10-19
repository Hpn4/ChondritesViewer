#pragma once

#include <vips/vips8>
#include <string>

class ImageTileLoader {
public:
    ImageTileLoader(const std::string &path);
    ~ImageTileLoader();

    int width() const { return width_; }
    int height() const { return height_; }

    const unsigned char* data() const { return static_cast<const unsigned char*>(data_); }

private:
    int width_;
    int height_;
    void* data_ = nullptr;
};
