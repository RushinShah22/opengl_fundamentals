//
// Created by Rushin Shah on 24/10/26.
//

#include "shader/Shader.h"
#include <iostream>

Shader::Shader(const std::string& vert_shader_loc, const std::string& frag_shader_loc): is_program_successfully_created(false) {
    vert_shader_file.open(vert_shader_loc);
    frag_shader_file.open(frag_shader_loc);
}

inline void Shader::create_program() noexcept {
    int success = read_shader_src(&vert_shader_file, &vert_shader_src);
    if (!success) {
        std::cout << "ERROR::PROGRAM::READING::SHADER::VERTEX" << std::endl;
        return;
    }
    success = read_shader_src(&frag_shader_file, &frag_shader_src);
    if (!success) {
        std::cout << "ERROR::PROGRAM::READING::SHADER::FRAGMENT" << std::endl;
        return;
    }
    unsigned int vert_shader, frag_shader;
    success = compile_shader(&vert_shader_src, GL_VERTEX_SHADER, &vert_shader);
    if (!success) {
        std::cout << "ERROR::PROGRAM::COMPILING::SHADER::VERTEX" << std::endl;
        return;
    }
    success = compile_shader(&frag_shader_src, GL_FRAGMENT_SHADER, &frag_shader);
    if (!success) {
        std::cout << "ERROR::PROGRAM::COMPILING::SHADER::FRAGMENT" << std::endl;
        return;
    }
    ID = glCreateProgram();
    glAttachShader(ID, vert_shader);
    glAttachShader(ID, frag_shader);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(ID, 512, nullptr, log);
        std::cout << "ERROR::LINKING::PROGRAM " << log << std::endl;
        return;
    }
    is_program_successfully_created = true;
}

inline int Shader::read_shader_src(std::ifstream *file, std::string *src) noexcept {
    if (!file->is_open()) {
        std::cout << "ERROR::SHADER::FILE unable to open!!!" <<  std::endl;
        return 0;
    }
    std::string line;
    while (std::getline(*file, line)) {
        if (*src != "") *src += "\n";
        *src += line;
    }
    return 1;
}

inline int Shader::compile_shader(const std::string *src, const GLuint shader_type, GLuint *shader) const noexcept {
    *shader = glCreateShader(shader_type);
    const char * src_c = src->c_str();
    glShaderSource(*shader, 1, &src_c, nullptr);
    glCompileShader(*shader);
    int success;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(*shader, 512, nullptr, log);
        std::cout << "ERROR::COMPILING::SHADER " << log << std::endl;
        return 0;
    }
    return 1;
}

inline int Shader::use() const noexcept {
    if (!is_program_successfully_created) {
        return -1;
    }
    glUseProgram(ID);
    return 1;
}

inline void Shader::set_int(const GLint loc, const int val) {
    glUniform1i(loc, val);
}
