//
// Created by Rushin Shah on 24/10/26.
//

#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include <string>
#include <glad/glad.h>

#include "image/image_loader.h"

class Texture {
public:
    inline void set_texture(unsigned char* image_data, int width, int height, unsigned int image_color_format, GLuint texture_loc) noexcept;
    inline void set_texture_image_style_S(GLuint style);
    inline void set_texture_image_style_T(GLuint style);
    inline void set_texture_min_style(GLuint style);
    inline void set_texture_mag_style(GLuint style);
private:
    GLuint image_style_S = GL_MIRRORED_REPEAT, image_style_T = GL_MIRRORED_REPEAT, min_style = GL_LINEAR_MIPMAP_NEAREST, mag_style = GL_LINEAR;
};
#endif //OPENGL_TEXTURE_H
