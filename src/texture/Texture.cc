//
// Created by Rushin Shah on 24/10/26.
//
#include "texture/Texture.h"


void Texture::set_texture(unsigned char *image_data, const int width, const int height, const unsigned int image_color_format, const unsigned int texture_number) noexcept {
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(texture_number);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, image_style_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, image_style_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_style);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_style);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, image_color_format, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::set_texture_image_style_S(const GLuint style) {
    image_style_S = style;
}
void Texture::set_texture_image_style_T(const GLuint style) {
    image_style_T = style;
}
void Texture::set_texture_min_style(const GLuint style) {
    min_style = style;
}
void Texture::set_texture_mag_style(const GLuint style) {
    mag_style = style;
}
