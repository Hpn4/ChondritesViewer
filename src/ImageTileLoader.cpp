#include "ImageTileLoader.h"
#include <stdexcept>

using namespace vips;

ImageTileLoader::ImageTileLoader(const std::string &path) {
    if(VIPS_INIT("ImageTileLoader"))
        throw std::runtime_error("Impossible d'initialiser libvips");

    // Charger l'image telle quelle (RGB)
    VImage img = VImage::new_from_file(path.c_str(),
                                       VImage::option()->set("access","sequential"));

    width_ = img.width();
    height_ = img.height();

    // Écrire en mémoire en RGB
    data_ = img.write_to_memory(nullptr);
}

ImageTileLoader::~ImageTileLoader() {
    if(data_) g_free(data_);
}
