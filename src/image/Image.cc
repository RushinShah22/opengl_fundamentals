//
// Created by Rushin Shah on 24/10/26.
//

#include "image/Image.h"
#include <iostream>

Image::Image(const std::string& loc) {
    load_image(loc);
}

inline void Image::load_image(const std::string &loc) noexcept {
    const char * loc_c = loc.c_str();
    data = stbi_load(loc_c, &width, &height, &nchannels, 0);
    if (!data) {
        std::cout << "Fail to load image: " << loc << std::endl;
    }
}

inline unsigned char* Image::get_image_data(int& width, int& height, int&nchannels) const noexcept {
    width = this->width;
    height = this->height;
    nchannels = this->nchannels;
    return data;
}

Image::~Image() {
    stbi_image_free(data);
}
