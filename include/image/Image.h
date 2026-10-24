//
// Created by Rushin Shah on 24/10/26.
//

#ifndef OPENGL_IMAGE_H
#define OPENGL_IMAGE_H
#include <string>
#include "image_loader.h"

class Image {
public:
    explicit Image(const std::string& loc);
    inline unsigned char* get_image_data(int& width, int& height, int& nchannels) const noexcept;
    ~Image();
private:
    unsigned char* data;
    int width, height, nchannels;
    inline void load_image(const std::string& loc) noexcept;
};

#endif //OPENGL_IMAGE_H
